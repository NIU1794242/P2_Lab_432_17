#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include "util.h"
#include <fstream>
#include <cstdlib>   // rand, srand
#include <ctime>     // time


Game::Game()
{
    srand((unsigned int)time(nullptr));  // semilla aleatoria, una sola vez

    m_board = new Board();
    m_blockX = SPAWN_COLUMN;
    m_blockY = -BLOCK_SIZE;
    m_frameCounter = 0;
    m_gameOver = false;
    m_score = 0;

    for (int i = 0; i < BLOCK_SIZE; ++i)
        m_block[i] = nullptr;

    spawnBlock();
}

Game::~Game() //PUEDE SER QUE EL PROGRAMA SE RALLE CON LOS DELETES
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
    // Si la parte de arriba está fuera del tablero, no cabe -> fin del juego
    if (m_blockY < 0)
    {
        m_gameOver = true;
        return;  // los caramelos del bloque se borran en el destructor
    }

    // Congelamos el bloque en el tablero (ahora los caramelos son del Board)
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        m_board->setCell(m_block[i], m_blockX, m_blockY + i);  // el Board copia
        delete m_block[i];                                     // borras tu original
        m_block[i] = nullptr;
    }

    resolveExplosions();
    spawnBlock();
}

void Game::resolveExplosions()
{
    std::vector<Candy*> explotados = m_board->explodeAndDrop();
    m_score += (int)explotados.size();

    // NO borramos nada: el Board ya ha borrado estos caramelos
    // dentro de explodeAndDrop (con setCell(nullptr, ...)).
}


//          ---Bucle del juego---
void Game::update(const Controller& controller)
{
    if (m_gameOver)
        return;

    // Mover a izquierda / derecha
    if (controller.isLeftPressed() && canMoveTo(m_blockX - 1, m_blockY))
        m_blockX = m_blockX - 1;
    if (controller.isRightPressed() && canMoveTo(m_blockX + 1, m_blockY))
        m_blockX = m_blockX + 1;

    // Rotar el bloque (tecla Q)
    if (controller.isKey1Pressed())
        rotateBlock();

    // Guardar partida en data/save.txt (tecla W)
    if (controller.isKey2Pressed())
        dump(getDataDirPath() + "save.txt");

    // Bajar: 1 vez cada 60 frames, o ya mismo si pulsas Abajo
    m_frameCounter = m_frameCounter + 1;
    bool bajar = (m_frameCounter >= FRAMES_PER_DROP) || controller.isDownPressed();

    if (bajar)
    {
        m_frameCounter = 0;
        if (canMoveTo(m_blockX, m_blockY + 1))
            m_blockY = m_blockY + 1;
        else
            landBlock();
    }
}

void Game::render(GraphicManager& graphics) //wtf que es esto?
{
    int pad = 3;
    int originX = CANDY_IMAGE_WIDTH * pad;
    int originY = CANDY_IMAGE_HEIGHT * pad;
    int w = m_board->getWidth();
    int h = m_board->getHeight();

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
    if (!m_gameOver)
        for (int i = 0; i < BLOCK_SIZE; ++i)
            if (m_block[i] != nullptr)
                graphics.drawImage(m_block[i]->getResourceName(),
                    originX + m_blockX * CANDY_IMAGE_WIDTH,
                    originY + (m_blockY + i) * CANDY_IMAGE_HEIGHT);

    // Textos
    graphics.drawImage("img/logo_small.png", 10, 10);
    if (m_gameOver)
        graphics.drawText("GAME OVER", 25, 25, 40, 240, 130, 130);
    graphics.drawText("Score: " + std::to_string(m_score), 450, 10, 40, 125, 200, 125);
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  "
        "Buttons: [Q] [W] [E]  --  Exit [ESC]",
        25, 700, 20, 100, 100, 100);
}

void Game::run() //wtf que es esto?
{
    const int screen_width = 750;
    const int screen_height = 750;
    runGraphicGame(*this, screen_width, screen_height, 255, 255, 0);
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
                m_board->setCell(nullptr, x, y);   // setCell ya borra lo que hubiera
            else
            {
                Candy temp((CandyType)code);       // caramelo temporal en la pila
                m_board->setCell(&temp, x, y);     // setCell hace SU copia; temp se destruye solo
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