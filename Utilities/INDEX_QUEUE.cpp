#include "INDEX_QUEUE.h"

INDEX_QUEUE::INDEX_QUEUE             (unsigned int minimum, unsigned int maximum){
        Index    = 0;
        Minimum  = minimum;
        Capacity = (maximum - minimum) + 1;
        if(Capacity > 0 && Capacity <= maximum + 1){
                Occupied = new bool[Capacity];
        } else {
                Occupied = NULL;
        }
        Release_All();
}

void         INDEX_QUEUE::Release_All(){
        for(unsigned int i = 0; i < Capacity; i++){
                Occupied[i] = false;
        }
}

unsigned int INDEX_QUEUE::Capture    (){
        for (unsigned int End = Index + (Capacity * 2); Index <= End; Index++) {
                if (!Occupied[Index % Capacity]) {
                        Occupied[Index % Capacity] = true;
                        return Minimum + (Index % Capacity);
                }
        }
        return -1;
}

bool         INDEX_QUEUE::Capture    (unsigned int index){
        if(!Is_Captured(index)){
                Occupied[index - Minimum] = true;
                return true;
        } else {
                return false;
        }
}

bool         INDEX_QUEUE::Is_Captured(unsigned int index){
        if(index >= Minimum && index < Minimum + Capacity){
                return Occupied[index - Minimum];
        } else {
                return false;
        }
}

void         INDEX_QUEUE::Release    (unsigned int index){
        if(index >= Minimum && index < Minimum + Capacity){
                Occupied[index - Minimum] = false;
        }
}
