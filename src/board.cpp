#include "board.h"
#include <memory>
#include <iostream>


Board::Board(int width, int height) : m_width(DEFAULT_BOARD_WIDTH), m_height(DEFAULT_BOARD_HEIGHT),
m_candyRed(CandyType::TYPE_RED), m_candyGreen(CandyType::TYPE_GREEN), m_candyBlue(CandyType::TYPE_BLUE), 
m_candyYellow(CandyType::TYPE_YELLOW), m_candyPurple(CandyType::TYPE_PURPLE), m_candyOrange(CandyType::TYPE_ORANGE)
{
    // Implement your code here
    if (width >= 0)
        m_width = width;

    if (height >= 0)
        m_height = height;

    m_board.resize(m_height, std::vector<Candy*>(m_width, nullptr));
}

Board::~Board()
{
    // Implement your code here
    m_board.clear();
}

Candy* Board::getCell(int x, int y) const
{
    // Implement your code here
    Candy* cell = nullptr;
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
        cell = m_board[y][x];
    return cell;
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
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
    
    if (x >= 0 && x < m_width && y >= 0 && y < m_height && m_board[y][x] != nullptr)
    {
        CandyType type = m_board[y][x]->getType();
        int count = 1;
        int steps = 1;
        // N/S
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
        explode = count >= SHORTEST_EXPLOSION_LINE;
        // E/W
        if (!explode)
        {

            count = 1;
            steps = 1;
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
            explode = count >= SHORTEST_EXPLOSION_LINE;
        }
        // NE/SW
        if (!explode)
        {
            count = 1;
            steps = 1;
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
            explode = count >= SHORTEST_EXPLOSION_LINE;
        }
        // NW/SE
        if(!explode)
        {
            count = 1;
            steps = 1;
            while (y + steps < m_height && x - steps >= 0 && m_board[y + steps][x - steps] != nullptr && m_board[y + steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
            {
                count++;
                steps++;
            }
            steps = 1;
            while (y - steps >= 0 && x + steps < m_width && m_board[y - steps][x + steps]  != nullptr && m_board[y - steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
            {
                count++;
                steps++;
            }
            explode = count >= SHORTEST_EXPLOSION_LINE;
        }
    }
    return explode;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    std::vector<Candy*> exploded;
    std::vector<std::vector<bool>> toExplode (m_height, std::vector<bool>(m_width, false));
    bool explosion = false;
    do
    {
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
                toExplode[y][x] = shouldExplode(x, y);
        for (int y = 0; y < m_height; y++)
            for (int x = 0; x < m_width; x++)
                if (toExplode[y][x])
                {
                    explosion = true;
                    exploded.push_back(m_board[y][x]);
                    m_board[y][x] = nullptr;
                }
        for (int x = 0; x < m_width; x++)
        {
            int y = m_height - 1;
            int shift = 0;
            while (y-shift >= 0)
            {
                while (y - shift >= 0 && m_board[y][x] == nullptr)
                    shift++;
                if (y - shift >= 0)
                    m_board[y][x] = m_board[y - shift][x];
                y--;
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

    char charCandy;
    
    file >> m_width >> m_height;
    m_board.resize(m_height, std::vector<Candy*>(m_width));
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            file >> charCandy;
            switch (charCandy)
            {
            case 'R': setCell(&m_candyRed, x, y); break;
            case 'G': setCell(&m_candyGreen, x, y); break;
            case 'B': setCell(&m_candyBlue, x, y); break;
            case 'Y': setCell(&m_candyYellow, x, y); break;
            case 'P': setCell(&m_candyPurple, x, y); break;
            case 'O': setCell(&m_candyOrange, x, y); break;
            default: setCell(nullptr, x, y); break;
            }
        }
    }

    file.close();
    
    return true;
}