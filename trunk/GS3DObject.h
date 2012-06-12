/// File Name : GS3dObject.h

#ifndef GS3DOBJECT_H
#define GS3DOBJECT_H

#include <QList>
#include <QColor>
#include <QString>

#include "GSPublicDefine.h"

class SoSwitch;
class SoSeparator;

/*
uniqueName规则

*/

/*
记得
1、把被拾取的节点添加上setUserData到this
2、uniqueName
*/
class GS3DObject
{
public:
    GS3DObject(GSObjectType type,
               QString uniqueName, /// uniqueName，由本类及其子类进行约定，做到独一无二即可
               GS3DObjectParameters para);
    virtual ~GS3DObject();

    virtual SoSeparator* getOIVNode() = 0;
    virtual int refreshObject() = 0;
    virtual void changeColorMap(QList<QColor>) = 0;

    virtual void show();
    virtual void hide();
    virtual void calcHistogram();
    virtual void setOIV3dObjectParameters(GS3DObjectParameters para);

protected:
    void splitAndSetUniqueName(QString str); /// uniqueName约定，分割符是m_sUniqueNameSpliter

protected:
    GS3DObjectParameters m_parameters;

    GSObjectType    m_eObjectType;
    QStringList     m_sUniqueNameList;
    bool            m_bShow;

    bool            m_bUseColorMap;
    QList<QColor>   m_lColorMap;
    double          m_fColorMapMin;
    double          m_fColorMapMax;

    SoSeparator*    m_pSep;
    SoSwitch*       m_pSw;
};

#endif // GS3DOBJECT_H
