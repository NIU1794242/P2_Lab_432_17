#include "board.h"
#include <memory>
#include <iostream>

Board::Board(int width, int height) : m_width(DEFAULT_BOARD_WIDTH), m_height(DEFAULT_BOARD_HEIGHT),
m_candyRed(CandyType::TYPE_RED), m_candyGreen(CandyType::TYPE_GREEN), m_candyBlue(CandyType::TYPE_BLUE), 
m_candyYellow(CandyType::TYPE_YELLOW), m_candyPurple(CandyType::TYPE_PURPLE), m_candyOrange(CandyType::TYPE_ORANGE)
{
    // Implement your code here
    if (width > 0)
        m_width = width;

    if (height > 0)
        m_height = height;

    m_board = new Candy**[m_height];
    for (int y = 0; y < m_height; y++)
    {
        m_board[y] = new Candy*[m_width];
        for (int x = 0; x < m_width; x++)
            m_board[y][x] = nullptr;
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
                    if (m_board[y][x] != nullptr)
                        delete m_board[y][x];

                delete[] m_board[y];
            }
        }
        delete[] m_board;
    }
    m_board = nullptr;
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
        for (int y = 0; y < m_height; y++)
        {
            m_board[y] = new Candy*[m_width];
            for (int x = 0; x < m_width; x++)
                m_board[y][x] = nullptr;
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
        m_board[y][x] = candy;
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
                    setCell(nullptr, x, y);
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
                        y--;
                    }
                }
                while (y >= 0)
                {
                    setCell(nullptr, x, y);
                    y--;
                }
            }
        }
    } while (explosion);

    for (Candy* explodedCandy : exploded)
        delete explodedCandy;

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
            if (currCandy == nullptr)
                file << '-';
            else
            {
                switch (currCandy->getType())
                {
                case CandyType::TYPE_RED: file << 'R'; break;
                case CandyType::TYPE_GREEN: file << 'G'; break;
                case CandyType::TYPE_BLUE: file << 'B'; break;
                case CandyType::TYPE_YELLOW: file << 'Y'; break;
                case CandyType::TYPE_PURPLE: file << 'P'; break;
                case CandyType::TYPE_ORANGE: file << 'O'; break;
                default: file << '-'; break;
                }
            }
            file << ' ';
        }
        file << std::endl;
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

    char charCandy;
    CandyType type;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            file >> charCandy;
            switch (charCandy)
            {
            case 'R': type = CandyType::TYPE_RED; break;
            case 'G': type = CandyType::TYPE_GREEN; break;
            case 'B': type = CandyType::TYPE_BLUE; break;
            case 'Y': type = CandyType::TYPE_YELLOW; break;
            case 'P': type = CandyType::TYPE_PURPLE; break;
            case 'O': type = CandyType::TYPE_ORANGE; break;
            default: type = CandyType::COUNT; break;
            }
            Candy* newCandy = type != CandyType::COUNT ? new Candy(type) : nullptr;
            setCell(newCandy, x, y);
        }
    }

    file.close();
    
    return true;
}