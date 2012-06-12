/// File Name : GS3dGrid.cpp

#include <QFile>
#include <QMessageBox>

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>

#include "GS3DGrid.h"
#include "GSGridReader.h"
#include "GSPublicDefine.h"
#include "GSPublicFunctions.h"

GS3DGrid::GS3DGrid(QString uniqueName, GS3DObjectParameters para)
    : GS3DObject(GridType, uniqueName, para)
{
    m_pDrawStyle    = 0;
    m_pPickStyle    = 0;
    m_pShapeHints   = 0;
    m_pMatSwitch    = 0;
    m_pMaterialPORO = 0;
    m_pMaterialPERM = 0;
    m_pVertex       = 0;
    m_pFaceSet      = 0;

    clearData();
}

GS3DGrid::~GS3DGrid()
{

}

SoSeparator *GS3DGrid::getOIVNode()
{
    if (m_pSep)
    {
        return m_pSep;
    }

    m_pSep = new SoSeparator;
    m_pSep->ref();

    m_pSw = new SoSwitch;
    m_pSep->addChild(m_pSw);

    m_pDrawStyle = new SoDrawStyle;

    m_pPickStyle = new SoPickStyle;
    m_pPickStyle->style = SoPickStyle::BOUNDING_BOX;

    m_pShapeHints = new SoShapeHints; // 面优化
    m_pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE; // 双边光照
    m_pShapeHints->creaseAngle.setValue(M_PI);

    m_pMaterialPORO = new SoMaterial;
    m_pMaterialPERM = new SoMaterial;
    m_pMaterialPORO->diffuseColor.setValue(getSbColorFromQColor(Qt::red));
    m_pMaterialPERM->diffuseColor.setValue(getSbColorFromQColor(Qt::yellow));
    m_pMatSwitch = new SoSwitch;
    m_pMatSwitch->addChild(m_pMaterialPORO);
    m_pMatSwitch->addChild(m_pMaterialPERM);
    setShowPOROorPERM("PORO");
    SoMaterialBinding *pMaterialBinding = new SoMaterialBinding;
    pMaterialBinding->value.setValue(SoMaterialBinding::PER_FACE);
    m_pVertex = new SoVertexProperty;
    m_pFaceSet = new SoIndexedFaceSet;
    m_pFaceSet->vertexProperty.setValue(m_pVertex);

    m_pSw->addChild(m_pDrawStyle);
    m_pSw->addChild(m_pPickStyle);
    m_pSw->addChild(m_pShapeHints);
    m_pSw->addChild(pMaterialBinding);
    m_pSw->addChild(m_pMatSwitch);
    m_pSw->addChild(m_pFaceSet);
    m_pSw->whichChild.setValue(SO_SWITCH_ALL);

    return m_pSep;
}

int GS3DGrid::refreshObject()
{
    int readDataError = -1;

    if (readDataFromFile(m_sUniqueNameList.at(0)) != 0)
    {
        return readDataError;
    }

    int nip = m_i + 1;
    int njp = m_j + 1;
    int nkp = m_k + 1;
    int nijk = m_i * m_j * m_k;
    int nijkp = nip * njp * nkp;
    int pointCount = nijkp * 8;

    if (m_vecPoints.size() != nijkp || m_vecPORO.size() != nijk || m_vecPERM.size() != nijk || m_vecACTNUM.size() != nijk)
    {
        /// TODO : error
        return readDataError;
    }

    QVector<int> allIndex; /// index of all coords
    QVector<SbVec3f> allPoints;

    {
        /// 由GSPoint构建SbVec3f数组
        allPoints.resize(pointCount);
        for (int i = 0; i < nijkp; i++)
        {
            GSPoint pt = m_vecPoints.at(i);
            allPoints[i * 8 + 0] = pt.getPointCoord(false, false, false);
            allPoints[i * 8 + 1] = pt.getPointCoord(true, false, false);
            allPoints[i * 8 + 2] = pt.getPointCoord(false, true, false);
            allPoints[i * 8 + 3] = pt.getPointCoord(true, true, false);
            allPoints[i * 8 + 4] = pt.getPointCoord(false, false, true);
            allPoints[i * 8 + 5] = pt.getPointCoord(true, false, true);
            allPoints[i * 8 + 6] = pt.getPointCoord(false, true, true);
            allPoints[i * 8 + 7] = pt.getPointCoord(true, true, true);
        }

        for (int i = 0; i < allPoints.size(); i++)
        {
            SbVec3f &vec = allPoints[i];
            vec[0] += m_parameters.xTrans;
            vec[1] += m_parameters.yTrans;
            vec[2] += m_parameters.zTrans;
            vec[0] *= m_parameters.xScale;
            vec[1] *= m_parameters.yScale;
            vec[2] *= m_parameters.zScale;
        }

    }

    for (int k = 0; k < m_k; k++)
    {
        for (int j = 0; j < m_j; j++)
        {
            for (int i = 0; i < m_i; i++)
            {
                /// TODO :
                int index[8];
                index[0] = k * njp * nip + j * nip + i;
                index[1] = k * njp * nip + j * nip + (i + 1);
                index[2] = k * njp * nip + (j + 1) * nip + i;
                index[3] = k * njp * nip + (j + 1) * nip + (i + 1);
                index[4] = (k + 1) * njp * nip + j * nip + i;
                index[5] = (k + 1) * njp * nip + j * nip + (i + 1);
                index[6] = (k + 1) * njp * nip + (j + 1) * nip + i;
                index[7] = (k + 1) * njp * nip + (j + 1) * nip + (i + 1);

                QVector<GSPoint> pts;

                for (int t = 0; t < 8; t++)
                {
                    pts.append(m_vecPoints[index[t]]);
                }

                /// i 0264 1375
                if (i == 0)
                {
                    if (j != 0 || k  != 0)
                    {
                        qDebug() << i << j << k;
                        qDebug() << getIndexFrom3Bool(true, true, true);
                        debug(pts[0].getPointCoord(true, true, true));
                        qDebug() << getIndexFrom3Bool(true, false, true);
                        debug(pts[2].getPointCoord(true, false, true));
                        qDebug() << getIndexFrom3Bool(true, false, false);
                        debug(pts[6].getPointCoord(true, false, false));
                        qDebug() << getIndexFrom3Bool(true, true, false);
                        debug(pts[4].getPointCoord(true, true, false));
                        BP;
                    }

                    if (pts[0].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[2].getPointCoord(true, false, true) != g_invalidCoord &&
                        pts[6].getPointCoord(true, false, false) != g_invalidCoord &&
                        pts[4].getPointCoord(true, true, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[0] * 8 + pts[0].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[2] * 8 + pts[2].getPointIndex(true, false, true);
                        allIndex[coordNum + 2] = index[6] * 8 + pts[6].getPointIndex(true, false, false);
                        allIndex[coordNum + 3] = index[4] * 8 + pts[4].getPointIndex(true, true, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        debug(allPoints[allIndex[coordNum + 0]]);
                        debug(allPoints[allIndex[coordNum + 1]]);
                        debug(allPoints[allIndex[coordNum + 2]]);
                        debug(allPoints[allIndex[coordNum + 3]]);

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                    else
                    {
                        BP;
                    }
                }

                /// all i
                if (pts[1].getPointCoord(true, true, true) == pts[1].getPointCoord(false, true, true) &&
                    pts[3].getPointCoord(true, false, true) == pts[3].getPointCoord(false, false, true) &&
                    pts[7].getPointCoord(true, false, false) == pts[7].getPointCoord(false, false, false) &&
                    pts[5].getPointCoord(true, true, false) == pts[5].getPointCoord(false, true, false))
                {
                    /// 重叠，不创建面
                }
                else
                {
                    if (pts[1].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[3].getPointCoord(true, false, true) != g_invalidCoord &&
                        pts[7].getPointCoord(true, false, false) != g_invalidCoord &&
                        pts[5].getPointCoord(true, true, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[1] * 8 + pts[1].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[3] * 8 + pts[3].getPointIndex(true, false, true);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(true, false, false);
                        allIndex[coordNum + 3] = index[5] * 8 + pts[5].getPointIndex(true, true, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i + 1]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i + 1]);
                    }
                    if (pts[1].getPointCoord(false, true, true) != g_invalidCoord &&
                        pts[3].getPointCoord(false, false, true) != g_invalidCoord &&
                        pts[7].getPointCoord(false, false, false) != g_invalidCoord &&
                        pts[5].getPointCoord(false, true, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[1] * 8 + pts[1].getPointIndex(false, true, true);
                        allIndex[coordNum + 1] = index[3] * 8 + pts[3].getPointIndex(false, false, true);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(false, false, false);
                        allIndex[coordNum + 3] = index[5] * 8 + pts[5].getPointIndex(false, true, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                }
                /// all i done

                /// j 0154 2376
                if (j == 0)
                {
                    if (pts[0].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[1].getPointCoord(false, true, true) != g_invalidCoord &&
                        pts[5].getPointCoord(false, true, false) != g_invalidCoord &&
                        pts[4].getPointCoord(true, true, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[0] * 8 + pts[0].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[1] * 8 + pts[1].getPointIndex(false, true, true);
                        allIndex[coordNum + 2] = index[5] * 8 + pts[5].getPointIndex(false, true, false);
                        allIndex[coordNum + 3] = index[4] * 8 + pts[4].getPointIndex(true, true, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                    else
                    {
                        BP;
                    }
                }

                /// all j
                if (pts[2].getPointCoord(true, true, true) == pts[2].getPointCoord(true, false, true) &&
                    pts[3].getPointCoord(false, true, true) == pts[3].getPointCoord(false, false, true) &&
                    pts[7].getPointCoord(false, true, false) == pts[7].getPointCoord(false, false, false) &&
                    pts[6].getPointCoord(true, true, false) == pts[6].getPointCoord(true, false, false))
                {
                    /// 重叠，不创建面
                }
                else
                {
                    if (pts[2].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[3].getPointCoord(false, true, true) != g_invalidCoord &&
                        pts[7].getPointCoord(false, true, false) != g_invalidCoord &&
                        pts[6].getPointCoord(true, true, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[2] * 8 + pts[2].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[3] * 8 + pts[3].getPointIndex(false, true, true);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(false, true, false);
                        allIndex[coordNum + 3] = index[6] * 8 + pts[6].getPointIndex(true, true, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + (j + 1) * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + (j + 1) * m_i + i]);
                    }
                    if (pts[2].getPointCoord(true, false, true) != g_invalidCoord &&
                        pts[3].getPointCoord(false, false, true) != g_invalidCoord &&
                        pts[7].getPointCoord(false, false, false) != g_invalidCoord &&
                        pts[6].getPointCoord(true, false, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[2] * 8 + pts[2].getPointIndex(true, false, true);
                        allIndex[coordNum + 1] = index[3] * 8 + pts[3].getPointIndex(false, false, true);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(false, false, false);
                        allIndex[coordNum + 3] = index[6] * 8 + pts[6].getPointIndex(true, false, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                }
                /// all j done

                /// k 0132 4576
                if (k == 0)
                {
                    if (pts[0].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[1].getPointCoord(false, true, true) != g_invalidCoord &&
                        pts[3].getPointCoord(false, false, true) != g_invalidCoord &&
                        pts[2].getPointCoord(true, false, true) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[0] * 8 + pts[0].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[1] * 8 + pts[1].getPointIndex(false, true, true);
                        allIndex[coordNum + 2] = index[3] * 8 + pts[3].getPointIndex(false, false, true);
                        allIndex[coordNum + 3] = index[2] * 8 + pts[2].getPointIndex(true, false, true);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                    else
                    {
                        BP;
                    }
                }

                /// all k
                if (pts[4].getPointCoord(true, true, true) == pts[4].getPointCoord(true, true, false) &&
                    pts[5].getPointCoord(false, true, true) == pts[5].getPointCoord(false, true, false) &&
                    pts[7].getPointCoord(false, false, true) == pts[7].getPointCoord(false, false, false) &&
                    pts[6].getPointCoord(true, false, true) == pts[6].getPointCoord(true, false, false))
                {
                    /// 重叠，不创建面
                }
                else
                {
                    if (pts[4].getPointCoord(true, true, true) != g_invalidCoord &&
                        pts[5].getPointCoord(false, true, true) != g_invalidCoord &&
                        pts[7].getPointCoord(false, false, true) != g_invalidCoord &&
                        pts[6].getPointCoord(true, false, true) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[4] * 8 + pts[4].getPointIndex(true, true, true);
                        allIndex[coordNum + 1] = index[5] * 8 + pts[5].getPointIndex(false, true, true);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(false, false, true);
                        allIndex[coordNum + 3] = index[6] * 8 + pts[6].getPointIndex(true, false, true);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[(k + 1) * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[(k + 1) * m_j * m_i + j * m_i + i]);
                    }
                    if (pts[4].getPointCoord(true, true, false) != g_invalidCoord &&
                        pts[5].getPointCoord(false, true, false) != g_invalidCoord &&
                        pts[7].getPointCoord(false, false, false) != g_invalidCoord &&
                        pts[6].getPointCoord(true, false, false) != g_invalidCoord)
                    {
                        int coordNum = allIndex.size();
                        allIndex.resize(coordNum + 5);
                        allIndex[coordNum + 0] = index[4] * 8 + pts[4].getPointIndex(true, true, false);
                        allIndex[coordNum + 1] = index[5] * 8 + pts[5].getPointIndex(false, true, false);
                        allIndex[coordNum + 2] = index[7] * 8 + pts[7].getPointIndex(false, false, false);
                        allIndex[coordNum + 3] = index[6] * 8 + pts[6].getPointIndex(true, false, false);
                        allIndex[coordNum + 4] = SO_END_POLYGON_INDEX;

                        m_nFacetCount++;
                        m_vecFacetPORO.append(m_vecPORO[k * m_j * m_i + j * m_i + i]);
                        m_vecFacetPERM.append(m_vecPERM[k * m_j * m_i + j * m_i + i]);
                    }
                }
                /// all k done
            }
        }
    }

    compressData(allIndex, allPoints);
    m_pFaceSet->coordIndex.setValues(0, allIndex.size(), allIndex.constData());
    m_pVertex->vertex.setValues(0, allPoints.size(), allPoints.constData());

    bool bDebugVertexAndIndex = false;
    if (bDebugVertexAndIndex)
    {
        QVector<bool> bInvalid;
        bInvalid.resize(allPoints.size());
        bInvalid.fill(true);
        int invalidCount = 0;
        for (int k = 0; k < nkp; k++)
        {
            for (int j = 0; j < njp; j++)
            {
                for (int i = 0; i < nip; i++)
                {
                    int index = k * njp * nip + j * nip + i;
                    if (i == 0)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 0 || l == 2 || l == 6 || l == 4)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                    if (i == nip - 1)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 1 || l == 3 || l == 7 || l == 5)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                    if (j == 0)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 0 || l == 1 || l == 5 || l == 4)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                    if (j == njp - 1)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 2 || l == 3 || l == 7 || l == 6)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                    if (k == 0)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 0 || l == 1 || l == 3 || l == 2)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                    if (k == nkp - 1)
                    {
                        for (int l = 0; l < 8; l++)
                        {
                            if (l == 4 || l == 5 || l == 7 || l == 6)
                            {
                                if (allPoints[index * 8 + l] != g_invalidCoord)
                                {
                                    qDebug() << "Error" << i << j << k << "Index is " << l;
                                }
                                else
                                {
                                    bInvalid[index * 8 + l] = false;
                                    invalidCount++;
                                }
                            }
                        }
                    }
                }
            }
        }
        int xxvalidCount = 0;
        int xxinvalidCount = 0;
        for (int i = 0; i < allPoints.size(); i++)
        {
            if (allPoints[i] != g_invalidCoord)
            {
                xxvalidCount++;
            }
            else
            {
                xxinvalidCount++;
            }
        }
        qDebug() << invalidCount;
        qDebug() << xxvalidCount << xxinvalidCount;
        for (int i = 0; i < bInvalid.size(); i++)
        {
            if (bInvalid[i])
            {
                if (allPoints[i] == g_invalidCoord)
                {
                    qDebug() << "Error" << i << "Have no value while should have.";
                }
            }
        }
        for (int i = 0; i < m_pVertex->vertex.getNum(); i++)
        {
            printf("vertex %d %d %d  ", i / 8, i % 8, i);
            debug(m_pVertex->vertex[i]);
            fflush(0);
            if (i % 8 == 7)
            {
                qDebug();
            }
        }
//        for (int i = 0; i < m_pFaceSet->coordIndex.getNum(); i += 5)
//        {
//            qDebug() << "coordIndex" << i / 5 << m_pFaceSet->coordIndex[i + 0] << m_pFaceSet->coordIndex[i + 1] << m_pFaceSet->coordIndex[i + 2] << m_pFaceSet->coordIndex[i + 3] << m_pFaceSet->coordIndex[i + 4];
//        }
    }

    return 0;
}

void GS3DGrid::changeColorMap(QList<QColor> cm)
{
    /// TODO :
    bool bDebugTestColorMap = true;

    if (bDebugTestColorMap)
    {
        cm.clear();
        cm.append(Qt::yellow);
        cm.append(Qt::green);
        cm.append(Qt::blue);
    }
    else
    {
        if (cm.size() == 0)
        {
            return;
        }
    }

    if (m_fColorMapMax > m_fMaxPORO)
    {
        m_fColorMapMax = m_fMaxPORO;
    }
    if (m_fColorMapMin < m_fMinPORO)
    {
        m_fColorMapMin = m_fMinPORO;
    }

    QVector<SbColor> diffColor;
//    qDebug() << m_nFacetCount;
    diffColor.resize(m_nFacetCount);

    for (int i = 0; i < m_nFacetCount; i++)
    {
        int colorIndex = getColorIndexFromList(m_fColorMapMin, m_fColorMapMax, cm.size(), m_vecFacetPORO.at(i));
//        qDebug() << colorIndex;
        diffColor[i] = getSbColorFromQColor(cm.at(colorIndex));
    }
    m_pMaterialPORO->diffuseColor.setValues(0, m_nFacetCount, diffColor.constData());
}

void GS3DGrid::setShowPOROorPERM(QString type)
{
    if (type.contains("PORO"))
    {
        m_pMatSwitch->whichChild.setValue(0);
    }
    else if (type.contains("PERM"))
    {
       m_pMatSwitch->whichChild.setValue(1);
    }
}

int GS3DGrid::getICount()
{
    return m_i;
}

int GS3DGrid::getJCount()
{
    return m_j;
}

int GS3DGrid::getKCount()
{
    return m_k;
}

void GS3DGrid::getPOROandPERMbyCoord(SbVec3f coord, float &fPoro, float &fPerm)
{
    /// TODO :
}

int GS3DGrid::readDataFromFile(QString fileName)
{
    clearData();

    bool bDebugTestData = false; /// TODO :
    if (bDebugTestData)
    {
        /// 使用测试数据

        int ni = 1;
        int nj = 1;
        int nk = 2;
        int nip = ni + 1;
        int njp = nj + 1;
        int nkp = nk + 1;

        GSPoint point[nip * njp * nkp];
        for (int i = 0; i < nip; i++)
        {
            for (int j = 0; j < njp; j++)
            {
                for (int k = 0; k < nkp; k++)
                {
                    int index = i * njp * nkp + j * nkp + k;
                    point[index] = GSPoint(i, j, k);
                    point[index].setPointCoord(255, SbVec3f(i * 500, j * 500, k * 500));
                    if (index == 1)
                    {
                        point[index].setPointCoord(128, SbVec3f(i * 500, j * 500, k * 500 + 300));
                    }
                    else if (index == 4)
                    {
                        point[index].setPointCoord(32, SbVec3f(i * 500, j * 500, k * 500 + 300));
                    }
                    else if (index == 7)
                    {
                        point[index].setPointCoord(8, SbVec3f(i * 500, j * 500, k * 500 + 300));
                    }
                }
            }
        }

        for (int i = 0; i < ni; i++)
        {
            for (int j = 0; j < nj; j++)
            {
                for (int k = 0; k < nk; k++)
                {
                    int index = i * nj * nk + j * nk + k;
                    m_vecPORO.append(index * 100);
                }
            }
        }

        bool bDebugCheckTestData = false;
        if (bDebugCheckTestData)
        {
            for (int it = 0; it < nip * njp * nkp; it++)
            {
                GSPoint pt = point[it];
                int i, j, k;
                pt.getIJK(i, j, k);
                qDebug() << i << j << k;
                SbVec3f vec1 = pt.getPointCoord(true, true, true);
                SbVec3f vec2 = pt.getPointCoord(true, true, false);
                SbVec3f vec3 = pt.getPointCoord(true, false, true);
                SbVec3f vec4 = pt.getPointCoord(true, false, false);
                SbVec3f vec5 = pt.getPointCoord(false, true, true);
                SbVec3f vec6 = pt.getPointCoord(false, true, false);
                SbVec3f vec7 = pt.getPointCoord(false, false, true);
                SbVec3f vec8 = pt.getPointCoord(false, false, false);

                qDebug() << vec1[0] << vec1[1] << vec1[2];
                qDebug() << vec2[0] << vec2[1] << vec2[2];
                qDebug() << vec3[0] << vec3[1] << vec3[2];
                qDebug() << vec4[0] << vec4[1] << vec4[2];
                qDebug() << vec5[0] << vec5[1] << vec5[2];
                qDebug() << vec6[0] << vec6[1] << vec6[2];
                qDebug() << vec7[0] << vec7[1] << vec7[2];
                qDebug() << vec8[0] << vec8[1] << vec8[2];
            }
        }

        for (int it = 0; it < nip * njp * nkp; it++)
        {
            m_vecPoints.append(point[it]);
        }
    }
    else
    {
        GSGridReader reader(fileName);
        if (reader.readFile() != 0)
        {
            return -1;
        }

        reader.getIJKCount(m_i, m_j, m_k);
        QVector<SbVec3f> verticalLines = reader.getCOORD();
        QVector<float> zValues = reader.getZCORN();
        qDebug() << zValues.at(0) << zValues.at(20) << zValues.at(400) << zValues.at(420);
        m_vecPORO = reader.getPORO();
        m_vecPERM = reader.getPERM();
        m_vecACTNUM = reader.getACTNUM();

        bool bDebugCheckOriginData = false;
        if (bDebugCheckOriginData)
        {
//            qDebug() << "-----------verticalLines-----------" << verticalLines.size();
//            for (int i = 0; i < verticalLines.size() / 2; i++)
//            {
//                printf("%15f %15f %15f, %15f %15f %15f\n",
//                       verticalLines[2 * i + 0][0],
//                       verticalLines[2 * i + 0][1],
//                       verticalLines[2 * i + 0][2],
//                       verticalLines[2 * i + 1][0],
//                       verticalLines[2 * i + 1][1],
//                       verticalLines[2 * i + 1][2]);
//            }

//            qDebug() << "-----------zValues-----------" << zValues.size();
//            for (int i = 0; i < zValues.size() / 6 + 1; i++)
//            {
//                for (int j = 0; j < 6; j++)
//                {
//                    int v1 = 6 * i + j;
//                    int v2 = zValues.size();
//                    if (v1 < v2)
//                    {
//                        printf("%f ", zValues[6 * i + j]);
//                    }
//                }
//                printf("\n");
//            }
            fflush(0);
        }

        int nip = m_i + 1;
        int njp = m_j + 1;
        int nkp = m_k + 1;
        GSPoint point[nip * njp * nkp];
        bool pointCreated[nip * njp * nkp];
        for (int i = 0; i < nip * njp * nkp; i++)
        {
            pointCreated[i] = false;
        }

        for (int k = 0; k < m_k; k++)
        {
            for (int j = 0; j < m_j; j++)
            {
                for (int i = 0; i < m_i; i++)
                {
                    int index = k * m_j * m_i + j * m_i + i;
                    if (m_vecACTNUM[index] == false)
                    {
                        continue;
                    }

                    int indexP[8]; /// 角点的索引
                    indexP[0] = k * njp * nip + j * nip + i;
                    indexP[1] = k * njp * nip + j * nip + (i + 1);
                    indexP[2] = k * njp * nip + (j + 1) * nip + i;
                    indexP[3] = k * njp * nip + (j + 1) * nip + (i + 1);
                    indexP[4] = (k + 1) * njp * nip + j * nip + i;
                    indexP[5] = (k + 1) * njp * nip + j * nip + (i + 1);
                    indexP[6] = (k + 1) * njp * nip + (j + 1) * nip + i;
                    indexP[7] = (k + 1) * njp * nip + (j + 1) * nip + (i + 1);
//                    printf("P ");
//                    for (int l = 0; l < 8; l++)
//                    {
//                        printf("%d ", indexP[l]);
//                    }
//                    printf("\n");
//                    fflush(0);

                    int indexL[8]; /// 竖线的索引
                    indexL[0] = j * nip + i;
                    indexL[1] = j * nip + i + 1;
                    indexL[2] = (j + 1) * nip + i;
                    indexL[3] = (j + 1) * nip + i + 1;
                    indexL[4] = j * nip + i;
                    indexL[5] = j * nip + i + 1;
                    indexL[6] = (j + 1) * nip + i;
                    indexL[7] = (j + 1) * nip + i + 1;
//                    printf("L ");
//                    for (int l = 0; l < 8; l++)
//                    {
//                        printf("%d ", indexL[l]);
//                    }
//                    printf("\n");
//                    fflush(0);

                    int indexZ[8]; /// z坐标的索引
                    indexZ[0] = (2 * k) * (2 * m_j) * (2 * m_i) + (2 * j) * (2 * m_i) + (2 * i);
                    indexZ[1] = (2 * k) * (2 * m_j) * (2 * m_i) + (2 * j) * (2 * m_i) + (2 * i + 1);
                    indexZ[2] = (2 * k) * (2 * m_j) * (2 * m_i) + (2 * j + 1) * (2 * m_i) + (2 * i);
                    indexZ[3] = (2 * k) * (2 * m_j) * (2 * m_i) + (2 * j + 1) * (2 * m_i) + (2 * i + 1);
                    indexZ[4] = (2 * k + 1) * (2 * m_j) * (2 * m_i) + (2 * j) * (2 * m_i) + (2 * i);
                    indexZ[5] = (2 * k + 1) * (2 * m_j) * (2 * m_i) + (2 * j) * (2 * m_i) + (2 * i + 1);
                    indexZ[6] = (2 * k + 1) * (2 * m_j) * (2 * m_i) + (2 * j + 1) * (2 * m_i) + (2 * i);
                    indexZ[7] = (2 * k + 1) * (2 * m_j) * (2 * m_i) + (2 * j + 1) * (2 * m_i) + (2 * i + 1);
//                    printf("Z ");
//                    for (int l = 0; l < 8; l++)
//                    {
//                        printf("%d ", indexZ[l]);
//                    }
//                    printf("\n");
//                    fflush(0);

                    SbVec3f coord[8];
                    for (int l = 0; l < 8; l++)
                    {
                        if (i < 1 && j < 1 && k < 1)
                        {
//                            qDebug() << 2 * indexL[l] << 2 * indexL[l] + 1 << indexZ[l] << zValues[indexZ[l]];
//                            debug(verticalLines[2 * indexL[l]]);
//                            debug(verticalLines[2 * indexL[l] + 1]);
//                            debug(coord[l]);
//                            BP;
                        }
                        getCoordByLineAndZ(verticalLines[2 * indexL[l]],
                                           verticalLines[2 * indexL[l] + 1],
                                           zValues[indexZ[l]],
                                           coord[l]);
                    }

                    int indexD[8]; /// 方向的索引
                    indexD[0] = 128;
                    indexD[1] = 64;
                    indexD[2] = 32;
                    indexD[3] = 16;
                    indexD[4] = 8;
                    indexD[5] = 4;
                    indexD[6] = 2;
                    indexD[7] = 1;

                    if (pointCreated[indexP[0]] == false) point[indexP[0]] = GSPoint(i, j, k);
                    if (pointCreated[indexP[1]] == false) point[indexP[1]] = GSPoint(i + 1, j, k);
                    if (pointCreated[indexP[2]] == false) point[indexP[2]] = GSPoint(i, j + 1, k);
                    if (pointCreated[indexP[3]] == false) point[indexP[3]] = GSPoint(i + 1, j + 1, k);
                    if (pointCreated[indexP[4]] == false) point[indexP[4]] = GSPoint(i, j, k + 1);
                    if (pointCreated[indexP[5]] == false) point[indexP[5]] = GSPoint(i + 1, j, k + 1);
                    if (pointCreated[indexP[6]] == false) point[indexP[6]] = GSPoint(i, j + 1, k + 1);
                    if (pointCreated[indexP[7]] == false) point[indexP[7]] = GSPoint(i + 1, j + 1, k + 1);
                    for (int l = 0; l < 8; l++)
                    {
                        pointCreated[indexP[l]] = true;
                    }

                    for (int l = 0; l < 8; l++)
                    {
                        point[indexP[l]].setPointCoord(indexD[l], coord[l]);
                    }
                }
            }
        }

        for (int it = 0; it < nip * njp * nkp; it++)
        {
            m_vecPoints.append(point[it]);
        }

        int i = 0;
        int j = 0;
        int k = 0;
        for (k = 0; k < nkp; k++)
        {
            for (j = 0; j < njp; j++)
            {
                for (i = 0; i < nip; i++)
                {
                    if ((i == 0 || i == 1) && (j == 1 || j == 2) && (k == 0 || k == 1))
                    {
                        int index = k * njp * nip + j * nip + i;
                        GSPoint pt = m_vecPoints.at(index);
                        SbVec3f vec1 = pt.getPointCoord(true, true, true);
                        SbVec3f vec2 = pt.getPointCoord(true, true, false);
                        SbVec3f vec3 = pt.getPointCoord(true, false, true);
                        SbVec3f vec4 = pt.getPointCoord(true, false, false);
                        SbVec3f vec5 = pt.getPointCoord(false, true, true);
                        SbVec3f vec6 = pt.getPointCoord(false, true, false);
                        SbVec3f vec7 = pt.getPointCoord(false, false, true);
                        SbVec3f vec8 = pt.getPointCoord(false, false, false);

                        pt.debugCoords();
                        qDebug() << i << j << k;
                        debug(vec8);
                        debug(vec7);
                        debug(vec6);
                        debug(vec5);
                        debug(vec4);
                        debug(vec3);
                        debug(vec2);
                        debug(vec1);
                        qDebug();
                    }
                }
            }
        }
    }


    for (int i = 0; i < m_vecPORO.size(); i++)
    {
        if (m_vecPORO[i] > m_fMaxPORO) m_fMaxPORO = m_vecPORO[i];
        if (m_vecPORO[i] < m_fMinPORO) m_fMinPORO = m_vecPORO[i];
    }
    for (int i = 0; i < m_vecPERM.size(); i++)
    {
        if (m_vecPERM[i] > m_fMaxPERM) m_fMaxPERM = m_vecPERM[i];
        if (m_vecPERM[i] < m_fMinPERM) m_fMinPERM = m_vecPERM[i];
    }
    /// 初始化把ColorMap的min/max设成整个数据的PORO的min/max
    {
        m_fColorMapMin = m_fMinPORO;
        m_fColorMapMax = m_fMaxPORO;
    }

    bool bDebugCheckFinalData = false;
    if (bDebugCheckFinalData)
    {
        qDebug() << m_i << m_j << m_k;
        for (int it = 0; it < m_vecPoints.size(); it++)
        {
            int i, j, k;
            GSPoint pt = m_vecPoints.at(it);
            pt.getIJK(i, j, k);
            qDebug() << i << j << k;
            SbVec3f vec1 = pt.getPointCoord(true, true, true);
            SbVec3f vec2 = pt.getPointCoord(true, true, false);
            SbVec3f vec3 = pt.getPointCoord(true, false, true);
            SbVec3f vec4 = pt.getPointCoord(true, false, false);
            SbVec3f vec5 = pt.getPointCoord(false, true, true);
            SbVec3f vec6 = pt.getPointCoord(false, true, false);
            SbVec3f vec7 = pt.getPointCoord(false, false, true);
            SbVec3f vec8 = pt.getPointCoord(false, false, false);

            qDebug() << vec1[0] << vec1[1] << vec1[2];
            qDebug() << vec2[0] << vec2[1] << vec2[2];
            qDebug() << vec3[0] << vec3[1] << vec3[2];
            qDebug() << vec4[0] << vec4[1] << vec4[2];
            qDebug() << vec5[0] << vec5[1] << vec5[2];
            qDebug() << vec6[0] << vec6[1] << vec6[2];
            qDebug() << vec7[0] << vec7[1] << vec7[2];
            qDebug() << vec8[0] << vec8[1] << vec8[2];
            qDebug();
        }
    }

    return 0;
}

void GS3DGrid::clearData()
{
    m_fMinPORO = -g_fInvalidValue;
    m_fMaxPORO = g_fInvalidValue;
    m_fMinPERM = -g_fInvalidValue;
    m_fMaxPERM = g_fInvalidValue;
    m_vecFacetPORO.clear();
    m_vecFacetPERM.clear();
    m_nFacetCount = 0;

    m_i = -1;
    m_j = -1;
    m_k = -1;
    m_vecPoints.clear();
    m_vecPORO.clear();
    m_vecPERM.clear();
    m_vecACTNUM.clear();

}

void GS3DGrid::compressData(QVector<int> &allIndex, QVector<SbVec3f> &allPoints)
{
    /// TODO :
}
