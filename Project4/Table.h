//
//  Table.h
//  Project4
//
//  Created by Edmund Zhi on 5/31/21.
//

#ifndef Table_h
#define Table_h

#include <string>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <iostream>
#include <cstdlib>
#include <stack>

using namespace std;

const int MAX_BUCKETS = 997; // largest prime closest to 1000 to create less collisions

class Table
    {
      public:
        Table(std::string keyColumn, const std::vector<std::string>& columns);
        ~Table();
        bool good() const;
        bool insert(const std::string& recordString);
        void find(std::string key, std::vector<std::vector<std::string>>& records) const;
        int select(std::string query, std::vector<std::vector<std::string>>& records) const;

          // We prevent a Table object from being copied or assigned by
          // making the copy constructor and assignment operator unavailable.
        Table(const Table&) = delete;
        Table& operator=(const Table&) = delete;
      private:
        int hashFunction(string key) const {
            return std::hash<string>()(key) % MAX_BUCKETS;
        }
        string m_keyCol; // of key column
        int m_numCols; // number of columns
        int m_indexOfKey; // index of keycolumn
        vector<string> m_colNames; // vector of column names
        vector<vector<string>> m_buckets[MAX_BUCKETS]; // buckets of DataItems, DataItems are vectors of strings
        
        
    };


#endif /* Table_h */
