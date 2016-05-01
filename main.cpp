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
#include <string>
//#include time.h


// Init allegro with 800x600 (resolution)
Graphic g(800,600,32);

int main()
{
    // Do not touch !
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    // Load level from a file

    std::string a;
    std::string chemin = "./levels/";
    std::cout<< " choisissez le niveau de difficulté, 0 pour test, 1 pour easy, 2 pour medium et 3 pour Hard" << std::endl;
    std::cin >> a;
    while(a.size()!= 1 && (a[0]!='0' || a[0]!='1' || a[0]!='2' || a[0]!='3'))
    {
        std::cout<< " choisissez le niveau de difficulté, 0 pour test, 1 pour easy, 2 pour medium et 3 pour Hard" << std::endl;
        std::cin >> a;
    }
    if(a[0]=='0') a = "test/test_"; else if(a[0]=='1') a = "easy/easy_"; else if(a[0]=='2') a = "medium/medium_"; else if(a[0]=='3') a = "hard/hard_";
    chemin = chemin + a;
    std::cout<< " choisissez la partie correspondante, ATTENTION, prendre un terrain qui existe" << std::endl;
    std::cin >> a;
    chemin = chemin + a + ".dat";

    //Maze m("./levels/easy/easy_3.dat");
    Maze m(chemin);
    if (!m.init()) return -1;
    std::cout << m << std::endl;
    std::cout<< " pour avoir les case morte tapez: 'm' ou 'M', autre chose sinon" << std::endl;
    bool deadBloc = false;
    std::cin >> a;
    if(a.size()== 1 && (a[0]=='m' || a[0]=='M')) deadBloc = true;


    std::cout << std::endl;
    std::cout << " Tapez U, pour le Brut Force  " << std::endl;
    std::cout << " Tapez B, pour le BFS perso " << std::endl;
    std::cout << " Tapez D, pour le DFS perso " << std::endl;
    std::cout << " Tapez F, pour le BFS cas  " << std::endl;
    std::cout << " Tapez H, pour l'heuristique ( pas terminé )  " << std::endl;









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
                    //win = m.bfsCaseMoveOnly(deadBloc);
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
                case KEY_F:
                    //win = m.bfs(deadBloc);
                    win = m.bfsCaseMoveOnly(deadBloc);
                    std::cout << "temps de bfs : "<< (clock()-tempDeRecherche)/1000 << std::endl;
                    if (win){m.animTabDir(g);}
                    break;
                case KEY_H:
                    win = m.heuristique(deadBloc);
                    std::cout << "temps de l'heurestique : "<< (clock()-tempDeRecherche)/1000 << std::endl;
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
