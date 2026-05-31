/*
#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

bool test()
{
    // Test board 2D container
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    b.setCell(&c, 0, 0);
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }
    // Dump and load board
    {
        Board b2(10, 10);
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    // Dump and load game
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (g != g2)
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }
    
    return true;
}
*/
#include <cstdio> 
#include <iostream> 
#include <string> 
#include <vector>

#include "test.h"
#include "board.h"
#include "candy.h"
#include "util.h"

bool test()
{
    bool ok = true;
    
    // 1. Constructor, ancho, alto y tablero vacío
    { 
        Board b(4, 3);

        if (b.getWidth() != 4) 
        {
            std::cout << "Error en getWidth()" << std::endl;
            ok = false;
        }

        if (b.getHeight() != 3) 
        {
            std::cout << "Error en getHeight()" << std::endl;
            ok = false;
        }

        for (int x = 0; x < b.getWidth(); x++)
        {
            for (int y = 0; y < b.getHeight(); y++)
            {
                if (b.getCell(x, y) != nullptr)
                {
                    std::cout << "El tablero no empieza vacío" << std::endl;
                    ok = false;
                }
            }
        }
    }
    
    // 2. getCell y setCell en caso válido
    {
        Board b(3, 3);
        Candy* red = new Candy(CandyType::TYPE_RED);
        Candy* blue = new Candy(CandyType::TYPE_BLUE);

        b.setCell(red, 0, 0);
        b.setCell(blue, 2, 1);

        if (b.getCell(0, 0) != red)
        {
            std::cout << "Error en setCell/getCell en (0,0)" << std::endl;
            ok = false;
        }

        if (b.getCell(2, 1) != blue)
        {
            std::cout << "Error en setCell/getCell en (2,1)" << std::endl;
            ok = false;
        }

        b.setCell(nullptr, 0, 0);

        if (b.getCell(0, 0) != nullptr)
        {
            std::cout << "Error al vaciar una celda con nullptr" << std::endl;
            ok = false;
        }
    }
    
    // 3. Casos inválidos de getCell y setCell
    {
        Board b(2, 2);
        Candy* orange = new Candy(CandyType::TYPE_ORANGE);

        b.setCell(orange, 0, 0);

        if (b.getCell(-1, 0) != nullptr)
        {
            std::cout << "getCell(-1,0) debería devolver nullptr" << std::endl;
            ok = false;
        }

        if (b.getCell(0, -1) != nullptr)
        {
            std::cout << "getCell(0,-1) debería devolver nullptr" << std::endl;
            ok = false;
        }

        if (b.getCell(2, 0) != nullptr)
        {
            std::cout << "getCell(2,0) debería devolver nullptr" << std::endl;
            ok = false;
        }

        if (b.getCell(0, 2) != nullptr)
        {
            std::cout << "getCell(0,2) debería devolver nullptr" << std::endl;
            ok = false;
        }

        b.setCell(orange, -1, 0);
        b.setCell(orange, 0, -1);
        b.setCell(orange, 2, 0);
        b.setCell(orange, 0, 2);

        if (b.getCell(0, 0) != orange)
        {
            std::cout << "setCell fuera de rango ha cambiado una celda válida" << std::endl;
            ok = false;
        }

    }

    // 4. shouldExplode: casos inválidos y sin falso positivo
    {
        Board b(4, 4);
        Candy* g1 = new Candy(CandyType::TYPE_GREEN);
        Candy* g2 = new Candy(CandyType::TYPE_GREEN);

        b.setCell(g1, 0, 0);
        b.setCell(g2, 1, 0);

        if (b.shouldExplode(-1, 0))
        {
            std::cout << "shouldExplode fuera de rango debería dar false" << std::endl;
            ok = false;
        }

        if (b.shouldExplode(3, 3))
        {
            std::cout << "shouldExplode en celda vacía debería dar false" << std::endl;
            ok = false;
        }

        if (b.shouldExplode(0, 0) || b.shouldExplode(1, 0))
        {
            std::cout << "Dos caramelos iguales no deberían explotar" << std::endl;
            ok = false;
        }
    }

    // 5. shouldExplode horizontal
    {
        Board b(4, 4);
        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);
        Candy* r3 = new Candy(CandyType::TYPE_RED);

        b.setCell(r1, 0, 1);
        b.setCell(r2, 1, 1);
        b.setCell(r3, 2, 1);

        if (!b.shouldExplode(0, 1) || !b.shouldExplode(1, 1) || !b.shouldExplode(2, 1))
        {
            std::cout << "Error detectando explosión horizontal" << std::endl;
            ok = false;
        }
    }

    // 6. shouldExplode vertical
    {
        Board b(4, 4);
        Candy* y1 = new Candy(CandyType::TYPE_YELLOW);
        Candy* y2 = new Candy(CandyType::TYPE_YELLOW);
        Candy* y3 = new Candy(CandyType::TYPE_YELLOW);

        b.setCell(y1, 2, 0);
        b.setCell(y2, 2, 1);
        b.setCell(y3, 2, 2);

        if (!b.shouldExplode(2, 0) || !b.shouldExplode(2, 1) || !b.shouldExplode(2, 2))
        {
            std::cout << "Error detectando explosión vertical" << std::endl;
            ok = false;
        }
    }

    // 7. shouldExplode diagonal principal
    {
        Board b(4, 4);
        Candy* p1 = new Candy(CandyType::TYPE_PURPLE);
        Candy* p2 = new Candy(CandyType::TYPE_PURPLE);
        Candy* p3 = new Candy(CandyType::TYPE_PURPLE);

        b.setCell(p1, 0, 0);
        b.setCell(p2, 1, 1);
        b.setCell(p3, 2, 2);

        if (!b.shouldExplode(0, 0) || !b.shouldExplode(1, 1) || !b.shouldExplode(2, 2))
        {
            std::cout << "Error detectando diagonal principal" << std::endl;
            ok = false;
        }
    }

    // 8. shouldExplode diagonal secundaria
    {
        Board b(4, 4);
        Candy* o1 = new Candy(CandyType::TYPE_ORANGE);
        Candy* o2 = new Candy(CandyType::TYPE_ORANGE);
        Candy* o3 = new Candy(CandyType::TYPE_ORANGE);

        b.setCell(o1, 2, 0);
        b.setCell(o2, 1, 1);
        b.setCell(o3, 0, 2);

        if (!b.shouldExplode(2, 0) || !b.shouldExplode(1, 1) || !b.shouldExplode(0, 2))
        {
            std::cout << "Error detectando diagonal secundaria" << std::endl;
            ok = false;
        }
    }

    // 9. explodeAndDrop cuando no hay explosión
    {
        Board b(3, 3);
        Candy* red = new Candy(CandyType::TYPE_RED);
        Candy* blue = new Candy(CandyType::TYPE_BLUE);

        b.setCell(red, 0, 0);
        b.setCell(blue, 2, 2);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 0)
        {
            std::cout << "explodeAndDrop no debería explotar nada aquí" << std::endl;
            ok = false;
        }

        if (b.getCell(0, 0) != red || b.getCell(2, 2) != blue)
        {
            std::cout << "explodeAndDrop ha cambiado el tablero sin motivo" << std::endl;
            ok = false;
        }
    }

    // 10. explodeAndDrop simple con caída
    {
        Board b(3, 4);
        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);
        Candy* r3 = new Candy(CandyType::TYPE_RED);
        Candy* blue = new Candy(CandyType::TYPE_BLUE);

        b.setCell(r1, 0, 3);
        b.setCell(r2, 1, 3);
        b.setCell(r3, 2, 3);
        b.setCell(blue, 1, 1);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 3)
        {
            std::cout << "explodeAndDrop debería explotar 3 caramelos" << std::endl;
            ok = false;
        }

        if (b.getCell(1, 3) != blue)
        {
            std::cout << "El caramelo azul no ha caído bien" << std::endl;
            ok = false;
        }
    }

    // 11. explodeAndDrop con reacción en cadena
    {
        Board b(3, 4);

        Candy* r1 = new Candy(CandyType::TYPE_RED);
        Candy* r2 = new Candy(CandyType::TYPE_RED);
        Candy* r3 = new Candy(CandyType::TYPE_RED);
                 
        Candy* g1 = new Candy(CandyType::TYPE_GREEN);
        Candy* g2 = new Candy(CandyType::TYPE_GREEN);
        Candy* g3 = new Candy(CandyType::TYPE_GREEN);

        // Primera explosión: columna roja en x = 1
        b.setCell(g1, 0, 3);
        b.setCell(g2, 1, 0);
        b.setCell(g3, 2, 3);

        b.setCell(r1, 1, 1);
        b.setCell(r2, 1, 2);
        b.setCell(r3, 1, 3);

        std::vector<Candy*> exploded = b.explodeAndDrop();

        if (exploded.size() != 6)
        {
            std::cout << "La reacción en cadena debería explotar 6 caramelos" << std::endl;
            ok = false;
        }

        for (int x = 0; x < b.getWidth(); x++)
        {
            for (int y = 0; y < b.getHeight(); y++)
            {
                if (b.getCell(x, y) != nullptr)
                {
                    std::cout << "Después de la reacción en cadena el tablero debería quedar vacío" << std::endl;
                    ok = false;
                }
            }
        }
    }

    // 12. dump y load correctos
    {
        Board b1(4, 4);
        Candy* c1 = new Candy(CandyType::TYPE_RED);
        Candy* c2 = new Candy(CandyType::TYPE_BLUE);
        Candy* c3 = new Candy(CandyType::TYPE_GREEN);

        b1.setCell(c1, 0, 0);
        b1.setCell(c2, 2, 1);
        b1.setCell(c3, 3, 3);

        std::string fileName = getDataDirPath() + "board_test.txt";

        if (!b1.dump(fileName))
        {
            std::cout << "Error en dump()" << std::endl;
            ok = false;
        }

        Board b2(4, 4);

        if (!b2.load(fileName))
        {
            std::cout << "Error en load() con fichero válido" << std::endl;
            ok = false;
        }

        Candy* a = b2.getCell(0, 0);
        Candy* b = b2.getCell(2, 1);
        Candy* c = b2.getCell(3, 3);

        if (a == nullptr || a->getType() != CandyType::TYPE_RED)
        {
            std::cout << "Error al cargar la celda (0,0)" << std::endl;
            ok = false;
        }

        if (b == nullptr || b->getType() != CandyType::TYPE_BLUE)
        {
            std::cout << "Error al cargar la celda (2,1)" << std::endl;
            ok = false;
        }

        if (c == nullptr || c->getType() != CandyType::TYPE_GREEN)
        {
            std::cout << "Error al cargar la celda (3,3)" << std::endl;
            ok = false;
        }

        if (b2.getCell(1, 1) != nullptr)
        {
            std::cout << "Error: una celda vacía se ha cargado mal" << std::endl;
            ok = false;
        }

        std::remove(fileName.c_str());
    }

    // 13. load con fichero inexistente
    {
        Board b(3, 3);
        std::string fileName = getDataDirPath() + "fichero_que_no_existe.txt";

        if (b.load(fileName))
        {
            std::cout << "load debería devolver false si el fichero no existe" << std::endl;
            ok = false;
        }
    }
    
    return ok;
}