#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
{
    // Implement your code here
    m_height = height;
    m_width = width;
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
    // Implement your code here
    int x, y;
    bool correcte = true;
    char charCandy;
    m_storage = {};
    for (int i = 0; i < m_height; i++)
        for (int j = 0; j < m_width; j++)
            m_board[i][j] = nullptr;
    std::ifstream file;
    file.open(input_path);
    correcte = file.is_open();
    while(correcte && !file.eof())
    {
        file >> x;
        file >> y;
        file >> charCandy;
        correcte = !file.fail();
        if (correcte)
        {
            switch (charCandy)
            {
            case 'R':
                Candy candy(CandyType::TYPE_RED);
                m_storage.push_back(candy);
                break;
            case 'G':
                Candy candy(CandyType::TYPE_GREEN);
                m_storage.push_back(candy);
                break;
            case 'B':
                Candy candy(CandyType::TYPE_BLUE);
                m_storage.push_back(candy);
                break;
            case 'Y':
                Candy candy(CandyType::TYPE_YELLOW);
                m_storage.push_back(candy);
                break;
            case 'P':
                Candy candy(CandyType::TYPE_PURPLE);
                m_storage.push_back(candy);
                break;
            case 'O':
                Candy candy(CandyType::TYPE_ORANGE);
                m_storage.push_back(candy);
                break;
            }
            m_board[y][x] = &m_storage.back();
        }
    }
    return correcte;
}
