#include "BucketSort.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <cmath>
#include <mutex>

bool aLessB(const unsigned int& x, const unsigned int& y, unsigned int pow) {
    
    if (x == y) return false; // if the two numbers are the same then one is not less than the other
    
    unsigned int a = x;
    unsigned int b = y;
    
    // work out the digit we are currently comparing on.
    if (pow == 0) {
        while (a / 10 > 0) {
            a = a / 10;
        }
        while (b / 10 > 0) {
            b = b / 10;
        }
    } else {
        while (a / 10 >= (unsigned int) std::round(std::pow(10,pow))) {
            a = a / 10;
        }
        while (b / 10 >= (unsigned int) std::round(std::pow(10,pow))) {
            b = b / 10;
        }
    }
    
    if (a == b)
        return aLessB(x,y,pow + 1);  // recurse if this digit is the same
    else
        return a < b;
}

void fillSmallBucket(std::vector<std::vector<unsigned>>& v, std::vector<unsigned>::iterator start, std::vector<unsigned>::iterator end, unsigned interval_length, unsigned min) {
    for (auto it = start; it != end; ++it) {
        unsigned val = *it;
        unsigned target = (val - min) / interval_length;
        v.at(target).push_back(val);
    }
}


void communicate(std::vector<unsigned>& from, std::vector<unsigned>& to) {

}

// TODO: replace this with a parallel version.
void BucketSort::sort(unsigned int numCores) {
    /******************************************************************** Phase 1 *********************************************************************/
    unsigned n = static_cast<unsigned>(numbersToSort.size());
    unsigned max = *std::max_element(numbersToSort.begin(), numbersToSort.end());
    unsigned min = *std::min_element(numbersToSort.begin(), numbersToSort.end());
    unsigned p = numCores;
    
    std::vector<std::thread> containerOfThreads;
    std::vector<std::vector<std::vector<unsigned>>> small_buckets;
    for (unsigned i = 0; i < p; ++i) {
        small_buckets.emplace_back(std::vector<std::vector<unsigned>> {});
        for (unsigned j = 0; j < p; ++j) {
            small_buckets.at(i).emplace_back(std::vector<unsigned> {});
        }
    }
    unsigned interval_length;
    if ((max - min + 1) % p == 0) {
        interval_length = (max - min + 1) / p;
    } else {
        interval_length = (max - min + 1) / p + 1;
    }
    
    /******************************************************************** Phase 2 *********************************************************************/
    unsigned z = 0;
    for (unsigned i = 0; i < p; ++i) {
        auto start = numbersToSort.begin() + i * n / p;
        auto end = numbersToSort.begin() + (i + 1) * n / p;
        containerOfThreads.emplace_back(std::thread {fillSmallBucket, std::ref(small_buckets.at(z++)), start, end, interval_length, min});
    }
    
    for (auto it = containerOfThreads.begin(); it != containerOfThreads.end(); ++it)
        it->join();
    
    
    for (auto vv : small_buckets) {
        for (auto v : vv) {
            std::cout << "[ ";
            for (auto i : v) {
                std::cout << i << ' ';
            }
            std::cout << "]     ";
        }
        std::cout << '\n';
    }
    
    /******************************************************************** Phase 3 *********************************************************************/


    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}
