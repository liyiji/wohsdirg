/// File Name : GS3dObject.cpp

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>

#include "GS3DObject.h"

GS3DObject::GS3DObject(GSObjectType type,
                       QString uniqueName,
                       GS3DObjectParameters para)
{
    m_parameters.xTrans = para.xTrans;
    m_parameters.yTrans = para.yTrans;
    m_parameters.zTrans = para.zTrans;
    m_parameters.xScale = para.xScale;
    m_parameters.yScale = para.yScale;
    m_parameters.zScale = para.zScale;

    m_eObjectType = type;
    splitAndSetUniqueName(uniqueName);

    m_bShow = false;
    m_bUseColorMap = true;
    /// TODO :
//    m_lColorMap = EPColormapWidget::getDefaultColors("Attri_3Dview_A");
    m_fColorMapMin = 99999999.0;
    m_fColorMapMax = -99999999.0;

    m_pSep = 0;
    m_pSw = 0;
}

GS3DObject::~GS3DObject()
{
    if (m_pSep)
    {
        m_pSep->unref();
        m_pSep = 0;
    }
}

void GS3DObject::calcHistogram()
{

}

void GS3DObject::show()
{
    if (m_pSw)
    {
        m_pSw->whichChild.setValue(SO_SWITCH_ALL);
    }
}

void GS3DObject::hide()
{
    if (m_pSw)
    {
        m_pSw->whichChild.setValue(SO_SWITCH_NONE);
    }
}

void GS3DObject::setOIV3dObjectParameters(GS3DObjectParameters para)
{
    m_parameters = para;
    refreshObject();
}

void GS3DObject::splitAndSetUniqueName(QString str)
{
    m_sUniqueNameList = str.split(g_sUniqueNameSpliter);
}
