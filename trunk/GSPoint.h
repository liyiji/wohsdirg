/// File Name : GSPoint.h

#ifndef GSPOINT_H
#define GSPOINT_H

#include <QVector>
#include <Inventor/SbVec.h>

bool is2Power(int num);
int getIntFrom3Bool(bool iPlus, bool jPlus, bool kPlus);
int getIndexFrom3Bool(bool iPlus, bool jPlus, bool kPlus);
int mergeMultiple(int mul1, int mul2);
bool singleIsInMultiple(int single, int multiple);
QVector<int> splitMultiple(int multiple);

struct DirectionalPoint
{
    int directions;
    /*
    i+1, j+1, k+1 | i+1, j+1, k | i+1, j, k+1 | i+1, j, k | i, j+1, k+1 | i, j+1, k | i, j, k+1 | i, j, k
          1               1            1             1          1             1          1           1
        128              64           32            16          8             4          2           1
    2^0 ~ 2^8-1
    1 ~ 255
    */
    SbVec3f coord;
};

class GSPoint
{
public:
    GSPoint();
    GSPoint(int i, int j, int k);
    virtual ~GSPoint();

public:
    void setIJK(int i, int j, int k);
    void getIJK(int &i, int &j, int &k);

    void setPointCoord(int directions, SbVec3f coord, bool bDebug = false);
    SbVec3f getPointCoord(int index, bool bDebug = false);
    SbVec3f getPointCoord(bool iPlus, bool jPlus, bool kPlus, bool bDebug = false);
    int getPointIndex(bool iPlus, bool jPlus, bool kPlus);

    bool isSameCoordByDirection(int d1, int d2);

    void debugCoords();

private:
    void setInitData();

private:
    int m_i, m_j, m_k;
    QVector<DirectionalPoint> m_vecCoords; /// 8个点
};

#endif // GSPOINT_H
