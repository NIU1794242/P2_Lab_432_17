#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
{
    // Implement your code here
    if (width <= 0)
        m_width = DEFAULT_BOARD_WIDTH;
    else if (width > MAX_SIZE)
        m_width = MAX_SIZE;
    else
        m_width = width;

    if (height <= 0)
        m_height = DEFAULT_BOARD_HEIGHT;
    else if (height > MAX_SIZE)
        m_height = MAX_SIZE;
    else
        m_height = height;

    m_storage.reserve(m_height);
    for (int i = 0; i < m_height; i++)
    {
        m_storage.reserve(m_width);
    }

    for (int i = 0; i < m_height; i++)
        for (int j = 0; j < m_width; j++)
            m_board[i][j] = nullptr;
}

Board::~Board()
{
    // Implement your code here
    for (int i = 0; i < m_height; i++)
        for (int j = 0; j < m_width; j++)
            m_board[i][j] = nullptr;
}


Candy* Board::getCell(int x, int y) const
{
    // Implement your code here
    Candy* cell = nullptr;
    if (x >= 0 && x < m_width && y >= 0 && x < m_height)
        cell = m_board[y][x];
    return cell;
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here
    if (x >= 0 && x < m_width && y >= 0 && x < m_height)
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

bool Board::shouldExplode(int x, int y) const
{
    bool explode = false;

    if (x >= 0 && x < m_width && y >= 0 && x < m_height)
    {
        CandyType type = m_board[y][x]->getType();
        int count = 1;
        int steps = 1;
        // N/S
        while (y + steps < m_height && m_board[y + steps][x]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        steps = 1;
        while (y - steps >= 0 && m_board[y - steps][x]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        explode = count >= SHORTEST_EXPLOSION_LINE;
        // E/W
        count = 1;
        steps = 1;
        while (!explode && x + steps < m_width && m_board[y][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        steps = 1;
        while (!explode && x - steps >= 0 && m_board[y][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        explode = count >= SHORTEST_EXPLOSION_LINE;
        // NE/SW
        count = 1;
        steps = 1;
        while (!explode && y + steps < m_height && x + steps < m_width && m_board[y + steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        steps = 1;
        while (!explode && y - steps >= 0 && x - steps >= 0 && m_board[y - steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        explode = count >= SHORTEST_EXPLOSION_LINE;
        // NW/SE
        while (!explode && y + steps < m_height && x - steps < m_width && m_board[y + steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        steps = 1;
        while (!explode && y - steps >= 0 && x + steps >= 0 && m_board[y - steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        explode = count >= SHORTEST_EXPLOSION_LINE;
    }
    return explode;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    return {};
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    
    bool correcte = true;
    char charCandy = '-';
    std::ofstream file(output_path);
    if (file.is_open())
    {
        for (int i = 0; i < m_width; i++)
        {
            int j = m_height-1;
            while (m_board[j][i] != nullptr)
            {
                switch ((*m_board[j][i]).getType())
                {
                case CandyType::TYPE_RED: charCandy = 'R'; break;
                case CandyType::TYPE_GREEN: charCandy = 'G'; break;
                case CandyType::TYPE_BLUE: charCandy = 'B'; break;
                case CandyType::TYPE_YELLOW: charCandy = 'Y'; break;
                case CandyType::TYPE_PURPLE: charCandy = 'P'; break;
                case CandyType::TYPE_ORANGE: charCandy = 'O'; break;
                default: charCandy = '-';
                }
                file << i << ' ' << j << ' ' << charCandy << std::endl;
                j--;
            } 
        }
        
    }
    else
        correcte = false;

    file.close();

    return correcte;
    
    //return false;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    
    int x, y;
    char charCandy;
    bool correcte = true;

    m_storage.clear();

    for (int i = 0; i < m_height; i++)
        for (int j = 0; j < m_width; j++)
            m_board[i][j] = nullptr;

    std::ifstream file(input_path);
    if (file.is_open())
        while (file >> x >> y >> charCandy)
        {
            CandyType type;

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
            if (type != CandyType::COUNT)
            {
                Candy candy(type);
                m_storage[y][x] = candy;
                m_board[y][x] = &m_storage[y][x];
            }
        }
    else
        correcte = false;
    
    file.close();
    
    return correcte;
    
    //return false;
}
