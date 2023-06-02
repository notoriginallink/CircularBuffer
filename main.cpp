#include "lib\CCircularBuffer\CCircularBuffer.h"
#include "lib\CCircularBuffer\CCircularBufferExt.h"

#include <iostream>

template<typename T>
class iterator{
    using value_type = T;
    using reference = T&;

    iterator& operator++();
    iterator operator++(int);
    bool operator==(const iterator& other);
    bool operator!=(const iterator& other);
    value_type operator*();
    reference operator->();
    iterator operator=(const iterator& other);

};

int main() {

}
