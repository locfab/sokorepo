/************************************************************
Sokoban project - Main file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/
#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include <iostream>
//#include time.h


// Init allegro with 800x600 (resolution)
Graphic g(800,600,32);

int main()
{
    // Do not touch !
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    // Load level from a file
    Maze m("./levels/test/test_0.dat");
    if (!m.init()) return -1;
    std::cout << m << std::endl;
    bool deadBloc = true;

    std::cout << "si vous voulez prendre en compte les cases morte tapez: m, ou ailleurs sinon"  << std::endl;


    if(deadBloc) m.detecteDeadBloc();

    if (g.init())
    {
        // While playing...
        while (!g.keyGet(KEY_ESC) && !g.keyGet(KEY_A))
        {
            // Check if user has pressed a key
            if (g.keyPressed())
            {
                bool win = false;
                long double tempDeRecherche = clock();
                switch (g.keyRead())
                {
                case ARROW_UP:
                    win = m.updatePlayer(TOP);
                    break;
                case ARROW_BOTTOM:
                    win = m.updatePlayer(BOTTOM);
                    break;
                case ARROW_RIGHT:
                    win = m.updatePlayer(RIGHT);
                    break;
                case ARROW_LEFT:
                    win = m.updatePlayer(LEFT);
                    break;
                case KEY_B:
                    win = m.bfs(deadBloc);
                    std::cout << "temps de bfs : "<< (clock()-tempDeRecherche)/1000 << std::endl;
                    if (win){m.animTabDir(g);}

                    break;
                case KEY_D:
                    win = m.dfs(deadBloc);
                    std::cout << "temps de dfs : " << (clock()-tempDeRecherche)/1000 << std::endl;
                    if (win){m.animTabDir(g);}

                    break;
                case KEY_U:
                    win = m.brutForce(deadBloc);
                    std::cout << "temps de brute force" << (clock()-tempDeRecherche)/1000 << std::endl;
                    if (win){m.animTabDir(g);}
                    break;
                }
            }
            // Display on screen
            g.clear();
            m.draw(g);
            g.display(Coord::m_nb_col);
        }
    }
    // Free memory
    g.deinit();
    Console::deleteInstance();

    return 0;
}
END_OF_MAIN()
