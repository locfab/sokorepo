#ifndef SAUVINFO_H_INCLUDED
#define SAUVINFO_H_INCLUDED

#include <vector>
#include <string>
#include <iostream>



class SauvInfo
{
    public:
        unsigned short m_pos_player;
        unsigned short m_coupPred;
        char m_dir_player;


        std::vector<unsigned char> m_field; // field
        std::vector<unsigned short> m_pos_boxes; // box positions
        std::vector<unsigned short> m_pos_goals; // goal positions
        int m_pred;


    public:
        SauvInfo();
        SauvInfo(unsigned short pos_player,
                 std::vector<unsigned char> field,
                 std::vector<unsigned short> pos_boxes,
                 int pred,
                 unsigned short coupPred);
        ~SauvInfo();



};

#endif // SAUVINFO_H_INCLUDED
