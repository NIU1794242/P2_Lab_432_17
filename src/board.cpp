#include "board.h"
#include <memory>
#include <iostream>

Board::Board(int width, int height) : m_width(DEFAULT_BOARD_WIDTH), m_height(DEFAULT_BOARD_HEIGHT)
{
    // Implement your code here
    if (width > 0)
        m_width = width;

    if (height > 0)
        m_height = height;

    m_board = new Candy**[m_height];
    m_originals = new bool*[m_height];
    for (int y = 0; y < m_height; y++)
    {
        m_board[y] = new Candy*[m_width];
        m_originals[y] = new bool[m_width];
        for (int x = 0; x < m_width; x++)
        {
            m_board[y][x] = nullptr;
            m_originals[y][x] = true;
        }
    }
}

Board::~Board()
{
    // Implement your code here
    clear();
    
}

void Board::clear()
{
    if (m_board != nullptr)
    {
        for (int y = 0; y < m_height; y++)
        {
            if (m_board[y] != nullptr)
            {
                for (int x = 0; x < m_width; x++)
                    setCell(nullptr, x, y);

                delete[] m_board[y];
            }
        }
        delete[] m_board;
    }
    if (m_originals != nullptr)
    {
        for (int y = 0; y < m_height; y++)
            if (m_originals[y] != nullptr)
                delete[] m_originals[y];
        delete[] m_originals;
    }
    m_board = nullptr;
    m_originals = nullptr;
    m_width = 0;
    m_height = 0;
}

bool Board::clearAndResize(int width, int height)
{
    bool canResize = width > 0 && height > 0;
    if (canResize)
    {
        clear();
        m_width = width;
        m_height = height;
        m_board = new Candy**[m_height];
        m_originals = new bool* [m_height];
        for (int y = 0; y < m_height; y++)
        {
            m_board[y] = new Candy * [m_width];
            m_originals[y] = new bool[m_width];
            for (int x = 0; x < m_width; x++)
            {
                m_board[y][x] = nullptr;
                m_originals[y][x] = true;
            }
        }
    }
    return canResize;
}

bool Board::isValidPosition(int x, int y) const
{
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

Candy* Board::getCell(int x, int y) const
{
    // Implement your code here
    Candy* cell = nullptr;
    if (isValidPosition(x, y))
        cell = m_board[y][x];
    return cell;
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here
    if (isValidPosition(x, y))
    {
        if (m_board[y][x] != nullptr && m_originals[y][x])
            delete m_board[y][x];
        m_originals[y][x] = candy == nullptr;
        m_board[y][x] = candy;
    }
}

void Board::generateCandy(Candy* candy, int x, int y)
{
    // Implement your code here
    if (isValidPosition(x, y))
    {
        if (m_board[y][x] != nullptr && m_originals[y][x])
            delete m_board[y][x];
        m_originals[y][x] = true;
        m_board[y][x] = candy != nullptr ? new Candy(candy->getType()) : nullptr;
    }
}

int Board::getWidth() const
{
    // Implement your code here
    return m_width;
}

int Board::getHeight() const
{
    // Implement your code here
    return m_height;
}
// Transformar en funciones
bool Board::shouldExplode(int x, int y) const
{
    bool explode = false;
    
    if (isValidPosition(x, y) && m_board[y][x] != nullptr)
    {
        CandyType type = m_board[y][x]->getType();
        
        explode = shouldExplodeVertical(x, y, type);
        if (!explode)
            explode = shouldExplodeHorizontal(x, y, type);
        if (!explode)
            explode = shouldExplodeDiagonalDownTop(x, y, type);
        if (!explode)
            explode = shouldExplodeDiagonalTopDown(x, y, type);
    }
    return explode;
}

bool Board::shouldExplodeVertical(int x, int y, CandyType type) const
{
    int count = 1;
    int steps = 1;
    while (y + steps < m_height && m_board[y + steps][x] != nullptr && m_board[y + steps][x]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    steps = 1;
    while (y - steps >= 0 && m_board[y - steps][x] != nullptr && m_board[y - steps][x]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    return count >= SHORTEST_EXPLOSION_LINE;
}

bool Board::shouldExplodeHorizontal(int x, int y, CandyType type) const
{
    int count = 1;
    int steps = 1;
    while (x + steps < m_width && m_board[y][x + steps] != nullptr && m_board[y][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    steps = 1;
    while (x - steps >= 0 && m_board[y][x - steps] != nullptr && m_board[y][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    return count >= SHORTEST_EXPLOSION_LINE;
}

bool Board::shouldExplodeDiagonalTopDown(int x, int y, CandyType type) const
{
    int count = 1;
    int steps = 1;
    while (y + steps < m_height && x + steps < m_width && m_board[y + steps][x + steps] != nullptr && m_board[y + steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    steps = 1;
    while (y - steps >= 0 && x - steps >= 0 && m_board[y - steps][x - steps] != nullptr && m_board[y - steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    return count >= SHORTEST_EXPLOSION_LINE;
}

bool Board::shouldExplodeDiagonalDownTop(int x, int y, CandyType type) const
{
    int count = 1;
    int steps = 1;
    while (y + steps < m_height && x - steps >= 0 && m_board[y + steps][x - steps] != nullptr && m_board[y + steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    steps = 1;
    while (y - steps >= 0 && x + steps < m_width && m_board[y - steps][x + steps] != nullptr && m_board[y - steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
    {
        count++;
        steps++;
    }
    return count >= SHORTEST_EXPLOSION_LINE;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    std::vector<Candy*> exploded;
    std::vector<std::vector<bool>> toExplode (m_height, std::vector<bool>(m_width, false));
    bool explosion = false;
    do
    {
        // deteccion/marca de explosiones
        explosion = false;
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
                toExplode[y][x] = shouldExplode(x, y);

        // eliminar (explotar) candy marcados
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
                if (toExplode[y][x])
                {
                    explosion = true;
                    exploded.push_back(m_board[y][x]);
                    generateCandy(nullptr, x, y);
                }

        // hacer caer los candy
        if (explosion)
        {
            for (int x = 0; x < m_width; x++)
            {
                int y = m_height - 1;
                int shift = 0;
                while (y-shift >= 0)
                {
                    while (y - shift >= 0 && m_board[y-shift][x] == nullptr)
                        shift++;
                    if (y - shift >= 0)
                    {
                        m_board[y][x] = m_board[y - shift][x];
                        m_originals[y][x] = m_originals[y - shift][x];
                        y--;
                    }
                }
                while (y >= 0)
                {
                    m_board[y][x] = nullptr;
                    m_originals[y][x] = true;
                    y--;
                }
            }
        }
    } while (explosion);

    return exploded;
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    std::ofstream file;
    file.open(output_path);
    if (!file.is_open())
        return false;


    file << m_width << ' ' << m_height << std::endl;
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            Candy* currCandy = getCell(x, y);
            int code = (currCandy != nullptr) ? (int)currCandy->getType() : -1;
            file << code << " ";

        }
        file << "\n";
    }

    file.close();

    return true;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    std::ifstream file;
    file.open(input_path);
    if (!file.is_open())
        return false;

    int width, height;
    file >> width >> height;
    if(!clearAndResize(width, height))
    {
        file.close();
        return false;
    }

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            int code;
            file >> code;
            if (code == -1)
                setCell(nullptr, x, y);   // setCell ya borra lo que hubiera
            else
            {
                Candy temp((CandyType)code);       // caramelo temporal en la pila
                generateCandy(&temp, x, y);     // setCell hace SU copia; temp se destruye solo
            }
    }

    file.close();
    
    return true;
}