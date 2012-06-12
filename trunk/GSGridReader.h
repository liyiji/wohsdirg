/// File Name : GSGridReader.h

#ifndef GSGRIDREADER_H
#define GSGRIDREADER_H

#include <QString>

#include <Inventor/SbVec.h>

/**
先是121行的COORD，每行2个坐标，每个坐标是3个浮点数
然后是1333*6+1*2，总共8000个ZCORN
然后是属性
1000个poro(孔隙度？)
1000个perm(渗透率？)
1000个ACTNUM，这是个啥？
*/

class GSGridReader
{
public:
    GSGridReader(QString fileName);
    virtual ~GSGridReader();

public:
    int readFile();

    void getIJKCount(int &i, int &j, int &k);
    QVector<SbVec3f> getCOORD();
    QVector<float> getZCORN();
    QVector<float> getPORO();
    QVector<float> getPERM();
    QVector<bool> getACTNUM();

private:
    void clearData();
    bool checkData();

private:
    QString m_sFileName;

    int m_i, m_j, m_k;

    QVector<SbVec3f> m_vallCOORD;
    QVector<float> m_vallZCORN;
    QVector<float> m_vallPORO;
    QVector<float> m_vallPERM;
    QVector<bool> m_vallACTNUM;
};

#endif // GSGRIDREADER_H
