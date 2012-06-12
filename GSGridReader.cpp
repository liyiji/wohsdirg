/// File Name : GSGridReader.cpp

#include <QFile>
#include <QVector>
#include <QStringList>
#include <QTextStream>

#include "GSGridReader.h"
#include "GSPublicDefine.h"

GSGridReader::GSGridReader(QString fileName)
{
    m_sFileName = fileName;
    clearData();
}

GSGridReader::~GSGridReader()
{

}

int GSGridReader::readFile()
{
    int iErrorFileNotExists     = -1;
    int iErrorFileCanNotOpen    = -2;
    int iErrorDataIsInvalid     = -3;

    QString fullFileName = g_sDataPath + m_sFileName;
    if (QFile::exists(fullFileName) == false)
    {
        return iErrorFileNotExists;
    }
    QFile f(fullFileName);
    if (f.open(QFile::ReadOnly) == false)
    {
        f.close();
        return iErrorFileCanNotOpen;
    }
    QTextStream ts(&f);

    QString strType;
    while (!ts.atEnd())
    {
        QString line = ts.readLine();
        QStringList stl = line.split(" ", QString::SkipEmptyParts);
        int stlSize = stl.size();
        QString sBegin;
        QString sEnd;
        if (stlSize)
        {
            sBegin = stl.at(0);
            sEnd = stl.at(stlSize - 1);
        }
        if (sBegin == "/")
        {
            strType.clear();
        }
        else if (strType == "COORD")
        {
            if (stlSize / 3 * 3 != stlSize)
            {
                continue;
            }
            else
            {
                int oriSize = m_vallCOORD.size();
                m_vallCOORD.resize(oriSize + stlSize / 3);
                for (int i = 0; i < stlSize / 3; i++)
                {
                    QString strX = stl[i * 3 + 0];
                    QString strY = stl[i * 3 + 1];
                    QString strZ = stl[i * 3 + 2];
                    m_vallCOORD[oriSize + i] = SbVec3f(strX.toFloat(), strY.toFloat(), strZ.toFloat());
                }
            }
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else if (strType == "ZCORN")
        {
            int oriSize = m_vallZCORN.size();
            m_vallZCORN.resize(oriSize + stlSize);
            for (int i = 0; i < stlSize; i++)
            {
                QString strZ = stl[i];
                m_vallZCORN[oriSize + i] = strZ.toFloat();
            }
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else if (strType == "PORO")
        {
            int oriSize = m_vallPORO.size();
            m_vallPORO.resize(oriSize + stlSize);
            for (int i = 0; i < stlSize; i++)
            {
                QString strP = stl[i];
                m_vallPORO[oriSize + i] = strP.toFloat();
            }
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else if (strType == "PERM")
        {
            int oriSize = m_vallPERM.size();
            m_vallPERM.resize(oriSize + stlSize);
            for (int i = 0; i < stlSize; i++)
            {
                QString strP = stl[i];
                m_vallPERM[oriSize + i] = strP.toFloat();
            }
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else if (strType == "ACTNUM")
        {
            int oriSize = m_vallACTNUM.size();
            m_vallACTNUM.resize(oriSize + stlSize);
            for (int i = 0; i < stlSize; i++)
            {
                QString str = stl[i];
                m_vallACTNUM[oriSize + i] = (str.toInt() == 1);
            }
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else if (strType == "SPECGRID")
        {
            QString s0 = stl.at(0);
            m_i = s0.toInt();
            QString s1 = stl.at(1);
            m_j = s1.toInt();
            QString s2 = stl.at(2);
            m_k = s2.toInt();
            /// TODO : 后边的都是什么意思？
            if (sEnd.endsWith("/"))
            {
                strType.clear();
            }
        }
        else
        {
            if (line.startsWith("COORD"))
            {
                strType = "COORD";
                continue;
            }
            else if (line.startsWith("ZCORN"))
            {
                strType = "ZCORN";
                continue;
            }
            else if (line.startsWith("PORO"))
            {
                strType = "PORO";
                continue;
            }
            else if (line.startsWith("PERM"))
            {
                strType = "PERM";
                continue;
            }
            else if (line.startsWith("ACTNUM"))
            {
                strType = "ACTNUM";
                continue;
            }
            else if (line.startsWith("SPECGRID"))
            {
                strType = "SPECGRID";
                continue;
            }
            else if (line.startsWith("/"))
            {
                strType.clear();
                continue;
            }
            else if (sEnd.endsWith("/"))
            {
                strType.clear();
                continue;
            }

            else
            {
                strType.clear();
                {
                    /// TODO :
                }
            }
        }
    }

    if (checkData() == false)
    {
        f.close();
        clearData();
        return iErrorDataIsInvalid;
    }

    return 0;
}

void GSGridReader::getIJKCount(int &i, int &j, int &k)
{
    i = m_i;
    j = m_j;
    k = m_k;
}

QVector<SbVec3f> GSGridReader::getCOORD()
{
    return m_vallCOORD;
}

QVector<float> GSGridReader::getZCORN()
{
    return m_vallZCORN;
}

QVector<float> GSGridReader::getPORO()
{
    return m_vallPORO;
}

QVector<float> GSGridReader::getPERM()
{
    return m_vallPERM;
}

QVector<bool> GSGridReader::getACTNUM()
{
    return m_vallACTNUM;
}

void GSGridReader::clearData()
{
    m_i = -1;
    m_j = -1;
    m_k = -1;
    m_vallCOORD.clear();
    m_vallZCORN.clear();
    m_vallPORO.clear();
    m_vallPERM.clear();
    m_vallACTNUM.clear();
}

bool GSGridReader::checkData()
{
    if (m_i <= 0 || m_j <= 0 || m_k <= 0)
    {
        return false;
    }

    int cellCount = m_i * m_j * m_k;

    if (m_vallCOORD.size() != (m_i + 1) * (m_j + 1) * 2)
    {
        return false;
    }

    if (m_vallZCORN.size() != 8 * cellCount)
    {
        return false;
    }

    if (m_vallPORO.size() != cellCount)
    {
        return false;
    }

    if (m_vallPERM.size() != cellCount)
    {
        return false;
    }

    if (m_vallACTNUM.size() != cellCount)
    {
        return false;
    }

    return true;
}
