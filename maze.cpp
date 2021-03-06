/************************************************************
Sokoban project - Maze file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#include "maze.h"
#include "graphic.h"
#include "utils/console.h"
#include "utils/coord.h"
#include <fstream>
#include <iomanip>
#include <algorithm>    // std::reverse



Maze::Maze(const std::string& path)
    : m_lig(0), m_col(0), m_pos_player(0), m_dir_player(TOP), m_level_path(path)
{
}

Maze::~Maze()
{
}

bool Maze::init()
{
    bool res = this->_load(this->m_level_path);
    if (!res)
    {
        std::cerr << "Cannot load maze... Check file : " << this->m_level_path << std::endl;
        return false;
    }

    return res;
}

// Check if all boxes are on a goal
bool Maze::_isCompleted() const
{
    for (unsigned int i=0; i<this->m_pos_boxes.size(); ++i)
    {
        if (!this->isSquareBoxPlaced(this->m_pos_boxes[i]))
            return false;
    }
    return true;
}

// Check if we can push a box in a direction
// INPUT: position of the box to check, direction,
// OUTPUT : the position of the box after pushing
//      TRUE if all goes right otherwise FALSE
bool Maze::_canPushBox(unsigned short posBox, char dir, unsigned short& newPosBox) const
{
    // Check if this position is a box !
    if (!this->isSquareBox(posBox))
        return false;

    // Compute new position according to push direction
    newPosBox = Coord::getDirPos(posBox, dir);

    // Can we push the box ?
    return this->isSquareWalkable(newPosBox);
}

// Load a maze from a file (DO NOT TOUCH)
bool Maze::_load(const std::string& path)
{
    std::vector<unsigned short> tmpPosBoxes;
    std::ifstream ifs(path.c_str());
    if (ifs)
    {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(ifs, line))
        {
            lines.push_back(line);
            this->m_lig++;
            this->m_col = (this->m_col < line.size() ? line.size() : this->m_col);
        }
        ifs.close();

        if (this->m_col > NB_MAX_WIDTH || this->m_lig > NB_MAX_HEIGHT)
        {
            std::cerr << "Maze::load => Bad formatting in level data..." << std::endl;
            return false;
        }

        Coord::m_nb_col = this->m_col;
        for (unsigned int i=0; i<lines.size(); i++)
        {
            //LDebug << "Maze::load => Reading : " << lines[i];
            for (unsigned int j=0; j<this->m_col; j++)
            {
                if (j < lines[i].size())
                {
                    bool both = false;
                    unsigned short pos = Coord::coord1D(i, j);
                    unsigned char s = (unsigned char)(lines[i][j] - '0');

                    // Need to add a goal and a box ;)
                    if (s == SPRITE_BOX_PLACED)
                    {
                        both = true;
                    }

                    if (s == SPRITE_GOAL || both)
                    {
                        this->m_pos_goals.push_back(pos);
                    }
                    if (s == SPRITE_BOX || both)
                    {
                        tmpPosBoxes.push_back(pos);
                    }

                    // Assign player position
                    if (s == SPRITE_MARIO)
                    {
                        this->m_pos_player = pos;
                        //LDebug << "\tAdding player pos (" << pos << ")";
                        s = SPRITE_GROUND;
                    }

                    // Add this value in the field
                    this->m_field.push_back(s);
                }
                else
                {
                    // Here - Out of bound
                    this->m_field.push_back(SPRITE_GROUND);
                }
            }
        }

        // Copy box position
        this->m_pos_boxes.resize(tmpPosBoxes.size());
        for (unsigned int i=0; i<tmpPosBoxes.size(); ++i)
        {
            this->m_pos_boxes[i] = tmpPosBoxes[i];
        }

        return (this->m_pos_boxes.size() == this->m_pos_goals.size());
    }
    else
    {
        std::cerr << "Maze::load => File does not exist..." << std::endl;
    }

    return false;
}

bool Maze::updatePlayer(char dir)
{
    m_dir_player = dir;
    if (dir < 0 || dir > MAX_DIR)
    {
        std::cerr << "Maze::updatePlayer => Direction not correct... " << +dir << std::endl;
        return false;
    }

    int posTabBox=-1;
    unsigned short newPos=Coord::getDirPos(m_pos_player,dir);

    if(isSquareWalkable(newPos))
    {
        m_pos_player = Coord::getDirPos(m_pos_player,dir);
        return false;
    }
    if(isSquareBox(newPos))
    {
        for (int i=0; i<m_pos_boxes.size(); i++)
        {
            if (m_pos_boxes[i] == newPos )
            {
                posTabBox=i;
            }
        }
        unsigned short oldPos=newPos;
        newPos=Coord::getDirPos(oldPos,dir);

        if(_canPushBox(oldPos,dir,newPos))
        {
            if (m_field[newPos] == SPRITE_GOAL )
            {
                m_field[newPos] = SPRITE_BOX_PLACED;
                if (m_field[oldPos] == SPRITE_BOX_PLACED )
                    m_field[oldPos] = SPRITE_GOAL;
                else if (m_field[oldPos] == SPRITE_BOX )
                    m_field[oldPos] = SPRITE_GROUND;
                else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                    m_field[oldPos] = SPRITE_DEADSQUARE;

            }
            else if (m_field[newPos] == SPRITE_GROUND )
            {
                m_field[newPos] = SPRITE_BOX;
                if (m_field[oldPos] == SPRITE_BOX_PLACED )
                    m_field[oldPos] = SPRITE_GOAL;
                else if (m_field[oldPos] == SPRITE_BOX )
                    m_field[oldPos] = SPRITE_GROUND;
                else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                    m_field[oldPos] = SPRITE_DEADSQUARE;
            }
            else if (m_field[newPos] == SPRITE_DEADSQUARE )
            {
                m_field[newPos] = SPRITE_BOX_DEADSQUARE;
                if (m_field[oldPos] == SPRITE_BOX_PLACED )
                    m_field[oldPos] = SPRITE_GOAL;
                else if (m_field[oldPos] == SPRITE_BOX )
                    m_field[oldPos] = SPRITE_GROUND;
                else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                    m_field[oldPos] = SPRITE_DEADSQUARE;
            }

            m_pos_boxes[posTabBox] = newPos;
            //m_field[newPos] = SPRITE_BOX;
            m_pos_player = Coord::getDirPos(m_pos_player,dir);
        }
    }
    if(_isCompleted())
        return true;
    else
    {
        return false;
    }
}

// Display maze on screen with Allegro
void Maze::draw(const Graphic& g) const
{
    for(unsigned int i=0; i<this->getSize(); i++)
    {
        unsigned int l = 0, c = 0;
        Coord::coord2D(i, l, c);

        if (i == this->m_pos_player)
        {
            g.drawT(g.getSpritePlayer(this->m_dir_player), c, l);
        }
        else
        {
            g.drawT(g.getSprite(this->m_field[i]), c, l);
        }
    }
}

// DO NOT TOUCH !
// Overload function for displaying debug information
// about Maze class
std::ostream& operator << (std::ostream& O, const Maze& m)
{
    unsigned int l, c;
    int i = 0;
    Coord::coord2D(m.m_pos_player, l, c);
    O << "Player position " << m.m_pos_player << " (" << l << "," << c << ")" << std::endl;
    O << "Field Size " << +m.m_lig << " x " << +m.m_col << " = " << m.getSize() << std::endl;
    O << "Field Vector capacity : " << m.m_field.capacity() << std::endl;
    O << "Field array : " << std::endl << std::endl;
    for(unsigned int l=0; l<m.getSize(); l++)
    {
        if (l == m.m_pos_player) Console::getInstance()->setColor(_COLOR_YELLOW);
        else if (m.isSquareWall(l)) Console::getInstance()->setColor(_COLOR_PURPLE);
        else if (m.isSquareBoxPlaced(l) || m.isSquareGoal(l)) Console::getInstance()->setColor(_COLOR_GREEN);
        else if (m.isSquareBox(l)) Console::getInstance()->setColor(_COLOR_BLUE);
        else if (m.m_field[l] == SPRITE_DEADSQUARE) Console::getInstance()->setColor(_COLOR_RED);
        else Console::getInstance()->setColor(_COLOR_WHITE);

        O << std::setw(2) << +m.m_field[l] << " "; // + => print as "int"

        if ((l+1) % m.m_col == 0)
        {
            O << std::endl;
        }
    }
    Console::getInstance()->setColor(_COLOR_DEFAULT);

    O << std::endl;
    O << "Box position : " << std::endl;
    for (unsigned int i=0; i<m.m_pos_boxes.size(); i++)
    {
        Coord::coord2D(m.m_pos_boxes[i], l, c);
        O << "\t" << "Box #" << i << " => " << std::setw(3) << m.m_pos_boxes[i] << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
    }

    O << std::endl;
    O << "Goal position : " << std::endl;
    for (const auto& goal : m.m_pos_goals)
    {
        unsigned int l, c;
        Coord::coord2D(goal, l, c);
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_GREEN);
        O << "\t" << "Goal #" << i << " => " << std::setw(3) << goal << std::setw(2) << " (" << l << "," << c << ")" << std::endl;
        if (m.isSquareBoxPlaced(goal)) Console::getInstance()->setColor(_COLOR_DEFAULT);
        i++;
    }

    return O;
}

bool Maze::brutForce(bool deadBloc)
{
    bool win = false;
    m_tabDir.push_back(0);
    pos_boxesSauv = m_pos_boxes;
    pos_playerSauv = m_pos_player;
    fieldSauv = m_field;
    int j=1;

    while(!win)
    {

        for(int i=0; i< m_tabDir.size(); i++) //faire tous les d�placement qui sont indiqu�s dans le tableau de direction
        {
            win=updatePlayer(m_tabDir[i]); //booleen qui va nous dire si on a gagn� ou pas
        }

        regenererPos(pos_playerSauv,fieldSauv,pos_boxesSauv);//on remet l'objet aux valeurs initiale

        if(!win) /// tout ce if(!win) va permettre d'incrementer la suite de direction
        {
            m_tabDir[m_tabDir.size()-1]++;//on change la derniere direction
            for(int i=m_tabDir.size()-1; i>0; i--)
            {
                if(m_tabDir[i]==4)//si on a fait les quatres direction alors on change d'avant derniere position et on reste pour la derniere direction
                {
                    m_tabDir[i]=0;
                    m_tabDir[i-1]++;
                }
            }
            if(m_tabDir.size()==1)//on incremente la case s'il n'y a qu'une direction (car s'il n'y a qu'une case elle ne rentre pas dans le for du dessus
            {
                m_tabDir[0]++;
            }
            if(m_tabDir[0]==4)//on met la premiere case � 0 (les autres le sont dej�) et on ajoute une case � 0
            {
                m_tabDir[0]=0;
                m_tabDir.push_back(0);
            }

            //affichage de la suite
            /* for(int i=0; i<m_tabDir.size(); i++)
             {

                 std::cout << m_tabDir[i] << " ";
             }
             j++;
             std::cout << " " << j <<"\n";
             //std::cout << std::endl;*/
        }
    }
    afficherInfoTabDir();

    return true;
}

bool Maze::bfs(bool deadBloc)
{
    bool win = false;
    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));
    std::queue <SauvInfo> maFile;
    maFile.push(sauvegarde[0]);
    int indicePred = 0;

    while(!win)
    {
        SauvInfo temp = maFile.front(); //prend la case qui a etait mis a la file en premier
        maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer
        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());//on remet le plateau � l'etat de temp

        for(int i = 0; i<4; i++)
        {
            if(!win)
            {
                win=updatePlayer(i);
                if(!win)
                {
                    if(!caseOnDeadBloc() && !doublon(sauvegarde))//on verifie qu'il n'y a pas de doublon
                    {
                        sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,indicePred,i));
                        maFile.push(sauvegarde[sauvegarde.size()-1]);
                        //if(sauvegarde.size()%101 == 100) std::cout << sauvegarde.size() << std::endl;
                    }
                    regenererPos(temp.getPosPlayer(),temp.getField(),temp.getPosBoxes());
                }
                else if(win)
                {
                    sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,indicePred,i));
                    maFile.push(sauvegarde[sauvegarde.size()-1]);
                    regenererPos(sauvegarde[0].getPosPlayer(),sauvegarde[0].getField(),sauvegarde[0].getPosBoxes());
                }
            }
        }
        indicePred++;
    }

    std::stack <unsigned short> tabDirEnvers;
    int j = sauvegarde.size()-1;
    while(sauvegarde[j].getPred()!=-1)
    {
        tabDirEnvers.push(sauvegarde[j].getCoupPred());
        j = sauvegarde[j].getPred();
    }
    while(!tabDirEnvers.empty())
    {
        m_tabDir.push_back(tabDirEnvers.top());
        tabDirEnvers.pop();
    }

    afficherInfoTabDir();
    std::cout << " nombre de sauvegarde " << sauvegarde.size() << "nombre de sauvgarde" << std::endl;

    return true;
}

bool Maze::dfs(bool deadBloc)
{
    bool win = false;
    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));
    std::stack <SauvInfo> maPile;
    maPile.push(sauvegarde[0]);
    bool nouveauSave=false;
    int taillePile=0;


    while(!win)
    {
        if(!pileModifiee(taillePile, maPile.size()))
        {
            maPile.pop();
        }
        SauvInfo temp = maPile.top();
        taillePile = maPile.size();
        regenererPos(temp.getPosPlayer(),temp.getField(),temp.getPosBoxes());

        for(int i = 0; i<4; i++)
        {
            if(!pileModifiee(taillePile, maPile.size()))
            {
                win=updatePlayer(i);
                if(!win)
                {
                    if(!caseOnDeadBloc() && !doublon(sauvegarde))//savoir si la situation s'est d�ja produite
                    {
                        sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,sauvegarde.size()-1,i));
                        maPile.push(sauvegarde[sauvegarde.size()-1]);
                        //if(maPile.size()%11 == 10)
                        //std::cout << maPile.size() << std::endl;
                        //std::cout << sauvegarde.size() << std::endl;

                    }
                    regenererPos(temp.getPosPlayer(),temp.getField(),temp.getPosBoxes());
                }
                else if(win)
                {
                    sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,sauvegarde.size()-1,i));
                    maPile.push(sauvegarde[sauvegarde.size()-1]);
                    regenererPos(sauvegarde[0].getPosPlayer(),sauvegarde[0].getField(),sauvegarde[0].getPosBoxes());
                }
            }
        }
        nouveauSave = false;
    }

    std::cout << "WIN"<< std::endl;
    std::stack <unsigned short> m_tabDirEnvers;
    int j = sauvegarde.size()-1;
    while(!maPile.empty())
    {
        m_tabDirEnvers.push(maPile.top().getCoupPred());
        maPile.pop();
    }
    while(!m_tabDirEnvers.empty())
    {
        m_tabDir.push_back(m_tabDirEnvers.top());
        m_tabDirEnvers.pop();
    }

    afficherInfoTabDir();
    std::cout << " nombre de sauvegarde " << sauvegarde.size() << "nombre de sauvgarde" << std::endl;


    return true;
}


bool Maze::doublon(std::vector <SauvInfo> sauvegarde)
{
    for(int j=sauvegarde.size()-1; j>=0; j--)
    {
        if(sauvegarde[j].getPosPlayer()==m_pos_player && sauvegarde[j].getPosBoxes()==m_pos_boxes)
        {
            return true;
        }
    }
    return false;
}

void Maze::regenererPos(unsigned short pos_player,
                        std::vector<unsigned char> field,
                        std::vector<unsigned short> pos_boxes)
{
    m_pos_boxes = pos_boxes;
    m_field = field;
    m_pos_player = pos_player;
}
void Maze::animTabDir(Graphic g)
{
    std::cout << "Win ! " << std::endl;
    for(int i=0; i<getTabDir().size(); i++)
    {
        int a=clock();
        bool b=updatePlayer(getTabDir()[i]);
        while(clock()-a<50) {}
        g.clear();
        draw(g);
        g.display(Coord::m_nb_col);
    }
}

void Maze::afficherInfoTabDir()
{
    std::cout << "comb ";
    for(int i=0; i<m_tabDir.size(); i++)
    {
        std::cout << m_tabDir[i] << " , ";
    }
    std::cout << " comb" << std::endl;
    std::cout << "size " << m_tabDir.size() << " size" << std::endl;

}

bool Maze::pileModifiee(int sizePileActu, int sizePileAncienne)
{
    return(sizePileActu != sizePileAncienne ? true : false);
}

void Maze::detecteDeadBlocInCorner()
{
    for(int i=1; i< m_lig-1; i++)
    {
        for(int j=1; j< m_col-1; j++)
        {
            if(m_field[Coord::coord1D(i, j)]==SPRITE_GROUND || m_field[Coord::coord1D(i, j)]==SPRITE_MARIO)
            {
                for(int dir = 0; dir < 2; dir++)
                {
                    int caseTest = Coord::getDirPos(Coord::coord1D(i, j), dir);

                    if(isSquareWall(caseTest))
                    {
                        if(isSquareWall(Coord::getDirPos(Coord::coord1D(i, j), 2)) || isSquareWall(Coord::getDirPos(Coord::coord1D(i, j), 3)))
                            m_pos_deadBloc.push_back(Coord::coord1D(i, j));
                    }
                }
            }
        }
    }
}
void Maze::detecteDeadBlocFromDeadCorner()
{
    unsigned int a, b;
    unsigned int l, c;
    std::vector <unsigned short> m_Pos_DeadBlocFromDeadCorner;

    for(int i = 0; i< m_pos_deadBloc.size(); i++)
    {
        for(int j = i+1; j< m_pos_deadBloc.size(); j++)
        {
            Coord::coord2D(m_pos_deadBloc[i], l, c);
            Coord::coord2D(m_pos_deadBloc[j], a, b);

            if(l == a)
            {
                //std::cout << " 1 -> " << m_pos_deadBloc[i] << " . 2 -> " << m_pos_deadBloc[j] << std::endl;
                for(int dir = 0; dir < 2 ; dir ++)
                {
                    bool murContinu = true;
                    if(isSquareWall(Coord::getDirPos(m_pos_deadBloc[i], dir)) && isSquareWall(Coord::getDirPos(m_pos_deadBloc[j], dir)))
                    {
                        for(int k = std::min(c,b)+1; k< std::max(c,b); k++)
                        {
                            if(murContinu)
                            {
                                unsigned short caseTest = Coord::coord1D(a, k);
                                murContinu = isSquareWall(Coord::getDirPos(caseTest, dir));
                                if(m_field[caseTest] == SPRITE_GOAL || m_field[caseTest] == SPRITE_WALL || m_field[caseTest] == SPRITE_BOX_PLACED || m_field[caseTest] == SPRITE_BOX)
                                {
                                    murContinu = false;
                                }
                            }
                        }
                        if(murContinu)
                        {
                            for(int k = std::min(c,b)+1; k< std::max(c,b); k++)
                            {
                                m_Pos_DeadBlocFromDeadCorner.push_back(Coord::coord1D(a, k));
                            }
                        }
                    }
                }
            }
            if(c == b)
            {
                //std::cout << " 1 -> " << m_pos_deadBloc[i] << " . 2 -> " << m_pos_deadBloc[j] << std::endl;
                for(int dir = 2; dir < 4 ; dir ++)
                {
                    bool murContinu = true;
                    if(isSquareWall(Coord::getDirPos(m_pos_deadBloc[i], dir)) && isSquareWall(Coord::getDirPos(m_pos_deadBloc[j], dir)))
                    {
                        for(int k = std::min(a,l)+1; k< std::max(a,l); k++)
                        {
                            if(murContinu)
                            {
                                unsigned short caseTest = Coord::coord1D(k, c);
                                murContinu = isSquareWall(Coord::getDirPos(caseTest, dir));
                                if(m_field[caseTest] == SPRITE_GOAL || m_field[caseTest] == SPRITE_WALL || m_field[caseTest] == SPRITE_BOX_PLACED || m_field[caseTest] == SPRITE_BOX)
                                {
                                    murContinu = false;
                                }
                            }
                        }
                        if(murContinu)
                        {
                            for(int k = std::min(a,l)+1; k< std::max(a,l); k++)
                            {
                                m_Pos_DeadBlocFromDeadCorner.push_back(Coord::coord1D(k, c));
                            }
                        }
                    }
                }
            }
        }
    }
    m_pos_deadBloc.insert( m_pos_deadBloc.end(), m_Pos_DeadBlocFromDeadCorner.begin(), m_Pos_DeadBlocFromDeadCorner.end() );
}
void Maze::detecteDeadBloc()
{
    detecteDeadBlocInCorner();
    detecteDeadBlocFromDeadCorner();
    for(int i = 0; i< m_pos_deadBloc.size(); i++)
    {
        m_field[m_pos_deadBloc[i]] = SPRITE_DEADSQUARE;
    }
}

bool Maze::caseOnDeadBloc()
{
    for(int i=0; i< m_pos_boxes.size(); i++)
    {
        if(m_field[m_pos_boxes[i]] == SPRITE_BOX_DEADSQUARE)
        {
            return true;
        }
    }
    return false;

}

bool Maze::caseBoxMoveDifferently(std::vector <SauvInfo> tab1, int rangDebut, bool withZone)
{
    if(withZone)
    {
        for(int i=rangDebut; i< tab1.size(); i++)
        {
            if(tab1[i].getField() == m_field)
            {
                if(tab1[i].getFieldBoolZone()[m_pos_player])
                {
                    return false;
                }
            }
        }
    }
    else
    {
        for(int i=rangDebut; i< tab1.size(); i++)
        {
            if(tab1[i].getField() == m_field)
                return false;
        }
    }
    return true;
}






bool Maze::bfsCaseMoveOnly(bool deadBloc)
{
    bool win = false;
    int casePred;

    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0, createZone()));
    SauvInfo temp;
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0, createZone());

    std::queue <SauvInfo> maFile;
    maFile.push(sauvegarde[0]);

    while(!win)
    {
        while(!maFile.empty())
        {
            temp = maFile.front();
            maFile.pop();
            regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
            //std::cout << " posPlay " << m_pos_player << ", 0 " << m_pos_boxes[0] << " 1 " << m_pos_boxes[1] <<  std::endl;

            for(int j = 0; j< temp.getPosBoxes().size(); j++)
            {
                if(!win)
                {
                    for(int i = 0; i<4; i++)
                    {
                        if(!win)
                        {
                            win=updatePosBox(i, j);
                            if(!win)
                            {
                                if(!caseOnDeadBloc() && caseBoxMoveDifferently(sauvegarde, 0, false) && verifPossibilityMove(temp))
                                {

                                    int k = posDansTab(sauvegarde, temp);
                                    sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i, createZone()));
                                    maFile.push(sauvegarde[sauvegarde.size()-1]);
                                }
                            }
                            else if(win)
                            {
                                if(verifPossibilityMove(temp))
                                {
                                    int k = posDansTab(sauvegarde, temp);
                                    sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i, createZone()));
                                    win = backTrack(sauvegarde);
                                }
                                else
                                    win = false;

                            }
                        }
                        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
                    }
                }
            }
        }
    }
    regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
    std::cout << " sauvegarde " << sauvegarde.size() << " sauvegardes "<< std::endl;
    afficherInfoTabDir();

    return true;
}

std::vector <bool> Maze::createZone()
{
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);
    SauvInfo temp = saveDeb;
    std::vector <bool> fieldBoolZone(m_field.size(),false);
    fieldBoolZone[m_pos_player]=true;

    std::queue <SauvInfo> maFile;
    maFile.push(temp);
    maFile.front().setPred(-1);
    std::vector <SauvInfo> comb;
    bool win = false;
    regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());


    while(!maFile.empty())
    {
        temp = maFile.front(); //prend la case qui a etait mis a la file en premier
        maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer

        comb.push_back(temp);
        int casePred = comb.size()-1;
        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());//on remet le plateau � l'etat de temp


        for(int dir = 0; dir < 4; dir++)
        {
            win = updatePlayer(dir);
            if(!doublon(comb))//savoir si la situation s'est d�ja produite
            {
                if(m_field == saveDeb.getField())
                {
                    fieldBoolZone[m_pos_player] = true;
                    comb.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                    maFile.push(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                }
            }
            regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
        }
    }
    regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
    return fieldBoolZone;
}




void Maze::generateBoxVectDir(std::vector <SauvInfo> sauvegarde)
{
    int j = sauvegarde.size()-1;
    while(sauvegarde[j].getPred() != -1)
    {
        m_boxVectDir.push_back(sauvegarde[j]);
        j = sauvegarde[j].getPred();
    }
    m_boxVectDir.push_back(sauvegarde[0]);
}




bool Maze::backTrack(std::vector <SauvInfo> sauvegarde)
{
    generateBoxVectDir(sauvegarde);
    std::vector < unsigned short > tabEnvDir;
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);


    for(int i=1; i< m_boxVectDir.size(); i++)
    {
        std::queue <SauvInfo> maFile;
        maFile.push(m_boxVectDir[i]);
        maFile.front().setPred(-1);
        std::vector <SauvInfo> comb;
        bool continu = false;
        bool win = false;

        while(!maFile.empty())
        {
            if(!continu)
            {
                SauvInfo temp = maFile.front(); //prend la case qui a etait mis a la file en premier
                maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer

                comb.push_back(temp);
                int casePred = comb.size()-1;
                regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());//on remet le plateau � l'etat de temp


                for(int dir = 0; dir < 4; dir++)
                {
                    if(!continu)
                    {
                        win = updatePlayer(dir);

                        if(!doublon(comb))//savoir si la situation s'est d�ja produite
                        {
                            if(caseBoxMoveDifferently(comb, 0, false))
                            {
                                if(m_pos_boxes == m_boxVectDir[i-1].getPosBoxes())
                                {
                                    comb.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                                    int j = comb.size()-1;
                                    while(comb[j].getPred()!=-1)
                                    {
                                        tabEnvDir.push_back(comb[j].getCoupPred());
                                        j = comb[j].getPred();
                                        continu = true;
                                    }
                                    while(!maFile.empty())
                                    {
                                        maFile.pop();
                                    }
                                }
                            }
                            else
                            {
                                comb.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                                maFile.push(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                            }
                        }
                        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
                    }
                }
            }
        }
        if(continu == true) continu = false;
        else
        {
            std::cout << "ERREUR BACK" << std::endl;
            m_boxVectDir.erase(m_boxVectDir.begin(),m_boxVectDir.end());
            regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
            return false;
        }
    }
    std::reverse(tabEnvDir.begin(),tabEnvDir.end());
    m_tabDir = tabEnvDir;

    return true;
}






bool Maze::verifPossibilityMove(SauvInfo caseEgale)
{
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);
    std::vector <unsigned short> combBoxObjectif= m_pos_boxes;
    SauvInfo temp = caseEgale;

    std::queue <SauvInfo> maFile;
    maFile.push(temp);
    maFile.front().setPred(-1);
    std::vector <SauvInfo> comb;
    bool win = false;
    regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());

    while(!maFile.empty())
    {
        temp = maFile.front(); //prend la case qui a etait mis a la file en premier
        maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer

        comb.push_back(temp);
        int casePred = comb.size()-1;
        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());//on remet le plateau � l'etat de temp


        for(int dir = 0; dir < 4; dir++)
        {
            win = updatePlayer(dir);
            if(!doublon(comb))//savoir si la situation s'est d�ja produite
            {
                if(caseBoxMoveDifferently(comb, 0, false))
                {
                    if(m_pos_boxes == combBoxObjectif)
                    {
                        regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
                        return true;
                    }
                }
                else
                {
                    comb.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                    maFile.push(SauvInfo(m_pos_player,m_field,m_pos_boxes, casePred, dir));
                }
            }
            regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
        }
    }
    regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
    return false;
}





int Maze::posDansTab(std::vector <SauvInfo> tab1, SauvInfo tab2)
{
    for (int i=0; i<tab1.size(); i++)
    {
        if (tab1[i].getPosBoxes() == tab2.getPosBoxes())
        {
            return i;
        }
    }
    std::cout << "erreur dans la recherche de la valeur" << std::endl;
    return 0;
}


bool Maze::updatePosBox(char dir, int indiceVect)
{
    if (dir < 0 || dir > MAX_DIR)
    {
        std::cerr << "Maze::updatePlayer => Direction not correct... " << +dir << std::endl;
        return false;
    }

    char dirOposee;
    if(dir == 0) dirOposee = 1;
    if(dir == 1) dirOposee = 0;
    if(dir == 2) dirOposee = 3;
    if(dir == 3) dirOposee = 2;

    unsigned short oldPos = m_pos_boxes[indiceVect];
    unsigned short opositePos = Coord::getDirPos(oldPos,dirOposee);
    unsigned short newPos = Coord::getDirPos(oldPos,dir);

    //std::cout << " pos " << m_pos_boxes[indiceVect]  << ", nor  " << oldPos<< ", opo  " << opositePos << ", new  " << newPos << std::endl;
    //std::cout << " pos " << m_pos_boxes[indiceVect] << ", iBn " << isSquareBox(newPos) << ", iWn " << isSquareWall(newPos)<< ", iBo " << isSquareBox(opositePos) << ", iWo " << isSquareWall(opositePos) << std::endl;

    if(!isSquareBox(newPos) && !isSquareWall(newPos) && !isSquareBox(opositePos) && !isSquareWall(opositePos))
    {
        //std::cout << "===========>" << std::endl;
        if (m_field[newPos] == SPRITE_GOAL )
        {
            m_field[newPos] = SPRITE_BOX_PLACED;
            if (m_field[oldPos] == SPRITE_BOX_PLACED )
                m_field[oldPos] = SPRITE_GOAL;
            else if (m_field[oldPos] == SPRITE_BOX )
                m_field[oldPos] = SPRITE_GROUND;
            else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                m_field[oldPos] = SPRITE_DEADSQUARE;

        }
        else if (m_field[newPos] == SPRITE_GROUND )
        {
            m_field[newPos] = SPRITE_BOX;
            if (m_field[oldPos] == SPRITE_BOX_PLACED )
                m_field[oldPos] = SPRITE_GOAL;
            else if (m_field[oldPos] == SPRITE_BOX )
                m_field[oldPos] = SPRITE_GROUND;
            else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                m_field[oldPos] = SPRITE_DEADSQUARE;
        }
        else if (m_field[newPos] == SPRITE_DEADSQUARE )
        {
            m_field[newPos] = SPRITE_BOX_DEADSQUARE;
            if (m_field[oldPos] == SPRITE_BOX_PLACED )
                m_field[oldPos] = SPRITE_GOAL;
            else if (m_field[oldPos] == SPRITE_BOX )
                m_field[oldPos] = SPRITE_GROUND;
            else if (m_field[oldPos] == SPRITE_BOX_DEADSQUARE)
                m_field[oldPos] = SPRITE_DEADSQUARE;
        }
        m_pos_boxes[indiceVect] = newPos;
        m_pos_player = oldPos;
    }

    if(_isCompleted())
    {
        return true;
    }
    else
    {
        return false;
    }
}





bool Maze::heuristique(bool deadBloc)
{
    bool win = false;
    int casePred;
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);

    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));

    std::vector <SauvInfo> sauvegardeClass;
    sauvegardeClass.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0, poid()));

    SauvInfo temp;

    while(!win)
    {
        temp = sauvegardeClass.back();
        sauvegardeClass.pop_back();

        regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
        //std::cout << " posPlay " << m_pos_player << ", 0 " << m_pos_boxes[0] << " 1 " << m_pos_boxes[1] << " 2 " << m_pos_boxes[2] <<  std::endl;
        for(int j = 0; j< temp.getPosBoxes().size(); j++)
        {
            if(!win)
            {
                for(int i = 0; i<4; i++)
                {
                    if(!win)
                    {
                        win=updatePosBox(i, j);
                        if(!win)
                        {
                            if(!caseOnDeadBloc() && caseBoxMoveDifferently(sauvegarde, 0, false) && verifPossibilityMove(temp))
                            {
                                std::cout << " posPlay " << m_pos_player << " box, " << j << " = " << m_pos_boxes[j] << " dir " << i <<  std::endl;
                                int ponderation = poid();
                                std::cout << " sortiPoid " << ponderation << std::endl;

                                int k = posDansTab(sauvegarde, temp);
                                sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i, ponderation));
                                int a=0;
                                while(sauvegardeClass[a].getPonderation() <= sauvegarde.back().getPonderation())
                                {
                                    a++;
                                }
                                sauvegardeClass.insert(sauvegardeClass.begin()+a,sauvegarde.back());
                            }
                        }
                        else if(win)
                        {
                            if(verifPossibilityMove(temp))
                            {
                                int k = posDansTab(sauvegarde, temp);
                                sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i));
                                win = backTrack(sauvegarde);
                            }
                            else
                                win = false;
                        }
                    }
                    regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
                }
            }
        }

    }
    regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
    std::cout << " sauvegarde " << sauvegarde.size() << " sauvegardes "<< std::endl;
    afficherInfoTabDir();
    return true;
}



int Maze::poid()
{
    bool win = false;
    int casePred;

    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);
    SauvInfo temp = saveDeb;


    std::queue <SauvInfo> maFile;
    maFile.push(sauvegarde[0]);

    regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());


    int poid = 0;

    for(int j = 0; j< temp.getPosBoxes().size(); j++)
    {
        if(temp.getPosBoxes()[j] != SPRITE_BOX_PLACED)
        {
            std::cout << " quelle box, "<< m_pos_boxes[j] <<  std::endl;
            while(!maFile.empty())
            {
                temp = maFile.front();
                maFile.pop();
                regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());

                for(int i = 0; i<4; i++)
                {
                    win=updatePosBox(i, j);
                    if(!win)
                    {
                        if(!doublon(sauvegarde))
                        {
                            std::cout << " ====> "<< std::endl;
                            int k = posDansTab(sauvegarde, temp);
                            sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i));
                            maFile.push(sauvegarde.back());
                        }
                    }
                    regenererPos(temp.getPosPlayer(), temp.getField(), temp.getPosBoxes());
                }
            }
        }
        else
        poid = poid-10;

    }

    int l= sauvegarde.size()-1;
    while(sauvegarde[l].getPred() != -1)
    {
        poid++;
        l= sauvegarde[l].getPred();
    }

    regenererPos(saveDeb.getPosPlayer(), saveDeb.getField(), saveDeb.getPosBoxes());
    return poid;
}








/*
bool Maze::bfsCaseMoveOnly(bool deadBloc)
{
    bool win = false;
    int casePred;

    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));
    SauvInfo temp;
    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);

    int tailleDebutNouvelleSauv = sauvegarde.size();
    int tailleSauv = sauvegarde.size()-1;

    while(!win)
    {
        tailleDebutNouvelleSauv = tailleSauv;
        tailleSauv = sauvegarde.size();

        for(int k=tailleDebutNouvelleSauv; k < tailleSauv; k++)
        {
            for(int j = 0; j< sauvegarde[k].m_pos_boxes.size(); j++)
            {
                temp = sauvegarde[k];
                regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);

                for(int i = 0; i<4; i++)
                {
                    if(!win)
                    {
                        //std::cout<< k << std::endl;
                        win=updatePosBox(i, j);
                        if(!win)
                        {

                            if(!caseOnDeadBloc() && caseBoxMoveDifferently(sauvegarde, m_pos_boxes, 0) && verifPossibilityMove(temp))
                            {
                                sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i));
                            }
                        }
                        else if(win)
                        {
                            sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,k, i));
                            win = backTrack(sauvegarde);
                        }
                    }
                    regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);
                }
            }
        }
    }
    regenererPos(saveDeb.m_pos_player, saveDeb.m_field, saveDeb.m_pos_boxes);
    std::cout << " sauvegarde " << sauvegarde.size() << std::endl;
    afficherInfoTabDir();

    return true;
}
*/









/*


bool Maze::bfsCaseMoveOnly(bool deadBloc)
{
    bool win = false;

    std::vector <SauvInfo> sauvegarde;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));

    std::queue <SauvInfo> maFile;
    maFile.push(sauvegarde[0]);
    std::vector <SauvInfo> sauvegardeDeplacementJoueur;
    sauvegardeDeplacementJoueur.push_back(sauvegarde[0]);

    while(!win)
    {
        //for(int i=sauvegarde.size()-1; i>=0 ;i--){ maFile.push(sauvegarde[i]);}
        //for(int i=0; i< sauvegarde.size() ;i++){ maFile.push(sauvegarde[i]);}
        while(!maFile.empty())
        {
            SauvInfo temp = maFile.front(); //prend la case qui a etait mis a la file en premier
            maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer
            regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp

            sauvegardeDeplacementJoueur.push_back(temp);
            int caseTemp = sauvegardeDeplacementJoueur.size()-1;
            //std::cout << " cou" << " sauv " << sauvegarde[0].m_pred << " sauvT "<< sauvegardeDeplacementJoueur[0].m_pred << std::endl;


            for(int i = 0; i<4; i++)
            {

               // std::cout << " coucou i: " << i << " sauv " << sauvegarde.size() << " sauvT " << sauvegardeDeplacementJoueur.size() << " caseM " << caseMove(temp.m_pos_boxes,m_pos_boxes) << std::endl;
                if(!win)
                {
                    std::cout << " PosPlayer : " << m_pos_player << std::endl;
                    win=updatePlayer(i);
                    if(!win)
                    {
                        std::cout << " sousou i: " << i << " sauv " << sauvegarde.size() << " sauvT " << sauvegardeDeplacementJoueur.size() << " caseM " << caseMove(temp.m_pos_boxes,m_pos_boxes) << " doublon " << doublon(sauvegardeDeplacementJoueur) << " player " << m_pos_player <<std::endl;

                        if(!doublon(sauvegardeDeplacementJoueur))//savoir si la situation s'est d�ja produite
                        {
                            if(caseMove(temp.m_pos_boxes,m_pos_boxes))
                            {
                                /*std::vector <unsigned short> chemin;
                                int pred = sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1].m_pred;
                                while(pred != -1)
                                {
                                    chemin.push_back(pred);
                                    pred = sauvegardeDeplacementJoueur[pred].m_pred;
                                }
                                //std::cout << " GOUGOU i: " << i << " sauv " << sauvegarde.size() << " sauvT " << sauvegardeDeplacementJoueur.size() << " caseM " << caseMove(temp.m_pos_boxes,m_pos_boxes)<< " pred " << pred << " player " << m_pos_player << std::endl;

                                for(int j = chemin.size()-1; j >= 0; j--)
                                {
                                    SauvInfo save = sauvegardeDeplacementJoueur[chemin[j]];
                                    sauvegarde.push_back(SauvInfo(save.m_pos_player,save.m_field,save.m_pos_boxes,save.m_pred,save.m_coupPred));
                                }
                                if(!doublon(sauvegarde))
                                {
                                    SauvInfo save = sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1];
                                    sauvegarde.push_back(SauvInfo(save.m_pos_player,save.m_field,save.m_pos_boxes,save.m_pred,save.m_coupPred));
                                    regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp
                                }
                            }
                            else if(!caseMove(temp.m_pos_boxes,m_pos_boxes))
                            {
                                sauvegardeDeplacementJoueur.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,caseTemp,i));
                                maFile.push(sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1]);
                                std::cout << sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1].m_pos_player << std::endl;
                                regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp
                            }
                        }
                        double a = clock();
                        while(clock()-a < 1000){}
                    }
                    else if(win)
                    {
                       /* std::vector <unsigned short> chemin;
                        int pred = sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1].m_pred;
                        while(pred != -1)
                        {
                            chemin.push_back(pred);
                            pred = sauvegardeDeplacementJoueur[pred].m_pred;
                        }
                        for(int j = chemin.size()-1; j >= 0; j--)
                        sauvegarde.push_back(sauvegardeDeplacementJoueur[chemin[j]]);

                        SauvInfo save = sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1];
                        sauvegarde.push_back(SauvInfo(save.m_pos_player,save.m_field,save.m_pos_boxes,save.m_pred,save.m_coupPred));
                        regenererPos(sauvegarde[0].m_pos_player,sauvegarde[0].m_field,sauvegarde[0].m_pos_boxes);
                    }
                }
            }
            std::cout << " sizeFile : " << maFile.size() << std::endl;
        if(maFile.empty())
        {
            for(int i=0; i<sauvegarde.size();i++)
            std::cout<< " sauv " << i+1 << " " << sauvegarde.front().m_pos_player << std::endl;
        }
        if(maFile.empty())
        while(0==0){std::cout<<"vide"<< std::endl;}
        sauvegardeDeplacementJoueur.erase(sauvegardeDeplacementJoueur.begin()+1,sauvegardeDeplacementJoueur.end());
        std::cout << std::endl;
        std::cout << std::endl;
        }

    }




    std::stack <unsigned short> tabDirEnvers;
    int j = sauvegarde.size()-1;
    while(sauvegarde[j].m_pred!=-1)
    {
        tabDirEnvers.push(sauvegarde[j].m_coupPred);
        j = sauvegarde[j].m_pred;
    }
    while(!tabDirEnvers.empty())
    {
        m_tabDir.push_back(tabDirEnvers.top());
        tabDirEnvers.pop();
    }

    afficherInfoTabDir();
    std::cout << " nombre de sauvegarde " << sauvegarde.size() << "nombre de sauvgarde" << std::endl;

    return true;


*/









/*



bool Maze::bfsCaseMoveOnly(bool deadBloc)
{
    bool win = false;

    std::vector <SauvInfo> sauvegarde;

    std::queue <SauvInfo> maFile;
    maFile.push(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));

    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);

    std::vector <SauvInfo> sauvegardeDeplacementJoueur;
    sauvegardeDeplacementJoueur.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));

    while(!win)
    {
        for(int i=0; i< sauvegarde.size() ;i++){ maFile.push(sauvegarde[i]);}

        while(!maFile.empty())
        {
            SauvInfo temp = maFile.front(); //prend la case qui a etait mis a la file en premier
            maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer
            regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp

            sauvegardeDeplacementJoueur.push_back(temp);
            int caseTemp = sauvegardeDeplacementJoueur.size()-1;
            //std::cout << " PosPlayer : " << m_pos_player << std::endl;

            for(int i = 0; i<4; i++)
            {
                if(!win)
                {
                    win=updatePlayer(i);
                    if(!win)
                    {
                        //std::cout << " sousou i: " << i << " sauv " << sauvegarde.size() << " sauvT " << sauvegardeDeplacementJoueur.size() << " caseM " << caseBoxExist(sauvegardeDeplacementJoueur,m_pos_boxes) << " doublon " << doublon(sauvegardeDeplacementJoueur) << " player " << m_pos_player <<std::endl;

                                sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,caseTemp,i));
                            }
                            else //if (caseBoxExist(sauvegardeDeplacementJoueur,m_pos_boxes))
                            {
                                sauvegardeDeplacementJoueur.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,caseTemp,i));
                                maFile.push(sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1]);
                                //std::cout << sauvegardeDeplacementJoueur[sauvegardeDeplacementJoueur.size()-1].m_pos_player << " sizeFile : " << maFile.size() << std::endl;
                            }
                        }
                        regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp
                        //double a = clock();
                        //while(clock()-a < 100){}
                    }
                    else if(win)
                    {
                        sauvegarde.push_back(SauvInfo(temp.m_pos_player,temp.m_field, temp.m_pos_boxes,caseTemp,i));
                        regenererPos(sauvegardeDeplacementJoueur[0].m_pos_player,sauvegardeDeplacementJoueur[0].m_field,sauvegardeDeplacementJoueur[0].m_pos_boxes);
                    }
                }
            }
        }
    }


std::cout << "========================================>"<< std::endl;

    std::stack <unsigned short> tabDirEnvers;
    int j = sauvegarde[sauvegarde.size()-1].m_pred;
    while(sauvegardeDeplacementJoueur[j].m_pred!=-1)
    {
        tabDirEnvers.push(sauvegardeDeplacementJoueur[j].m_coupPred);
        j = sauvegardeDeplacementJoueur[j].m_pred;
    }
    while(!tabDirEnvers.empty())
    {
        m_tabDir.push_back(tabDirEnvers.top());
        tabDirEnvers.pop();
    }

    afficherInfoTabDir();
    std::cout << " nombre de sauvegarde " << sauvegarde.size() << "nombre de sauvgarde" << std::endl;
    return true;
}




*/






/*


 bool win = false;
    int casePred;

    std::vector <SauvInfo> sauvegarde;
    std::vector <SauvInfo> sauvegardeDeplacementJoueur;
    sauvegarde.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));
    sauvegardeDeplacementJoueur.push_back(SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0));


    std::queue <SauvInfo> maFile;

    SauvInfo saveDeb = SauvInfo(m_pos_player, m_field, m_pos_boxes,-1,0);


    while(!win)
    {
        for(int i=0; i< sauvegarde.size() ;i++){ maFile.push(sauvegarde[i]);}

        while(!maFile.empty())
        {

            SauvInfo temp = maFile.front(); //prend la case qui a etait mis a la file en premier
            maFile.pop();//et on supprime de la file, cette case que l'on vient de temporairement recuperer
            regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);//on remet le plateau � l'etat de temp
            casePred = posDansTab(sauvegarde,temp);

            //std::cout << " PosPlayer : " << temp.m_pos_player << " casePred "<< casePred << std::endl;

            for(int i = 0; i<4; i++)
            {
                if(!win)
                {
                    win=updatePlayer(i);
                    if(!win)
                    {
                        //std::cout << " sousou i: " << i << " sauv " << sauvegarde.size() << " sauvT " << sauvegardeDeplacementJoueur.size() << " caseM " << caseBoxMoveDifferently(sauvegarde,m_pos_boxes) << " doublon " << doublon(sauvegarde) << " doublonT " << doublon(sauvegardeDeplacementJoueur) << " player " << m_pos_player <<std::endl;
                        if(!doublon(sauvegardeDeplacementJoueur))//savoir si la situation s'est d�ja produite
                        {
                            if(caseBoxMoveDifferently(sauvegarde, m_pos_boxes))
                            {
                                sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,casePred, i));
                            }
                            else
                            {
                                sauvegardeDeplacementJoueur.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,-2, i));
                                maFile.push(SauvInfo(m_pos_player,m_field,m_pos_boxes,-2, i));
                            }
                        }
                        regenererPos(temp.m_pos_player, temp.m_field, temp.m_pos_boxes);
                    }
                    else if(win)
                    {
                        sauvegarde.push_back(SauvInfo(m_pos_player,m_field,m_pos_boxes,casePred, i));
                    }
                }
            }
            /*
            std::cout << std::endl;
            std::cout << " sauv " << std::endl;
            for(int j = 0; j< sauvegarde.size(); j++)
            {
                std::cout << sauvegarde[j].m_pos_player << " , ";
            }
            std::cout << std::endl;

            std::cout << " sauvT ";
            for(int j = 0; j< sauvegardeDeplacementJoueur.size(); j++)
            {
                std::cout << sauvegardeDeplacementJoueur[j].m_pos_player << " , ";
            }
            std::cout << std::endl;*/
// }
/*
std::cout << std::endl;
std::cout << std::endl;
std::cout << std::endl;
std::cout << std::endl;

sauvegardeDeplacementJoueur.erase(sauvegardeDeplacementJoueur.begin(),sauvegardeDeplacementJoueur.end());
sauvegardeDeplacementJoueur.push_back(sauvegarde.front());

}

std::cout << std::endl;
std::cout << std::endl;
std::cout << std::endl;
std::cout << " sauv " << std::endl;
for(int j = 0; j< sauvegarde.size(); j++)
{
std::cout << j << " - " << sauvegarde[j].m_pos_player << " , " << sauvegarde[j].m_pred  << " , " << sauvegarde[j].m_coupPred << std::endl;
}
std::cout << std::endl;
regenererPos(saveDeb.m_pos_player, saveDeb.m_field, saveDeb.m_pos_boxes);


*/



