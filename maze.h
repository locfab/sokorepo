/************************************************************
Sokoban project - Maze file
Copyright Florent DIEDLER
Date : 27/02/2016

Please do not remove this header, if you use this file !
************************************************************/

#ifndef MAZE_H_INCLUDED
#define MAZE_H_INCLUDED

#include "SauvInfo.h"
#include <vector>
#include <string>
#include <queue>
#include <stack>





// Max size for the field
#define NB_MAX_WIDTH     30
#define NB_MAX_HEIGHT    30

class Graphic;

enum
{
    SPRITE_GROUND = 0, SPRITE_WALL = 1, SPRITE_BOX = 2,
    SPRITE_BOX_PLACED = 3, SPRITE_GOAL = 4, SPRITE_MARIO = 5,
    SPRITE_DEADSQUARE = 9, SPRITE_BOX_DEADSQUARE = 6
};

class Maze
{
    private:
        unsigned char m_lig, m_col;
        unsigned short m_pos_player;
        char m_dir_player;
        std::string m_level_path;

        std::vector<unsigned char> m_field; // field
        std::vector<unsigned short> m_pos_boxes; // box positions
        std::vector<unsigned short> m_pos_goals; // goal positions
        std::vector<unsigned short> m_pos_deadBloc;
        std::vector<unsigned short> m_tabDir;


        std::vector<unsigned short> pos_boxesSauv;
        std::vector<unsigned short> pos_goalsSauv;
        unsigned short pos_playerSauv;
        std::vector<unsigned char> fieldSauv;

        friend std::ostream& operator << (std::ostream& O, const Maze& n);

        bool _isCompleted() const;
        bool _canPushBox(unsigned short posBox, char dir, unsigned short& newPosBox) const;
        bool _load(const std::string& path);
        void detecteDeadBlocInCorner();
        void detecteDeadBlocFromDeadCorner();

    public:
        Maze(const std::string& path);
        ~Maze();

        bool init();
        bool updatePlayer(char dir);
        void draw(const Graphic& g) const;

        // Specific getters for field
        bool isSquareWalkable(unsigned short pos) const;
        bool isSquareGround(unsigned short pos) const;
        bool isSquareBox(unsigned short pos) const;
        bool isSquareGoal(unsigned short pos) const;
        bool isSquareWall(unsigned short pos) const;
        bool isSquareBoxPlaced(unsigned short pos) const;

        // Other getters
        const std::string& getLevelPath() const;
        unsigned short getPosPlayer() const;
        unsigned int getSize() const;
        void setSquare(unsigned short pos, unsigned char s);
        const std::vector<unsigned char>& getField() const;
        const std::vector<unsigned short>& getGoals() const;
        std::vector<unsigned short> getPosBoxes() const;
        std::vector<unsigned short> getTabDir() const;

        // Setter
        void setPlayerPos(unsigned short p);

        bool brutForce(bool deadBloc);
        bool bfs(bool deadBloc);
        bool dfs(bool deadBloc);

        //fonction pour les bfs,dfs,brutfoce
        void animTabDir(Graphic g);
        bool doublon(std::vector <SauvInfo> sauvegarde);
        void regenererPos(unsigned short pos_player,
                          std::vector<unsigned char> field,
                          std::vector<unsigned short> pos_boxes);
        void afficherInfoTabDir();
        bool pileModifiee(int sizePileActu, int sizePileAncienne);

        //pour les deadbloc
        void detecteDeadBloc();
        bool caseOnDeadBloc();

};

// Inline implementation of getters and setters (DO NOT TOUCH !)

inline void Maze::setPlayerPos(unsigned short p) {
    if (p < this->getSize()) // NB: p always > 0 because unsigned short
        this->m_pos_player = p;
}

inline const std::string& Maze::getLevelPath() const { return this->m_level_path; }
inline const std::vector<unsigned short>& Maze::getGoals() const { return this->m_pos_goals; }
inline const std::vector<unsigned char>& Maze::getField() const { return this->m_field; }

inline unsigned int Maze::getSize() const { return this->m_field.size(); }
inline std::vector<unsigned short> Maze::getPosBoxes() const { return m_pos_boxes; }
inline std::vector<unsigned short> Maze::getTabDir() const {return m_tabDir;}
inline unsigned short Maze::getPosPlayer() const { return m_pos_player;}

inline void Maze::setSquare(unsigned short pos, unsigned char s)
{
    if (pos < (this->m_lig*this->m_col))
    {
        this->m_field[pos] = s;
    }
}

inline bool Maze::isSquareWalkable(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_GROUND || this->m_field[pos] == SPRITE_GOAL || this->m_field[pos] == SPRITE_DEADSQUARE) ? true : false);
}

inline bool Maze::isSquareGround(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_GROUND) ? true : false);
}

inline bool Maze::isSquareBox(unsigned short pos) const
{
    return ((this->m_field[pos] == SPRITE_BOX || this->m_field[pos] == SPRITE_BOX_PLACED || this->m_field[pos] == SPRITE_BOX_DEADSQUARE) ? true : false);
}

inline bool Maze::isSquareGoal(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_GOAL ? true : false);
}

inline bool Maze::isSquareWall(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_WALL ? true : false);
}

inline bool Maze::isSquareBoxPlaced(unsigned short pos) const
{
    return (this->m_field[pos] == SPRITE_BOX_PLACED ? true : false);
}

#endif // MAZE_H_INCLUDED
