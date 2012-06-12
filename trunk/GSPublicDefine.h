/// File Name : GSPublicDefine.h

#ifndef GSPUBLICDEFINE_H
#define GSPUBLICDEFINE_H

#include <QDebug>
#include <Inventor/SbVec.h>

#define BUSY_CURSOR     QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESTORE_CURSOR  QApplication::restoreOverrideCursor()
#define BP              fflush(0)

const double  g_fHistogramThreshold = 0.005;

const float   g_fInvalidValue       = -999999.0;
const int     g_iInvalidValue       = 999999;
const SbVec3f g_invalidCoord        = SbVec3f(g_fInvalidValue, g_fInvalidValue, g_fInvalidValue);
const QString g_sUniqueNameSpliter  = QString("@");
const QString g_sLibPath            = QString("../lib/");
const QString g_sResoucePath        = QString("../resource/");
const QString g_sDataPath           = QString("../data/"); /// TODO :

enum GSObjectType
{
    GridType,
    GridGroupType
};

class GS3DObjectParameters
{
public:
    float xTrans, yTrans, zTrans;
    float xScale, yScale, zScale;
};

#endif // GSPUBLICDEFINE_H
