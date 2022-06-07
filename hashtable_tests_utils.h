#ifndef HASHTABLE_TESTS_UTILS
#define HASHTABLE_TESTS_UTILS

#include <gtest/gtest.h>
#include <sstream>
#include <set>

#include "ht.h"


//Hash function for purposely creating collisions
struct IntHash {
    size_t size;
    IntHash(size_t s) {
        size = s;
    }
    HASH_INDEX_T operator()(const int& k) const
    {
        return k % size;
    }
};

//Hash function for purposely creating collisions
struct IntHash2 {
    // size_t size;
    IntHash2() {}
    HASH_INDEX_T operator()(const int& k) const
    {
        return k;
    }
};


template<typename K, typename V, typename Prober, typename Hash, typename KEqual>
testing::AssertionResult verifyItems(HashTable<K, V, Prober, Hash, KEqual>& ht, std::set<std::pair<K, V>>& items) {
    std::stringstream err;
    if(ht.size() != items.size()) {
        err << "Error, the size of the hashtable is incorrect. Expected: " << items.size() << " Actual: " << ht.size();
        return testing::AssertionFailure() << err.str();
    }
    for(auto it = items.begin(); it != items.end(); ++it) {
        if(*ht.find(it->first) != *it) {
            err << "Could not find pair (" << it->first << ", " << it->second <<")";
            return testing::AssertionFailure() << err.str();
        }
    }
    return testing::AssertionSuccess();
}



#endif