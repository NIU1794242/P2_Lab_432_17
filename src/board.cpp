#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
{
    // Implement your code here
    m_height = height;
    m_width = width;
    m_storage.reserve(m_width * m_height);
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
    // Implement your code here
    return false;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    return {};
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    return false;
}

bool Board::load(const std::string& input_path)
{
    int x, y;
    char charCandy;

    m_storage.clear();

    for (int i = 0; i < m_height; i++)
        for (int j = 0; j < m_width; j++)
            m_board[i][j] = nullptr;

    std::ifstream file(input_path);
    if (!file.is_open())
        return false;

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
        default:
            continue; // carácter inválido
        }
        Candy candy(type);
        m_storage.emplace_back(candy);
        m_board[y][x] = &m_storage.back();
    }

    return true;
}
