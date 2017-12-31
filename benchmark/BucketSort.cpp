#include "BucketSort.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <vector>
#include <map>

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

unsigned getFirstDigit(unsigned n) {
    unsigned p = std::log10(n);
    unsigned x = std::pow(10, p);
    return n / x;
}

std::map<unsigned, unsigned> distributeWork(std::vector<unsigned>& numbersToSort, unsigned numCores) {
    std::map<unsigned, unsigned> res;
    std::map<unsigned, unsigned> stat;
    for (auto it = numbersToSort.cbegin(); it != numbersToSort.cend(); ++it) {
        unsigned d = getFirstDigit(*it);
        auto map_it = stat.find(d);
        if (map_it == stat.end()) {
            stat.emplace(std::make_pair(d, 1u));
        } else {
            ++(map_it->second);
        }
    }

    std::vector<std::pair<unsigned, unsigned>> tmp;
    std::copy(stat.cbegin(), stat.cend(), std::back_inserter(tmp));
    std::sort(tmp.begin(), tmp.end(), [] (std::pair<unsigned, unsigned>& lhs, std::pair<unsigned, unsigned>& rhs) {
        return lhs.second < rhs.second;
    });

    for (unsigned i = 0; i < numCores; ++i) {
        if (i == numCores - 1) {
            while (tmp.size()) {
                unsigned d = tmp.back().first;
                res.emplace(std::make_pair(d, i));
                tmp.pop_back();
            }
        } else {
            if (tmp.size()) {
                unsigned d = tmp.back().first;
                res.emplace(std::make_pair(d, i));
                tmp.pop_back();
            }
        }
    }
    return res;
}

// TODO: replace this with a parallel version.
void BucketSort::sort(unsigned int numCores) {
    // Phase 1
    unsigned n = static_cast<unsigned>(numbersToSort.size());
    std::map<unsigned, unsigned> digitToCore = distributeWork(numbersToSort, numCores);
    std::vector<std::thread> containerOfThreads;

    // Phase 2
    std::vector<std::vector<std::vector<unsigned>>> sending_buckets {numCores, std::vector<std::vector<unsigned>> {numCores, std::vector<unsigned> {}}};
    for (unsigned i = 0; i < numCores; ++i) {
        containerOfThreads.emplace_back(std::thread {[&, i] {
            auto begin = numbersToSort.begin() + i * n / numCores;
            auto end = numbersToSort.begin() + (i + 1) * n / numCores;
            for (auto it = begin; it != end; ++it) {
                unsigned d = getFirstDigit(*it);
                unsigned core_id = digitToCore.find(d)->second;
                sending_buckets[i][core_id].push_back(*it);
            }
        }});
    }

    for (auto it = containerOfThreads.begin(); it != containerOfThreads.end(); ++it)
        it->join();

    containerOfThreads.clear();
    
    // Phase 3
    std::vector<std::vector<std::vector<unsigned>>> receiving_buckets {numCores, std::vector<std::vector<unsigned>> {numCores, std::vector<unsigned> {}}};
    std::vector<std::vector<unsigned>> large_buckets {numCores, std::vector<unsigned> {}};
    for (unsigned i = 0; i < numCores; ++i) {
        containerOfThreads.emplace_back(std::thread {[&, i] {
            for (unsigned j = 0; j < numCores; ++j) {
                std::vector<unsigned>& from = sending_buckets[i][j];
                std::vector<unsigned>& to = receiving_buckets[j][i];
                to = std::move(from);
            }
        }});
    }

    for (auto it = containerOfThreads.begin(); it != containerOfThreads.end(); ++it)
        it->join();

    containerOfThreads.clear();

    for (unsigned i = 0; i < numCores; ++i) {
        containerOfThreads.emplace_back(std::thread {[&, i] {
            std::vector<std::vector<unsigned>>& buckets = receiving_buckets[i];
            std::vector<unsigned>& to = large_buckets[i];
            for (std::vector<unsigned>& from : buckets)
                std::copy(from.cbegin(), from.cend(), std::back_inserter(to));
            std::stable_sort(to.begin(), to.end(), [] (const unsigned& x, const unsigned& y) {
                return aLessB(x, y, 0);
            });
        }});
    }

    for (auto it = containerOfThreads.begin(); it != containerOfThreads.end(); ++it)
        it->join();

    containerOfThreads.clear();

    // Phase 4
    numbersToSort.clear();
    for (auto it = digitToCore.cbegin(); it != digitToCore.cend(); ++it) {
        unsigned core_id = it->second;
        std::vector<unsigned>& v = large_buckets[core_id];
        std::copy(v.cbegin(), v.cend(), std::back_inserter(numbersToSort));
    }   
}













