#ifndef SAUVINFO_H_INCLUDED
#define SAUVINFO_H_INCLUDED

#include <vector>
#include <string>
#include <iostream>



class SauvInfo
{
    private:
        unsigned short m_pos_player;

        std::vector<unsigned char> m_field; // field
        std::vector<unsigned short> m_pos_boxes; // box positions
        std::vector<unsigned short> m_pos_goals; // goal positions
        std::vector <bool> m_fieldBoolZone;


        unsigned short m_coupPred;
        int m_pred;
        int m_ponderation;

    public:



    public:
        SauvInfo();
        SauvInfo(unsigned short pos_player,
                 std::vector<unsigned char> field,
                 std::vector<unsigned short> pos_boxes,
                 int pred,
                 unsigned short coupPred,
                 std::vector <bool> fieldBoolZone,
                 int ponderation = 1);

        SauvInfo(unsigned short pos_player,
        std::vector<unsigned char> field,
        std::vector<unsigned short> pos_boxes,
        int pred,
        unsigned short coupPred,
        int ponderation = 1);

        ~SauvInfo();

        unsigned short getPosPlayer() const;
        unsigned int getSize() const;
        const std::vector<unsigned char>& getField() const;
        const std::vector<unsigned short>& getGoals() const;
        std::vector<unsigned short> getPosBoxes() const;
        const std::vector<bool>& getFieldBoolZone() const;


        int getPred() const;
        int getPonderation() const;
        unsigned short getCoupPred() const;

        void setPred(int pred);






};

inline void SauvInfo::setPred(int pred)
{
    this->m_pred = pred;
}


inline const std::vector<unsigned short>& SauvInfo::getGoals() const { return this->m_pos_goals; }
inline const std::vector<unsigned char>& SauvInfo::getField() const { return this->m_field; }
inline const std::vector<bool>& SauvInfo::getFieldBoolZone() const { return this->m_fieldBoolZone; }


inline unsigned int SauvInfo::getSize() const { return this->m_field.size(); }
inline std::vector<unsigned short> SauvInfo::getPosBoxes() const { return m_pos_boxes; }
inline unsigned short SauvInfo::getPosPlayer() const { return m_pos_player;}
inline int SauvInfo::getPred() const { return m_pred;}
inline int SauvInfo::getPonderation() const { return m_ponderation;}
inline unsigned short SauvInfo::getCoupPred() const { return m_coupPred;}


#endif // SAUVINFO_H_INCLUDED
