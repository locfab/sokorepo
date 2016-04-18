#include "infoSauv.h"


infoSauv::infoSauv(){}

infoSauv::infoSauv(unsigned short pos_player,
                   std::vector<unsigned char> field,
                   std::vector<unsigned short> pos_boxes,
                   int pred)

{
    m_pos_player = pos_player;
    m_field = field; // field
    m_pos_boxes = pos_boxes; // box positions
    m_pred = pred;
}

infoSauv::~infoSauv()
{
}

