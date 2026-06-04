#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include "util.h"
#include <fstream>
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include "sound.h"

// Carga un efecto de sonido por nombre, relativo a data/img/Audios/
static T_SOUND* cargarSonido(const std::string& nombre)
{
    std::string full = getDataDirPath() + "img/Audios/" + nombre;
    return Sound_LoadSound(&full[0]);   // Sound_LoadSound pide char*
}

// Carga y arranca una OST (música en streaming)
static T_SOUND* cargarMusica(const std::string& nombre)
{
    std::string full = getDataDirPath() + "img/Audios/" + nombre;
    return Sound_LoadMusic(&full[0], PLAY_FROM_DISK_STREAM);
}

// ARREGLAMOS SONIDOS ORBES QUE SE PIERDE
// Existen en sound.cpp pero no están en sound.h; las declaramos para usarlas.
extern void Sound_Lock();
extern void Sound_Unlock();

// Reproduce un efecto protegiendo la lista de canales frente al hilo de audio
static int reproducirEfecto(T_SOUND* s, int mode)
{
    if (s == nullptr) return -1;
    Sound_Lock();
    int slot = Sound_Play(s, mode);   // el lock de SDL es recursivo, no pasa nada
    Sound_Unlock();
    return slot;
}

Game::Game()
{
    srand((unsigned int)time(nullptr));  // semilla aleatoria, una sola vez

    m_board = new Board();
    m_blockX = SPAWN_COLUMN;
    m_blockY = -BLOCK_SIZE;
    m_frameCounter = 0;
    m_gameOver = false;
    m_score = 0; //SCORE
    hardMode = false;

    for (int i = 0; i < BLOCK_SIZE; ++i)
        m_block[i] = nullptr;

    spawnBlock();
}

Game::~Game() 
{
    freeBlock();     // borra los caramelos del bloque que aún tengamos
    delete m_board;  // el destructor de Board borra los de las celdas
}

// Crea un bloque nuevo arriba del tablero, con 3 caramelos al azar
void Game::spawnBlock()
{
    m_blockX = SPAWN_COLUMN;
    m_blockY = -BLOCK_SIZE;  // del todo fuera: filas -3, -2, -1

    int numTypes = (int)CandyType::COUNT;  // 6 tipos
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        int t = rand() % numTypes;         // 0..5
        m_block[i] = new Candy((CandyType)t);
    }
    m_frameCounter = 0;
}

// Borra los 3 caramelos del bloque (si los hay)
void Game::freeBlock()
{
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        delete m_block[i];   // borrar nullptr no pasa nada
        m_block[i] = nullptr;
    }
}

// Gira ABC -> BCA
void Game::rotateBlock()
{
    Candy* arriba = m_block[0];
    m_block[0] = m_block[1];
    m_block[1] = m_block[2];
    m_block[2] = arriba;
}

// ¿Pueden los 3 caramelos estar en la columna newX, filas newY, newY+1, newY+2?
bool Game::canMoveTo(int newX, int newY) const
{
    // Fuera del tablero por los lados -> no
    if (newX < 0 || newX >= m_board->getWidth())
        return false;

    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        int fila = newY + i;

        if (fila >= m_board->getHeight())   // se sale por abajo
            return false;

        // Si la fila está dentro del tablero y hay un caramelo -> choca
        if (fila >= 0 && m_board->getCell(newX, fila) != nullptr)
            return false;

        // fila < 0 -> todavía por encima del tablero, está libre
    }
    return true;
}

// ---------- Aterrizaje y explosiones ----------

void Game::landBlock()
{
    // Colocamos en el tablero los caramelos que caben (filas >= 0).
    // Los que quedan por encima del tablero se descartan (overflow).
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        m_board->generateCandy(m_block[i], m_blockX, m_blockY + i);  // ignora filas < 0
        delete m_block[i];
        m_block[i] = nullptr;
    }


    // Game over solo si el bloque desbordó por arriba Y, tras explotar,
    // la cima de esa columna SIGUE ocupada (no hubo hueco).
    if (m_blockY < 0 && m_board->getCell(m_blockX, 0) != nullptr)
    {
        m_gameOver = true;
        if (m_soundReady && m_sndGameOver != nullptr)
            reproducirEfecto(m_sndGameOver, SOUND_PLAY_NORMAL);
        return;
    }

    resolveExplosions();
    spawnBlock();
}

void Game::resolveExplosions()
{
    std::vector<Candy*> explotados = m_board->explodeAndDrop();
    const int PUNTOS_POR_CARAMELO = 1;   //IMPORTANTE MODIFICAR ESTO ANTES DE ENTREGA
    m_score += (int)explotados.size() * PUNTOS_POR_CARAMELO;

    // Sonido al explotar candys
    if (!explotados.empty() && m_soundReady && m_sndExplode != nullptr)
        reproducirEfecto(m_sndExplode, SOUND_PLAY_NORMAL);

    // NO borramos nada: el Board ya ha borrado estos caramelos
    // dentro de explodeAndDrop (con setCell(nullptr, ...)).
}

//          ---Bucle del juego---
void Game::update(const Controller& controller)
{

    initSound();             // se inicializa una sola vez (primer frame)
    updateSoundForScore();   // dispara rangos y cambia la OST

    if (m_gameOver)
        return;

    // Hard Mode (tecla V)
    if (controller.isKey4Pressed())
        hardMode = !hardMode;

    // Mover a izquierda / derecha
    if (controller.isLeftPressed() && canMoveTo(m_blockX - 1, m_blockY))
        m_blockX = m_blockX - 1;
    if (controller.isRightPressed() && canMoveTo(m_blockX + 1, m_blockY))
        m_blockX = m_blockX + 1;

    // Rotar el bloque (tecla Q)
    if (controller.isKey1Pressed())
        rotateBlock();

    // Guardar partida en data/save.txt o en archivo de la consola de comnados (tecla W)
    string filePath;
    if (controller.isKey2Pressed())
        /*if (cin >> filePath)
            dump(filePath);
        else*/
            dump(getDataDirPath() + "save.txt");

    // Cargar partida de data/save.txt o de archivo de la consola de comnados (tecla W)
    if(controller.isKey3Pressed())
        /*if (cin >> filePath)
            load(filePath);
        else*/
            load(getDataDirPath() + "save.txt");

    m_frameCounter = m_frameCounter + 1;
    bool bajar = (m_frameCounter >= (int)((float)FRAMES_PER_DROP/(1.0 + (float)hardMode * (float)m_score/DIFFICULTY)) || controller.isDownPressed());
    // Hard drop, aterriza directamente (tecla SPACE)
    if (controller.isSpacePressed())
    {
        while (canMoveTo(m_blockX, m_blockY + 1))
            m_blockY++;
        landBlock();
    }
    // Bajar: 1 vez cada 60 frames, o ya mismo si pulsas Abajo
    else if (bajar)
    {
        m_frameCounter = 0;
        if (canMoveTo(m_blockX, m_blockY + 1))
            m_blockY = m_blockY + 1;
        else
            landBlock();
    }
}

void Game::render(GraphicManager& graphics) 
{
    int pad = 3;
    int originX = CANDY_IMAGE_WIDTH * pad;
    int originY = CANDY_IMAGE_HEIGHT * pad;
    int w = m_board->getWidth();
    int h = m_board->getHeight();

    string fondo;
    if (!hardMode)
        fondo = "img/candy/fondo.png";
    else
        fondo = "img/candy/fondo_hard_mode.png";
    graphics.drawImage(fondo , 0, 0);
    drawEasterEgg(graphics, 225, -14);   

    // Borde del tablero
    graphics.drawRectangle(originX, originY,
        CANDY_IMAGE_WIDTH * w, CANDY_IMAGE_HEIGHT * h,
        5, 150, 150, 150);

    // Caramelos que ya están en el tablero
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            Candy* c = m_board->getCell(x, y);
            if (c != nullptr)
                graphics.drawImage(c->getResourceName(),
                    originX + x * CANDY_IMAGE_WIDTH,
                    originY + y * CANDY_IMAGE_HEIGHT);
        }

    // El bloque que cae (puede estar por encima del tablero)
    if(!m_gameOver)
        for (int i = 0; i < BLOCK_SIZE; ++i)
            if (m_block[i] != nullptr)
                graphics.drawImage(m_block[i]->getResourceName(),
                    originX + m_blockX * CANDY_IMAGE_WIDTH,
                    originY + (m_blockY + i) * CANDY_IMAGE_HEIGHT);

    // Textos
    graphics.drawImage("img/logo_small.png", 10, 10);
    if (m_gameOver)
        graphics.drawText("GAME OVER", 265, 315, 40, 240, 130, 130);
    graphics.drawText("Score: " + std::to_string(m_score), 450, 10, 40, 125, 200, 125);
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
        "Buttons: [Q] [W] [E]  --  Exit [ESC]",
        25, 700, 20, 100, 100, 100);
    graphics.drawText("Hard Drop: [SPACE]  --  Alternate Hard Mode: [V]", 25, 678, 20, 100, 100, 100);
    graphics.drawText("Devil May Cry 5 Edition", 25, 640, 30, 100, 100, 100);

    //Dibujar Rangos
    graphics.drawText("Score: " + std::to_string(m_score), 450, 10, 40, 230, 230, 230);
    drawRank(graphics, 620, 60);   // Dibujar rango x, y
}

void Game::run() 
{
    const int screen_width = 750;
    const int screen_height = 750;
    runGraphicGame(*this, screen_width, screen_height, 255, 255, 255);
}

bool Game::dump(const std::string& output_path) const
{
    std::ofstream out(output_path);          // ofstream = ESCRIBIR
    if (!out.is_open())
        return false;

    int w = m_board->getWidth();
    int h = m_board->getHeight();
    out << w << " " << h << "\n";

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            Candy* c = m_board->getCell(x, y);
            int code = (c != nullptr) ? (int)c->getType() : -1;
            out << code << " ";
        }
        out << "\n";
    }

    int hayBloque = (m_block[0] != nullptr) ? 1 : 0;
    out << hayBloque << "\n";
    if (hayBloque == 1)
    {
        out << m_blockX << " " << m_blockY;
        for (int i = 0; i < BLOCK_SIZE; ++i)
            out << " " << (int)m_block[i]->getType();
        out << "\n";
    }

    out << (m_gameOver ? 1 : 0) << "\n";
    return true;
}

bool Game::load(const std::string& input_path)
{
    std::ifstream in(input_path);            // ifstream = LEER
    if (!in.is_open())
        return false;

    int w, h;
    in >> w >> h;

    freeBlock();   // libera el bloque actual (sin fugas)

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            int code;
            in >> code;
            if (code == -1)
                m_board->generateCandy(nullptr, x, y);   // generateCandy ya borra lo que hubiera
            else
            {
                Candy temp((CandyType)code);       // caramelo temporal en la pila
                m_board->generateCandy(&temp, x, y);     // generateCandy hace SU copia; temp se destruye solo
            }
        }

    int hayBloque;
    in >> hayBloque;
    if (hayBloque == 1)
    {
        in >> m_blockX >> m_blockY;
        for (int i = 0; i < BLOCK_SIZE; ++i)
        {
            int t;
            in >> t;
            m_block[i] = new Candy((CandyType)t);  // el bloque SÍ lo gestiona Game
        }
    }

    int fin;
    in >> fin;
    m_gameOver = (fin == 1);

    m_frameCounter = 0;
    return true;
}

bool Game::operator==(const Game& other) const
{
    // Mismas dimensiones
    if (m_board->getWidth() != other.m_board->getWidth())
        return false;
    if (m_board->getHeight() != other.m_board->getHeight())
        return false;

    int w = m_board->getWidth();
    int h = m_board->getHeight();

    // Mismo contenido en cada celda (comparamos por TIPO, no por puntero)
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            Candy* a = m_board->getCell(x, y);
            Candy* b = other.m_board->getCell(x, y);

            bool aVacia = (a == nullptr);
            bool bVacia = (b == nullptr);
            if (aVacia != bVacia)
                return false;
            if (!aVacia && a->getType() != b->getType())
                return false;
        }

    // Mismo bloque
    bool aHay = (m_block[0] != nullptr);
    bool bHay = (other.m_block[0] != nullptr);
    if (aHay != bHay)
        return false;
    if (aHay)
    {
        if (m_blockX != other.m_blockX || m_blockY != other.m_blockY)
            return false;
        for (int i = 0; i < BLOCK_SIZE; ++i)
            if (m_block[i]->getType() != other.m_block[i]->getType())
                return false;
    }

    if (m_gameOver != other.m_gameOver)
        return false;

    return true;
}

void Game::initSound()
{
    if (m_soundReady) return;
    m_soundReady = true;          // lo marcamos ya, para no reintentar

    Sound_Init();                 // ¡UNA sola vez en todo el programa!

    // Sonidos de rango (índices 1..7 = D..SSS)
    const char* nombres[8] = { nullptr, "D", "C", "B", "A", "S", "SS", "SSS" };
    for (int r = 1; r <= 7; ++r)
        m_rankSounds[r] = cargarSonido(std::string(nombres[r]) + ".ogg");

    m_sndExplode = cargarSonido("orbs_def.ogg");
    m_sndGameOver = cargarSonido("Pathetic.ogg");

    // OST inicial: Devil_Trigger empieza a sonar al cargarse
    m_ostDevil = cargarMusica("Devil_Trigger.ogg");
    m_lastOst = 0;
}

int Game::rankFromScore(int score) const
{
    if (score >= 200) return 7;   // SSS
    if (score >= 100)  return 6;   // SS
    if (score >= 60)  return 5;   // S
    if (score >= 40)  return 4;   // A
    if (score >= 30)  return 3;   // B
    if (score >= 20)  return 2;   // C
    if (score >= 10)  return 1;   // D
    return 0;                     // sin rango
}

void Game::updateSoundForScore()
{
    if (!m_soundReady) return;

    // Sonido de rango: SOLO cuando subimos de rango
    int rank = rankFromScore(m_score);
    if (rank > m_lastRank && m_rankSounds[rank] != nullptr)
        reproducirEfecto(m_rankSounds[rank], SOUND_PLAY_NORMAL);
    m_lastRank = rank;

    // OST según puntuación: Devil (0), Silver (>=60), Bury (>=90)
    int ost = 0;
    if (m_score >= 140) ost = 2;
    else if (m_score >= 60) ost = 1;

    if (ost != m_lastOst)
    {
        if (ost == 1)      m_ostSilver = cargarMusica("Silver_Bullet.ogg");
        else if (ost == 2) m_ostBury = cargarMusica("Bury_the_Light.ogg");
        m_lastOst = ost;
    }
}
// Carpeta de los PNG de las letras de rango: la MISMA que los candys.
// Ajústala si tus candys están en otra carpeta (ver nota abajo).
static const std::string RANK_IMG_DIR = "img/candy/";

void Game::drawRank(GraphicManager& graphics, int x, int y) const
{
    int rank = rankFromScore(m_score);
    if (rank == 0) return;   // todavía sin rango (score < 10): no dibujamos nada

    const char* letras[8] = { "", "D", "C", "B", "A", "S", "SS", "SSS" };
    std::string resource = RANK_IMG_DIR + letras[rank] + ".png";
    graphics.drawImage(resource, x, y);
}

void Game::drawEasterEgg(GraphicManager& graphics, int x, int y) const
{
    if (rankFromScore(m_score) == 7)   // 7 = SSS (score >= 100)
        graphics.drawImage(RANK_IMG_DIR + "Vergil_def.png", x, y);
}