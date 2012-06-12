/// File Name : GS3dGrid.h

#ifndef GS3DGRID_H
#define GS3DGRID_H

#include "GS3DObject.h"
#include "GSPoint.h"

class SoMaterial;
class SoDrawStyle;
class SoPickStyle;
class SoShapeHints;
class SoVertexProperty;
class SoIndexedFaceSet;

class GS3DGrid : public GS3DObject
{
public:
    GS3DGrid(QString uniqueName,
             GS3DObjectParameters para);

    virtual ~GS3DGrid();

    virtual SoSeparator *getOIVNode();
    virtual int refreshObject();
    virtual void changeColorMap(QList<QColor> cm);

public:
    void setShowPOROorPERM(QString type);
    int getICount();
    int getJCount();
    int getKCount();

    void getPOROandPERMbyCoord(SbVec3f coord, float &fPoro, float &fPerm);

protected:
    int readDataFromFile(QString fileName);
    void clearData();
    void compressData(QVector<int> &allIndex, QVector<SbVec3f> &allPoints);

protected:
    float m_fMinPORO, m_fMaxPORO;
    float m_fMinPERM, m_fMaxPERM;
    QVector<double> m_vecFacetPORO;
    QVector<double> m_vecFacetPERM;
    int m_nFacetCount;

    SoDrawStyle         *m_pDrawStyle;
    SoPickStyle         *m_pPickStyle;
    SoShapeHints        *m_pShapeHints;
    SoSwitch            *m_pMatSwitch;
    SoMaterial          *m_pMaterialPORO;
    SoMaterial          *m_pMaterialPERM;
    SoVertexProperty    *m_pVertex;
    SoIndexedFaceSet    *m_pFaceSet;

    int m_i, m_j, m_k;
    QVector<GSPoint> m_vecPoints;
    QVector<float> m_vecPORO;
    QVector<float> m_vecPERM;
    QVector<bool> m_vecACTNUM;
};

#endif // GS3DGRID_H
