/// File Name : GSPoint.cpp

#include "GSPoint.h"
#include "GSPublicDefine.h"
#include "GSPublicFunctions.h"

GSPoint::GSPoint()
{
    setInitData();
}

GSPoint::GSPoint(int i, int j, int k)
{
    setInitData();
    setIJK(i, j, k);
}

GSPoint::~GSPoint()
{
}

void GSPoint::setIJK(int i, int j, int k)
{
    if (i > m_i && j > m_j && k > m_k)
    {
        m_i = i;
        m_j = j;
        m_k = k;
    }
}

void GSPoint::getIJK(int &i, int &j, int &k)
{
    i = m_i;
    j = m_j;
    k = m_k;
}

void GSPoint::setPointCoord(int directions, SbVec3f coord, bool bDebug)
{
    if (directions >= 256 || directions <= 0)
    {
        return;
    }

    int sameCoordIndex = -1;

    for (int i = 0; i < m_vecCoords.size(); i++)
    {
        if (coord == m_vecCoords.at(i).coord)
        {
            sameCoordIndex = i;
            break;
        }
    }

    for (int j = 0; j < m_vecCoords.size(); j++)
    {
        DirectionalPoint &dp = m_vecCoords[j];
        int cross = dp.directions & directions;
        if (cross)
        {
            dp.directions -= cross;
        }
    }

    if (sameCoordIndex == -1) // 无相同coord时
    {
        DirectionalPoint newDP;
        newDP.coord = coord;
        newDP.directions = directions;
        m_vecCoords.append(newDP);

        // 清理掉directions为0的点
        for (int i = 0; i < m_vecCoords.size(); )
        {
            DirectionalPoint &dp = m_vecCoords[i];
            if (dp.directions == 0)
            {
                m_vecCoords.remove(i);
            }
            else
            {
                i++;
            }
        }
    }
    else // 有相同coord时
    {
        int merged = m_vecCoords[sameCoordIndex].directions | directions;
        m_vecCoords[sameCoordIndex].directions = merged;
    }
}

SbVec3f GSPoint::getPointCoord(int index, bool bDebug)
{
    for (int i = 0; i < m_vecCoords.size(); i++)
    {
        bool inMul = singleIsInMultiple(index, m_vecCoords.at(i).directions);
        if (inMul)
        {
            return m_vecCoords.at(i).coord;
        }
    }
    return g_invalidCoord;
}

SbVec3f GSPoint::getPointCoord(bool iPlus, bool jPlus, bool kPlus, bool bDebug)
{
    int fromBool = getIntFrom3Bool(iPlus, jPlus, kPlus);
    return getPointCoord(fromBool, bDebug);
}

int GSPoint::getPointIndex(bool iPlus, bool jPlus, bool kPlus)
{
    /// 得到 0-7之间的数
    int index = 0;
    if (kPlus) index += 4;
    if (jPlus) index += 2;
    if (iPlus) index += 1;
    return index;
}

bool GSPoint::isSameCoordByDirection(int d1, int d2)
{
    if (d1 < 256 && d1 > 0 && d2 < 256 && d2 > 0)
    {
        if (is2Power(d1) == false || is2Power(d2) == false)
        {
            return false;
        }
        else
        {
            for (int i = 0 ; i < m_vecCoords.size(); i++)
            {
                if ((m_vecCoords.at(i).directions && d1) && (m_vecCoords.at(i).directions && d2))
                {
                    return true;
                }
            }
            return false;
        }
    }
    else
    {
        return false;
    }
}

void GSPoint::debugCoords()
{
    if (m_vecCoords.size() > 2)
    {
        BP;
    }
    for (int i = 0; i < m_vecCoords.size(); i++)
    {
        if (m_vecCoords[i].coord != g_invalidCoord)
        {
            qDebug() << m_i << m_j << m_k;
            qDebug() << i << m_vecCoords[i].directions;
            debug(m_vecCoords[i].coord);
        }
    }
}

bool is2Power(int num)
{
    if (num <= 0)
        return false;

    int tmp = 1;
    bool b = false;
    while (1)
    {
        if (num == tmp)
        {
            b = true;
            break;
        }
        tmp *= 2;
        if (tmp > num)
        {
            break;
        }
    }
    return b;
}

int getIntFrom3Bool(bool iPlus, bool jPlus, bool kPlus)
{
    /// 得到 1-128之间的数
    int output = 1;
    int nth = getIndexFrom3Bool(iPlus, jPlus, kPlus);
    for (int i = 0; i < nth; i++)
    {
        output *= 2;
    }
    return output;
}

int getIndexFrom3Bool(bool iPlus, bool jPlus, bool kPlus)
{
    int nth = 0;
    if (kPlus) nth += 4;
    if (jPlus) nth += 2;
    if (iPlus) nth += 1;
    return nth;
}

int mergeMultiple(int mul1, int mul2)
{
    if (mul1 < 256 && mul1 > 0 && mul2 < 256 && mul2 > 0)
    {
        int merged = mul1 | mul2;
        if (merged < 256 && merged > 0)
        {
            return merged;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

bool singleIsInMultiple(int single, int multiple)
{
    if (is2Power(single) == false)
    {
        return false;
    }

    if (single & multiple)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QVector<int> splitMultiple(int multiple)
{
    QVector<int> splited;
    if (multiple < 256 && multiple > 0)
    {
        int num[8] = {128, 64, 32, 16, 8, 4, 2, 1};
        for (int i = 0; i < 8; i++)
        {
            if (num[i] & multiple)
            {
                splited.append(num[i]);
            }
        }
    }
    return splited;
}

void GSPoint::setInitData()
{
    m_i = -1;
    m_j = -1;
    m_k = -1;
    DirectionalPoint dp;
    dp.directions = 255;
    dp.coord = g_invalidCoord;
    m_vecCoords.append(dp);
}
