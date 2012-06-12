/// File Name : GSSelection.cpp

#include <Inventor/SoPath.h>

#include "GSSelection.h"
#include "GSView.h"

GSSelection::GSSelection()
{

}

GSSelection::~GSSelection()
{

}

void GSSelection::SelectObject(void *userData, SoPath *path)
{
    /// TODO :
    if (path->getLength() < 2)
    {
        return;
    }

//    GSView *pView = dynamic_cast<GSView *> (userData);
    GSView *pView = (GSView *) (userData);
    if (pView == 0)
    {
        return;
    }

    SoNode *pTailNode = path->getNodeFromTail(0);
    if (pTailNode == 0)
    {
        return;
    }
}

void GSSelection::DeselectObject(void *userData, SoPath *path)
{
    /// TODO :
}
