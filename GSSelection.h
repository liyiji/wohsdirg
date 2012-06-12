/// File Name : GSSelection.h

#ifndef GSSELECTION_H
#define GSSELECTION_H

#include <Inventor/nodes/SoSelection.h>

class GSSelection : public SoSelection
{
public:
    GSSelection();
    virtual ~GSSelection();

public:
    static void SelectObject(void *userData, SoPath *path);
    static void DeselectObject(void *userData, SoPath *path);
};

#endif // GSSELECTION_H
