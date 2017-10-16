#ifndef BucketSort_h
#define BucketSort_h

#include <stdio.h>
#include <vector>

struct BucketSort {
    std::vector<unsigned> numbersToSort;
    void sort(unsigned numCores);
};
#endif /* BucketSort_h */
