#include "SauvInfo.h"


SauvInfo::SauvInfo(){}

SauvInfo::SauvInfo(unsigned short pos_player,
                   std::vector<unsigned char> field,
                   std::vector<unsigned short> pos_boxes,
                   int pred,
                   unsigned short coupPred,
                   std::vector <bool> fieldBoolZone,
                   int ponderation)

{
    m_pos_player = pos_player;
    m_field = field; // field
    m_pos_boxes = pos_boxes; // box positions
    m_pred = pred;
    m_coupPred = coupPred;
    m_fieldBoolZone = fieldBoolZone;
}

SauvInfo::SauvInfo(unsigned short pos_player,
         std::vector<unsigned char> field,
         std::vector<unsigned short> pos_boxes,
         int pred,
         unsigned short coupPred,
         int ponderation)

{
    m_pos_player = pos_player;
    m_field = field; // field
    m_pos_boxes = pos_boxes; // box positions
    m_pred = pred;
    m_coupPred = coupPred;
}


SauvInfo::~SauvInfo()
{
}
