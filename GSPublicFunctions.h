/// File Name : GSPublicFunctions.h

#ifndef GSPUBLICFUNCTIONS_H
#define GSPUBLICFUNCTIONS_H

#include <QColor>
#include <Inventor/SbColor.h>

bool equal(double f1, double f2);

void swapDouble(double &d1, double &d2);

void debug(SbVec3f vec, QString str = "Coord is:");

void initOIVParameters(QWidget *pWidget);
int checkOIVLicense();
bool outputCheckOIVLicenseResult();
void outputOIVversionInfo();

/// SbColor和QColor的互相转换
SbColor getSbColorFromQColor(QColor c);
QColor getQColorFromSbColor(SbColor sc);

/// 根据最小值最大值取得第i个index
int getColorIndexFromList(double min, double max, int size, double value);

/// 根据一根直线和一个z值，确定坐标
int getCoordByLineAndZ(SbVec3f p1, SbVec3f p2, double z, SbVec3f &coord);

#endif // GSPUBLICFUNCTIONS_H
