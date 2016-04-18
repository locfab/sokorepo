#ifndef INFOSAUV_H_INCLUDED
#define INFOSAUV_H_INCLUDED

#include <vector>
#include <string>



class infoSauv
{
    public:
        unsigned short m_pos_player;
        char m_dir_player;


        std::vector<unsigned char> m_field; // field
        std::vector<unsigned short> m_pos_boxes; // box positions
        std::vector<unsigned short> m_pos_goals; // goal positions
        int m_pred;


    public:
        infoSauv();
        infoSauv(unsigned short pos_player,
                 std::vector<unsigned char> field,
                 std::vector<unsigned short> pos_boxes,
                 int pred);
        ~infoSauv();



};
#endif // INFOSAUV_H_INCLUDED
