/// File Name : GS3dGridSlice.h

#ifndef GS3DGRIDSLICE_H
#define GS3DGRIDSLICE_H

#include "GS3DGrid.h"

class GS3DGridSlice : public GS3DGrid
{
public:
    GS3DGridSlice(QString uniqueName,
                  GS3DObjectParameters para);

    virtual ~GS3DGridSlice();

};

#endif // GS3DGRIDSLICE_H
