#include "board.h"
#include <memory>
#include <iostream>

Candy candyRed(CandyType::TYPE_RED);
Candy candyGreen(CandyType::TYPE_GREEN);
Candy candyBlue(CandyType::TYPE_BLUE);
Candy candyYellow(CandyType::TYPE_YELLOW);
Candy candyPurple(CandyType::TYPE_PURPLE);
Candy candyOrange(CandyType::TYPE_ORANGE);

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
        count = 1;
        steps = 1;
        while (!explode && y + steps < m_height && x - steps >= 0 && m_board[y + steps][x - steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
        {
            count++;
            steps++;
        }
        steps = 1;
        while (!explode && y - steps >= 0 && x + steps < m_width && m_board[y - steps][x + steps]->getType() == type && count < SHORTEST_EXPLOSION_LINE)
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
        for (int x = 0; x < m_width; x++)
        {
            int y = m_height-1;
            while (y >= 0 && m_board[y][x] != nullptr)
            {
                CandyType type = m_board[y][x]->getType();
                switch (type)
                {
                case CandyType::TYPE_RED: charCandy = 'R'; break;
                case CandyType::TYPE_GREEN: charCandy = 'G'; break;
                case CandyType::TYPE_BLUE: charCandy = 'B'; break;
                case CandyType::TYPE_YELLOW: charCandy = 'Y'; break;
                case CandyType::TYPE_PURPLE: charCandy = 'P'; break;
                case CandyType::TYPE_ORANGE: charCandy = 'O'; break;
                default: charCandy = '-';
                }
                file << x << ' ' << y << ' ' << m_board[y][x] << std::endl;
                y--;
            } 
        }
    }
    else
        correcte = false;

    file.close();

    return correcte;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here
    int x, y;
    char charCandy;
    bool correcte = true;

    for (int y = 0; y < m_height; y++)
        for (int x = 0; x < m_width; x++)
            m_board[y][x] = nullptr;
    
    std::ifstream file;
    file.open(input_path);
    if (file.is_open())
        while (file >> x >> y >> charCandy)
            switch (charCandy)
            {
            case 'R': m_board[y][x] = &candyRed; break;
            case 'G': m_board[y][x] = &candyGreen; break;
            case 'B': m_board[y][x] = &candyBlue; break;
            case 'Y': m_board[y][x] = &candyYellow; break;
            case 'P': m_board[y][x] = &candyPurple; break;
            case 'O': m_board[y][x] = &candyOrange; break;
            }
    else
        correcte = false;
    
    file.close();
    
    return correcte;
}

std::ifstream& operator>>(std::ifstream& input, Board& board)
{
    int x, y;
    char charCandy;
    input >> x >> y >> charCandy;
    switch (charCandy)
    {
    case 'R': board.setCell(&candyRed, x, y); break;
    case 'G': board.setCell(&candyGreen, x, y); break;
    case 'B': board.setCell(&candyBlue, x, y); break;
    case 'Y': board.setCell(&candyYellow, x, y); break;
    case 'P': board.setCell(&candyPurple, x, y); break;
    case 'O': board.setCell(&candyOrange, x, y); break;
    }
    
}