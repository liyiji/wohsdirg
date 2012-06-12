/// File Name : GSPublicFunctions.cpp

#include <QFile>
#include <QDebug>
#include <QString>
#include <QProcess>
#include <QTextStream>
#include <QMessageBox>

#include <Inventor/Qt/SoQt.h>
#include <MeshViz/PoMeshViz.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>

#include "GSPublicFunctions.h"
#include "GSPublicDefine.h"

bool equal(double f1, double f2)
{
    if (fabs(f1 - f2) <= 0.001)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void swapDouble(double &d1, double &d2)
{
    double tmp = d1;
    d1 = d2;
    d2 = tmp;
}

void debug(SbVec3f vec, QString str)
{
    printf("%s ", str.toLocal8Bit().constData());
    printf("%15f %15f %15f\n", vec[0], vec[1], vec[2]);
    fflush(0);
}

void initOIVParameters(QWidget *pWidget)
{
    SoQt::init(pWidget);
    PoMeshViz::init();
    SoVolumeRendering::init();

#ifndef CODING
    SoDB::setIvTuneAllowed(FALSE); /// 禁止打开IvTune，如果允许打开的话，快捷键是shift+F12
#endif
}

int checkOIVLicense()
{
    int iLicenseIsOk                    = 0;
    int iLicenseDoNotExists             = -1;
    int iFloatLicenseFileContentError   = -2;
    int iLicenseCanNotOpen              = -3;
    int iHostsCanNotOpen                = -4;
    int iCanNotFindItemInHosts          = -5;
    int iSolidLicenseError              = -6;
    int iLicenseServerIsNotStarted      = -7;
    int iUnknowLicenseError             = -8;

    /// 判断许可证文件是否存在，是否有效
    QString str("OIVHOME");
    char* oivhomePATH = getenv(str.toLocal8Bit().constData());
    QString strOivhome = QString(oivhomePATH);
    strOivhome.append("/license/password.dat");
    QFile licenseFile(strOivhome);
#ifdef CODING
    printf("LicenseFile is %s\n", strOivhome.toLocal8Bit().constData());
    BP;
#endif

    if (licenseFile.exists() == false)
    {
        return iLicenseDoNotExists;
        /// 许可证文件不存在
    }

    int iLicenseType = -1;
    QString strLine1, strLine2;
    if (licenseFile.open(QFile::ReadOnly) == false)
    {
        return iLicenseCanNotOpen;
        /// 许可的license file无法打开
    }

    QTextStream ts(&licenseFile);
    for (;;)
    {
        if (ts.atEnd())
        {
            break;
        }
        strLine1 = ts.readLine();
        if (strLine1.contains(QString("SERVER")) && strLine1.contains(QString("ANY")))
        {
            strLine2 = ts.readLine();
            iLicenseType = 1; /// 浮动许可
            break;
        }
    }
    licenseFile.close();
    if (iLicenseType == -1)
    {
        /// 不是浮动许可
        /// 判断是否是固定许可，并进行检查
        licenseFile.open(QFile::ReadOnly);
        bool bOE, bME, bVE; /// 分别代表，OpenInventor/MeshViz/VolumeViz的许可证存在
        bOE = bME = bVE = false;
        QTextStream ts2(&licenseFile);
        for (;;)
        {
            if (ts2.atEnd())
            {
                break;
            }
            QString strLine4 = ts2.readLine();
            {
                for (int i = 0; i < strLine4.count(); i++)
                {
                    if (strLine4.at(i) == QChar('#'))
                    {
                        strLine4 = strLine4.left(i + 1);
                        break;
                    }
                }
            }
            if (strLine4.contains(QString("OpenInventor")))
            {
                bOE = true;
            }
            if (strLine4.contains(QString("MeshViz")))
            {
                bME = true;
            }
            if (strLine4.contains(QString("VolumeViz")))
            {
                bVE = true;
            }
        }
        licenseFile.close();
        if (bOE && bME && bVE)
        {
            return iLicenseIsOk;
            /// 固定许可的许可证正确
        }
        else
        {
            return iSolidLicenseError;
            /// 固定许可的许可证错误
        }
    }
    else
    {
        if (strLine2.contains("USE_SERVER") == false )
        {
            return iFloatLicenseFileContentError;
            /// 浮动许可的 license file 内容错误
        }

        QStringList strList = strLine1.split(" ", QString::SkipEmptyParts);
        for (int i = 0; i < strList.count(); i++)
        {
            QString strTmp = strList.at(i);
            if (strTmp == "SERVER" || strTmp == "ANY")
            {
                strList.removeAt(i);
            }
        }
        QString strServerName = strList.at(0);
        bool bIsIP = false;
        {
            QStringList strList4 = strServerName.split(".");
            if (strList4.count() == 4)
                bIsIP = true;
        }
        bool bHostNameIsInHosts = false;
        if (bIsIP == false)
        {
            QFile fileHosts("/etc/hosts");
            if (fileHosts.open(QFile::ReadOnly) == false)
            {
                return iHostsCanNotOpen;
                /// hosts文件打不开
            }

            QTextStream ts2(&fileHosts);
            QString strLine3;
            for (;;)
            {
                if (ts2.atEnd())
                {
                    break;
                }
                strLine3 = ts2.readLine();
                /// 处理strLine3
                for (int i = 0; i < strLine3.count(); i++)
                {
                    if (strLine3.at(i) == QChar('#'))
                    {
                        strLine3 = strLine3.left(i + 1);
                        break;
                    }
                }

                QStringList strList1 = strLine3.split(" ", QString::SkipEmptyParts);
                for (int i = 0; i < strList1.count(); i++)
                {
                    QString strTemp = strList1.at(i);
                    QStringList strList3 = strTemp.split("\t", QString::SkipEmptyParts);
                    for (int j = 0; j < strList3.count(); j++)
                    {
                        if (strList3.contains(strServerName))
                        {
                            bHostNameIsInHosts = true;
                            break;
                        }
                    }
                    if (bHostNameIsInHosts)
                        break;
                }
                if (bHostNameIsInHosts)
                    break;
            }
            fileHosts.close();
        }
        if (bIsIP || bHostNameIsInHosts)
        {
            bool serverStarted = false;

            /// 连接到 server 上检查服务是否开启
            QProcess pro27000;
            QString strProgram27000("nmap -p 27000 ");
            strProgram27000.append(strServerName);
            pro27000.start(strProgram27000);
            QString outputInfo27000;
            if (pro27000.waitForFinished())
            {
                outputInfo27000.append(pro27000.readAll());
            }

            QStringList strList2 = outputInfo27000.split("\n", QString::SkipEmptyParts);
            for (int i = 0; i < strList2.count(); i++)
            {
                QString curStr = strList2.at(i);
                QStringList strL = curStr.split(" ", QString::SkipEmptyParts);
                if (strL.contains("open"))
                {
                    for (int j = 0; j < strL.count(); j++)
                    {
                        QString strTemp = strL.at(j);
                        if (strTemp.contains("flexlm"))
                        {
                            serverStarted = true;
                            break;
                        }
                    }
                    if (serverStarted)
                        break;
                }
            }

            if (serverStarted)
            {
                return iLicenseIsOk;
                /// 许可证正确
            }
            else
            {
                QProcess pro;
                QString strProgram("nmap ");
                strProgram.append(strServerName);
                pro.start(strProgram);
                QString outputInfo;
                if (pro.waitForFinished())
                {
                    outputInfo.append(pro.readAll());
                }

                QStringList strList5 = outputInfo.split("\n", QString::SkipEmptyParts);
                for (int i = 0; i < strList5.count(); i++)
                {
                    QString curStr = strList5.at(i);
                    QStringList strL = curStr.split(" ", QString::SkipEmptyParts);
                    if (strL.contains("open"))
                    {
                        for (int j = 0; j < strL.count(); j++)
                        {
                            QString strTemp = strL.at(j);
                            if (strTemp.contains("flexlm"))
                            {
                                serverStarted = true;
                                break;
                            }
                        }
                        if (serverStarted)
                            break;
                    }
                }

                if (serverStarted)
                {
                    return iLicenseIsOk;
                    /// 许可证正确
                }
                else
                {
                    return iLicenseServerIsNotStarted;
                }
            }
        }
        else
        {
            return iCanNotFindItemInHosts;
            /// 在Hosts文件里没有找到相关记录
        }
    }

    return iUnknowLicenseError;
}



bool outputCheckOIVLicenseResult()
{
    int checkOIVLicenseErrorType = checkOIVLicense();

    if (checkOIVLicenseErrorType == 0)
    {
        /// license 正确
        return true;
    }
    else
    {
        QString errMsg;

        switch(checkOIVLicenseErrorType)
        {
        case -1: {
                errMsg = "Can not find license file.\nPlease check whether license file exists.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -2: {
                errMsg = "License file format is wrong.\nPlease rebuild license file.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -3: {
                errMsg = "License file can not open.\nPlease check the limits of authority of license file.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -4: {
#ifdef WIN32
                errMsg = "Hosts file can not open.\nPlease check whether \"C:\\windows\\system32\\drivers\\etc\\hosts\" exists.";
#else
                errMsg = "Hosts file can not open.\nPlease check whether \"/etc/hosts\" exists.";
#endif
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -5: {
                errMsg = "Can not find item of server computer in hosts file.\nPlease add corresponding item for server computer in hosts file.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -6: {
                errMsg = "License is invalid.\nPlease change another license file.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        case -7: {
                errMsg = "License server program on server may not start.\nPlease check whether the license server program is running.";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        default: {
                errMsg = "Unknown License Error!\nProgram will quit!";
                QMessageBox::critical(0, "License error!", errMsg);
                break;
            }
        }
        return false;
    }
}



void outputOIVversionInfo()
{
#ifdef USE_OIV800_IN_ADES
    printf("Open Inventor version is 8.0\n");
#endif
#ifdef USE_OIV813_IN_ADES
    printf("Open Inventor version is 8.1.3\n");
#endif
#ifdef USE_OIV850_IN_ADES
    printf("Open Inventor version is 8.5\n");
#endif
#ifdef USE_OIV860_IN_ADES
    printf("Open Inventor version is 8.6\n");
#endif
#ifdef USE_OIV900_IN_ADES
    printf("Open Inventor version is 9.0\n");
#endif
    BP;
}

SbColor getSbColorFromQColor(QColor c)
{
    SbColor sc(c.redF(), c.greenF(), c.blueF());
    return sc;
}

QColor getQColorFromSbColor(SbColor sc)
{
    QColor c(int(sc[0] * 255.0), int(sc[1] * 255.0), int(sc[2] * 255.0));
    return c;
}

int getColorIndexFromList(double min, double max, int size, double value)
{
    if (min >= max)
    {
        return 0;
    }
    if (size <= 0)
    {
        return 0;
    }
    if (value <= min)
    {
        return 0;
    }
    if (value >= max)
    {
        return size - 1;
    }

    int index = int((value - min) / (max - min) * size);

    return index;
}

int getCoordByLineAndZ(SbVec3f p1, SbVec3f p2, double z, SbVec3f &coord)
{
    if (equal(p1[2], p2[2]))
    {
        return -1;
    }
    if (equal(p1[2], z))
    {
        coord = p1;
        return 0;
    }
    else if (equal(p2[2], z))
    {
        coord = p2;
        return 0;
    }

    float x, y;
    x = (z - p1[2]) / (p2[2] - p1[2]) * (p2[0] - p1[0]) + p1[0];
    y = (z - p1[2]) / (p2[2] - p1[2]) * (p2[1] - p1[1]) + p1[1];
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;
    return 0;
}


/**

*/
