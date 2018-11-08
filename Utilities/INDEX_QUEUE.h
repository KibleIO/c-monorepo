#ifndef __INDEX_QUEUE_H
#define __INDEX_QUEUE_H

#include <iostream>

using namespace std;

class INDEX_QUEUE {
    private:
        bool*        Occupied;
        unsigned int Index;
        unsigned int Minimum;
        unsigned int Capacity;
    public:
        INDEX_QUEUE             (unsigned int minimum, unsigned int maximum);
        void         Release_All();
        unsigned int Capture    ();
        bool         Capture    (unsigned int index);
        bool         Is_Captured(unsigned int index);
        void         Release    (unsigned int index);
};
#endif // __INDEX_QUEUE_H