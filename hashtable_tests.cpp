#include <gtest/gtest.h>

#include "publicified_ht.h"
#include "hashtable_tests_utils.h"
#include <stdexcept>
#include <random_generator.h>

using namespace std;

struct TestHash2 {
    HASH_INDEX_T operator()(int k) const
    {
        return k;
    }
};
TEST(HashTable, LinearProberFailureCheck) {
    LinearProber<int> prober;
    int dummyKey = 1;
    prober.init(5, 11, dummyKey);
    //Check that the sequence is correct (Should not fail since given as starter code)
    for(size_t i = 0; i < 11; i++) {
        EXPECT_EQ((i + 5)%11, prober.next());
    }
    //Check for npos
    EXPECT_EQ(prober.next(), (HASH_INDEX_T) - 1); //check it is npos
}

TEST(HashTable, DoubleHashProberSequenceCheck) {
    DoubleHashProber<int, TestHash2> prober;
    int dummyKey = 8;
    // should choose the internal modulo of 7 from the table
    prober.init(5, 11, dummyKey);
    // Step size should be 6 (i.e. 7 - 8%7)
    //Check that the sequence is correct 
    vector<size_t> solutions1 = {5, 0, 6, 1, 7, 2, 8};
    for(size_t i = 0; i < 6; i++) {
        EXPECT_EQ(solutions1[i], prober.next());
    }
    // should choose the internal modulo of 43 from the table
    dummyKey = 38;
    prober.init(16, 47, dummyKey);
    // Step size should be 5 (i.e. 43 - 38%43)
    //Check that the sequence is correct 
    vector<size_t> solutions2 = {16, 21, 26, 31, 36, 41, 46, 4, 9, 14, 19}; 
    for(size_t i = 0; i < 11; i++) {
        EXPECT_EQ(solutions2[i], prober.next());
    }
}


TEST(HashTable, DoubleHashProberFailureCheck) {
    DoubleHashProber<int, TestHash2> prober;
    int dummyKey = 8;
    prober.init(5, 7, dummyKey);
    //Prober unit it reaches failure state
    for(size_t i = 0; i < 7; i++) {
        prober.next();
    }
    EXPECT_EQ(prober.next(), (HASH_INDEX_T) - 1); //Check it is npos
}

TEST(HashTable, Constructor) {
    HashTable<string, int, LinearProber<string>, hash<string>, equal_to<string> > ht;
    HashTable<int, string, DoubleHashProber<string, TestHash2>, hash<int>, equal_to<int> > ht2;
}

TEST(HashTable, Insert) {
    HashTable<string, int, LinearProber<string>, hash<string>, equal_to<string> > ht;
    set<pair<string, int>> items;
    //Insert (one, 1)
    pair<string, int> pair1("one", 1);
    ht.insert(pair1);
    items.insert(pair1);
    EXPECT_TRUE(verifyItems(ht, items));
    //Insert (two, 2)
    pair<string, int> pair2("two", 2);
    ht.insert(pair2);
    items.insert(pair2);
    EXPECT_TRUE(verifyItems(ht, items));
    //Insert (three, 3)
    pair<string, int> pair3("three", 3);
    ht.insert(pair3);
    items.insert(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, InsertDuplicate) {
    HashTable<string, int, LinearProber<string>, hash<string>, equal_to<string> > ht;
    set<pair<string, int>> items;
    //Insert (one, 1)
    pair<string, int> pair1("one", 1);
    ht.insert(pair1);
    items.insert(pair1);
    EXPECT_TRUE(verifyItems(ht, items));
    //Insert (two, 2)
    pair<string, int> pair2("two", 2);
    ht.insert(pair2);
    items.insert(pair2);
    EXPECT_TRUE(verifyItems(ht, items));
    pair<string, int> pair1dup("one", 3);
    ht.insert(pair1dup);
    items.erase(pair1);
    items.insert(pair1dup);
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, InsertCollision) {
    HashTable<int, int, LinearProber<int>, IntHash, equal_to<int>> ht(.4, LinearProber<int>(), IntHash(11));
    set<pair<int,int>> items;
    //Create items that create collisions
    pair<int,int> pair1(1,1);
    pair<int,int> pair2(12,2);
    pair<int,int> pair3(23,3);
    //Add to hashtable
    ht.insert(pair1);
    items.insert(pair1);
    ht.insert(pair2);
    items.insert(pair2);
    ht.insert(pair3);
    items.insert(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, InsertResize) {
    //Reach the default alpha factor of .4 (5 items /11 items = .45) to force a resize
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    set<pair<string, int>> items;
    for(int i = 0; i < 5; i++) {
        pair<string, int> newItem(to_string(i), i);
        ht.insert(newItem);
        items.insert(newItem);
    }
    EXPECT_EQ(ht.table_.size(), 11);
    //add another item should resize
    pair<string,int> newItem(to_string(5),5);
    ht.insert(newItem);
    items.insert(newItem);
    EXPECT_EQ(ht.table_.size(), 23);
    //check that all the items are still there
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, InsertResizeFactor) {
    //Reach the alpha factor of .7 (8 items/11 = .73) to force a resize
    HashTable<string, int, LinearProber<string>, hash<string>, equal_to<string> > ht(.70);
    set<pair<string, int>> items;
    for(int i = 0; i < 8; i++) {
        pair<string, int> newItem(to_string(i), i);
        ht.insert(newItem);
        items.insert(newItem);
    }
    EXPECT_EQ(ht.table_.size(),11);
    //add another item should resize
    pair<string,int> newItem(to_string(8),8);
    ht.insert(newItem);
    items.insert(newItem);
    EXPECT_EQ(ht.table_.size(), 23);
    //check that all the items are still there
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, Find) {
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    pair<string, int> pair1("one", 1);
    pair<string, int> pair2("two", 2);
    pair<string, int> pair3("three", 3);
    ht.insert(pair1);
    ht.insert(pair2);
    ht.insert(pair3);
    EXPECT_EQ(*ht.find("one"), pair1);
    EXPECT_EQ(*ht.find("two"), pair2);
    EXPECT_EQ(*ht.find("three"), pair3);
    EXPECT_EQ(ht.find("zero"), nullptr);
    EXPECT_EQ(ht.find("cats"), nullptr);
}

TEST(HashTable, FindCollisions) {
    HashTable<int, int, LinearProber<int>, IntHash, equal_to<int>> ht(.4, LinearProber<int>(), IntHash(11));
    //Create items that create collisions
    pair<int,int> pair1(1,1);
    pair<int,int> pair2(12,2);
    pair<int,int> pair3(23,3);

    //Insert Items
    ht.insert(pair1);
    ht.insert(pair2);
    ht.insert(pair3);

    //Find all items
    EXPECT_EQ(*ht.find(pair1.first), pair1);
    EXPECT_EQ(*ht.find(pair2.first), pair2);
    EXPECT_EQ(*ht.find(pair3.first), pair3);

    //Should not find other items that would cause collisions
    EXPECT_EQ(ht.find(34), nullptr);
    EXPECT_EQ(ht.find(45), nullptr);
}

TEST(HashTable, Remove) {
    pair<string, int> pair1("one", 1);
    pair<string, int> pair2("two", 2);
    pair<string, int> pair3("three", 3);
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    set<pair<string, int>> items;
    //Insert Items
    ht.insert(pair1);
    ht.insert(pair2);
    ht.insert(pair3);
    items.insert(pair1);
    items.insert(pair2);
    items.insert(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
    //Remove Items
    ht.remove(pair1.first);
    items.erase(pair1);
    EXPECT_TRUE(verifyItems(ht, items));
    ht.remove(pair3.first);
    items.erase(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
    ht.remove(pair2.first);
    items.erase(pair2);
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, RemoveNothing) {
    pair<string, int> pair1("one", 1);
    pair<string, int> pair2("two", 2);
    pair<string, int> pair3("three", 3);
    pair<string, int> nonExistantPair("cat", 25);
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    set<pair<string, int>> items;
    //Insert Items
    ht.insert(pair1);
    ht.insert(pair2);
    ht.insert(pair3);
    items.insert(pair1);
    items.insert(pair2);
    items.insert(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
    //Try removing non-existant items
    ht.remove("cat");
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, RemoveCollisions) {
    HashTable<int, int, LinearProber<int>, IntHash, equal_to<int>> ht(.4, LinearProber<int>(), IntHash(11));
    set<pair<int, int>> items;
    pair<int,int> pair1(1,1);
    pair<int,int> pair2(12,2);
    pair<int,int> pair3(23,3);
    pair<int,int> pair4(34,4);

    //Insert object that all hash to same value
    ht.insert(pair1);
    items.insert(pair1);
    ht.insert(pair2);
    items.insert(pair2);
    ht.insert(pair3);
    items.insert(pair3);
    ht.insert(pair4);
    items.insert(pair4);
    EXPECT_TRUE(verifyItems(ht, items));
    //Remove items, check to make sure we can still find the rest
    ht.remove(pair1.first);
    items.erase(pair1);
    EXPECT_TRUE(verifyItems(ht, items));
    ht.remove(pair3.first);
    items.erase(pair3);
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable, TestSize) {
    pair<string, int> pair1("one", 1);
    pair<string, int> pair2("two", 2);
    pair<string, int> pair3("three", 3);
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    EXPECT_EQ(ht.size(), 0);
    //Insert Items
    ht.insert(pair1);
    EXPECT_EQ(ht.size(), 1);
    ht.insert(pair2);
    EXPECT_EQ(ht.size(), 2);
    ht.insert(pair3);
    EXPECT_EQ(ht.size(), 3);
    //Remove Items
    ht.remove("one");
    EXPECT_EQ(ht.size(), 2);
    ht.remove("three");
    EXPECT_EQ(ht.size(), 1);
    ht.remove("two");
    EXPECT_EQ(ht.size(), 0);
}


TEST(HashTable, TestEmpty) {
    pair<string, int> pair1("one", 1);
    pair<string, int> pair2("two", 2);
    pair<string, int> pair3("three", 3);
    HashTable<string, int, DoubleHashProber<string, std::hash<string>>, hash<string>, equal_to<string> > ht;
    EXPECT_EQ(ht.empty(), true);
    //Insert Items
    ht.insert(pair1);
    EXPECT_EQ(ht.empty(), false);
    ht.insert(pair2);
    EXPECT_EQ(ht.empty(), false);
    //Remove Items
    ht.remove("two");
    EXPECT_EQ(ht.empty(), false);
    ht.remove("one");
    EXPECT_EQ(ht.empty(), true);
    //Insert and remove again
    ht.insert(pair3);
    EXPECT_EQ(ht.empty(), false);
    ht.remove("three");
    EXPECT_EQ(ht.empty(), true);
}


TEST(HashTable,LogicErrorTest){
    //If no location is available to insert the item, you should throw std::logic_error.
    //assuming no resize is called (loading factor > 1 )
    HashTable<int, int, LinearProber<int>, IntHash2, equal_to<int>> ht(2.0, LinearProber<int>(), IntHash2());
    bool correctThrow = false;
    for(int i = 0; i<=11; i++){
        if (i < 11){
            pair<int,int> pair(i,i);
            ht.insert(pair);
        }
        else{
            //collision will occur at i == 11 (11 mod 11 = 0)
            try{
                pair<int,int> pair(i,i);
                ht.insert(pair);
            }
            catch(std::logic_error const & err){
                correctThrow = true;
            }
            catch(...) {
                correctThrow = false;
            }
        }
    }
    EXPECT_TRUE(correctThrow);
}

TEST(HashTable,InsertDoubleHashProbeCollision){
    HashTable<int, int, DoubleHashProber<int, std::hash<int>>, IntHash2, equal_to<int> > ht(0.9, DoubleHashProber<int, std::hash<int>>(), IntHash2());
    set<pair<int, int>> items;
    pair<int,int> pair1(1,1);
    pair<int,int> pair2(3,1);
    pair<int,int> pair3(12,11);
    pair<int,int> pair4(89,23);
    pair<int,int> pair5(2,2);
    pair<int,int> pair6(46,46);
    items.insert(pair1);
    items.insert(pair2);
    items.insert(pair3);
    items.insert(pair4);
    items.insert(pair5);
    items.insert(pair6);
    //first item = loc = 1
    ht.insert(pair1);
    EXPECT_EQ(ht.probe(pair1.first),1);
    //second item = loc = 3
    ht.insert(pair2);
    EXPECT_EQ(ht.probe(pair2.first),3);
    //same h1 as pair1, and h2(k) = 7-12%7 = 2, so we probe in steps of 2 
    // (i.e. 1 (collide), 3 (collide), 5 (open))
    ht.insert(pair3);
    EXPECT_EQ(ht.probe(pair3.first),5);
    //same hash as pair1, and h2(k) = 7-89%7 = 2, so we probe in steps of 2 
    // (i.e. 1 (collide), 3 (collide), 5 (collide), 7 (open))
    ht.insert(pair4);
    EXPECT_EQ(ht.probe(pair4.first),7);
    //first item = loc = 2
    ht.insert(pair5);
    EXPECT_EQ(ht.probe(pair5.first),2);
    //same hash as pair5, and h2(k) = 7-46%7 = 3, so we probe in steps of 3 
    // (i.e. 2 (collide with 2), 5 (collide with 12), 8 (open))
    ht.insert(pair6);
    EXPECT_EQ(ht.probe(pair6.first),8);
    EXPECT_EQ(ht.size(),6);
    EXPECT_TRUE(verifyItems(ht, items));
}


TEST(HashTable,InsertStressTestResizeOneLoadFactor){
    const vector<int> sizemap = 
    {
        11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759
    };
    HashTable<int, int, LinearProber<int>, IntHash2, equal_to<int>> ht(1.0, LinearProber<int>(), IntHash2());
    set<pair<int, int>> items;
    int prevsize;
    for(size_t i = 0;i<sizemap.size()-1;i++){
        if(i == 0){prevsize = 0;}
        else{prevsize = sizemap[i-1]+1;}
        for(int j = prevsize; j<=sizemap[i];j++){
            pair<int,int> pair(j,j);
            items.insert(pair);
            ht.insert(pair);
            if(j == sizemap[i]-1){
                EXPECT_EQ(ht.table_.size(),sizemap[i]);
                EXPECT_EQ(ht.size(),items.size());
            }
            if(j == sizemap[i]){
                //resize should happen.
                EXPECT_EQ(ht.table_.size(),sizemap[i+1]);
                EXPECT_EQ(ht.size(),items.size());               
            }
        }
    }
    //All items should still be there
    EXPECT_TRUE(verifyItems(ht, items));
}


TEST(HashTable,InsertStressTestResizeNotOneLoadFactor){
    double loadfactor = 0.6;
    const vector<int> sizemap = 
    {
       11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759
    };
    HashTable<int, int, LinearProber<int>, IntHash2, equal_to<int>> ht(loadfactor, LinearProber<int>(), IntHash2());
    set<pair<int, int>> items;
    int prevsize;
    for(size_t i = 0;i<sizemap.size()-1;i++){
        int min = (int)floor(sizemap[i]*loadfactor);
        if(i == 0){prevsize = 0;}
        else{prevsize = sizemap[i-1]+1;}
        for(int j = prevsize; j<=sizemap[i];j++){
            pair<int,int> pair(j,j);
            items.insert(pair);
            ht.insert(pair);
            if(j == min){
                EXPECT_EQ(ht.table_.size(),sizemap[i]);
                EXPECT_EQ(ht.size(),items.size());
            }
            if(j == min+1){
                //resize should happen.
                EXPECT_EQ(ht.table_.size(),sizemap[i+1]);
                EXPECT_EQ(ht.size(),items.size());               
            }
        }
    }
    //All items should still be there
    EXPECT_TRUE(verifyItems(ht, items));
}

TEST(HashTable,StressInsertRemoveResizeLinear){
    double loadfactor = 0.6;
    const vector<int> sizemap = 
    {
        11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421
    };
    HashTable<int, int, LinearProber<int>, IntHash2, equal_to<int>> ht(loadfactor, LinearProber<int>(), IntHash2());   
    set<pair<int, int>> items;
    for(size_t i = 0;i<sizemap.size()-1;i++){
        int min = (int)floor(sizemap[i]*loadfactor);
        for(int j = 0; j<=min/2;j++){
            pair<int,int> pair(j,j);
            //insert then delete
            ht.insert(pair);
            ht.remove(j);
            EXPECT_TRUE(ht.find(j) == nullptr);
        }
        EXPECT_EQ(ht.size(),0);
        //reinsert the same pair, loc should change
        for(int j = 0; j<(min-(min/2));j++){
            pair<int,int> pair(j,j);
            //insert again
            ht.insert(pair);
            items.insert(pair);
            EXPECT_EQ(ht.probe(j),(min/2)+1+j);
        }
        EXPECT_EQ(ht.table_.size(),sizemap[i]);
        EXPECT_EQ(ht.size(),items.size());
        //add another should resize/rehash
        pair<int,int> pair((min/2)+1,(min/2)+1);
        ht.insert(pair);
        items.insert(pair);
        EXPECT_EQ(ht.table_.size(),sizemap[i+1]);
        EXPECT_EQ(ht.size(),items.size());
        //should be put into the correct spot, now that
        //the "deleted" items are actually "deleted" when resizing
        for(int j = 0; j<=(min/2+1);j++){
            EXPECT_EQ(ht.probe(j),j);
        }
    }
    EXPECT_TRUE(verifyItems(ht, items));
}