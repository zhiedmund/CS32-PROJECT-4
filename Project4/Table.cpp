//
//  Table.cpp
//  Project4
//
//  Created by Edmund Zhi on 6/2/21.
//
#include <string>
#include <algorithm>
#include <ctype.h>
#include <iostream>
#include <cstdlib>
#include <stack>
#include "Table.h"
using namespace std;
// StringParser

// A StringParser object is constructed from a string.  Successive calls
// to the getNextField member function extract the next field from the
// string, returning true if it got one and false if there was no next field.
// Fields are separated by blank, tab, carriage return, or newline, and
// may be surrounded by single quotes, which are stripped off in the
// extracted field.  Two consecutive single quotes in a quoted field are
// converted to one.
//
// Here's an example of what it does:
//     StringParser parser("  abc  'def ghi'  'Don''t cry'  ''  jk ");
//     string s;
//     assert(parser.getNextField(s)  &&  s == "abc");
//     assert(parser.getNextField(s)  &&  s == "def ghi");
//     assert(parser.getNextField(s)  &&  s == "Don't cry");
//     assert(parser.getNextField(s)  &&  s == "");
//     assert(parser.getNextField(s)  &&  s == "jk");
//     assert(!parser.getNextField(s));

class StringParser
{
  public:
    StringParser(std::string text = "")
    {
        setString(text);
    }

    void setString(std::string text)
    {
        m_text = text;
        m_start = 0;
    }

    bool getNextField(std::string& field);

  private:
    std::string m_text;
    size_t m_start;
};

bool StringParser::getNextField(std::string& fieldText)
{
    m_start = m_text.find_first_not_of(" \t\r\n", m_start);
    if (m_start == std::string::npos)
    {
        m_start = m_text.size();
        fieldText = "";
        return false;
    }
    if (m_text[m_start] != '\'')
    {
        size_t end = m_text.find_first_of(" \t\r\n", m_start+1);
        fieldText = m_text.substr(m_start, end-m_start);
        m_start = end;
        return true;
    }
    fieldText = "";
    for (;;)
    {
        m_start++;
        size_t end = m_text.find('\'', m_start);
        fieldText += m_text.substr(m_start, end-m_start);
        m_start = (end != std::string::npos ? end+1 : m_text.size());
        if (m_start == m_text.size()  ||  m_text[m_start] != '\'')
            break;
        fieldText += '\'';
    }
    return true;
}

Table::Table(std::string keyColumn, const std::vector<std::string>& columns) {
    m_colNames = columns;
    m_keyCol = keyColumn;
//    m_numCols = static_cast<int>(columns.size());
    m_numCols = columns.size();
//    if(!good()) {
//        exit(1);
//    }
    for (int i = 0; i < m_colNames.size(); i++) {
        if (m_colNames[i] == m_keyCol) {
            m_indexOfKey = i; // table may not always contain key
            break;
        }
    }
}

Table::~Table() {
    //The destructor does what is necessary to ensure that a Table releases any resources it holds when its lifetime ends.
}

bool Table::good() const {
    // Return true if the table was successfully constructed, and false otherwise.
    /*
     The parameters might be such that you could not construct a valid table: the columns vector might be empty or contain empty or duplicate strings, or the keyColumn might not be any of the column names.
     */
    if(m_colNames.empty()) {
        return false; // empty column vector
    }
    bool containsKey = false;
    for (int i = 0; i < m_colNames.size(); i++) { // for every element in the array
        if (m_colNames[i].empty()) { // if it is an empty item, return false
            return false;
        }
        for (int j = i+1; j < m_colNames.size(); j++) { // search every element after to see if there is a duplicate
            if (m_colNames[i] == m_colNames[j]) {
                return false; // contains duplicates
            }
        }
        if (!containsKey && m_colNames[i] == m_keyCol) {
            containsKey = true;
        }
        if(m_colNames[i] == "&" || m_colNames[i] == "|" || m_colNames[i] == "(" || m_colNames[i] == ")") {
            return false;
        }
    }
    if(!containsKey) {
        return false;
    }
    return true;
}

/*
     Insert a record into the database. The parameter is a string with the fields of the record in a format described below. If the string has the correct number of fields for the table, insert a record with those fields into the table and return true; otherwise, leave the table unchanged and return false. Here is an example:

           // Since C++11, vectors can be constructed from initializer lists
         vector<string> cols = {
             "customer", "product", "price", "location"
         };
         Table t("customer", cols);
         assert(t.good());
         assert(t.insert("Patel 12345 42.54 Westwood"));
         assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
         assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
         assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
         assert( ! t.insert("Figueroa 54321 59.95"));
     */

bool Table::insert(const std::string& recordString) {
    if(!good()) {
        return false;
    }
    StringParser parser(recordString);
    string s;
    vector<string> item;
    while(parser.getNextField(s)) {
        item.push_back(s);
    }
    if (item.size() != m_numCols) {
        return false; // incorrect number of fields for the table
    }
    int bucket = hashFunction(item[m_indexOfKey]); // decides which bucket to push our item into based on the index of the key
    m_buckets[bucket].push_back(item); // adds item to bucket
    return true;
}


/*
 Replace the value in the second parameter with a vector containing as many elements as there are records in the table whose key field is a string equal to the value of the first parameter. Each of those elements is one of the matching records, represented as a vector whose elements are the fields of that record. The records placed in the vector don't have to be in any particular order. For example, for the table t built in the example shown for the insert function, the assertions in the following should succeed:

     vector<vector<string>> v;
     t.find("Patel", v);
     assert(v.size() == 2);
     vector<vector<string>> expected = {
         { "Patel", "12345", "42.54", "Westwood" },
         { "Patel", "67890", "142.75", "Hollywood" }
     };
     assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
            (v[0] == expected[1]  &&  v[1] == expected[0]) );
 */

void Table::find(std::string key, std::vector<std::vector<std::string>>& records) const {
    if (!good()) {
        return;
    }
    vector<vector<string>> fromTable;
    string tempKey = key;
    int bucket = hashFunction(tempKey); //find the bucket of this key
    for (int i = 0; i < m_buckets[bucket].size(); i++) { // for each item in the bucket
        if (std::find(m_buckets[bucket][i].begin(), m_buckets[bucket][i].end(), key) != m_buckets[bucket][i].end()) { // if that item contains the key word
            fromTable.push_back(m_buckets[bucket][i]); // add to fromTable
        }
    }
    records = fromTable; // replace records with fromTable
}

bool stringToDouble(string s, double& d)
    {
        char* end;
        d = std::strtof(s.c_str(), &end);
        return end == s.c_str() + s.size()  &&  !s.empty();
    }

/*
The first parameter is a query (defined below) that specifies a condition that does or does not hold for a record (e.g., that a field named start date is greater than or equal to 2021-03-29). Replace the value in the second parameter with a vector containing as many elements as there are records in the table that satisfy the query. Each of those elements is one of the matching records, represented as a vector whose elements are the fields of that record. The records placed in the vector don't have to be in any particular order. The function returns 0 if it succeeds without any problems. (The return values are specified later.) For example, for the table t built in the example shown for the insert function, the assertions in the following should succeed:

 vector<vector<string>> v;
 assert(t.select("location < Westwood", v) == 0);
 assert(v.size() == 2);
 vector<vector<string>> expected = {
     { "Hoang", "12345", "30.46", "Santa Monica" },
     { "Patel", "67890", "142.75", "Hollywood" }
 };
 assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
        (v[0] == expected[1]  &&  v[1] == expected[0]) );
*/

bool isLetterOrCloseParen();

inline
int precedence(string str)
  //  Precondition:  ch is in "^&!("
{
//    static string ops = "^&!(";
//    static int prec[4] = { 1, 2, 3, 0 };
//    int pos = ops.find(ch);
//    assert(pos != string::npos);  // must be found!
//    return prec[pos];
    if (str == "==" || str == "=" || str == "<" || str == "<=" || str == ">=" || str == ">" || str == "!=" || str == "LT" || str == "LE" || str == "GT" || str == "GE" || str == "NE" || str == "EQ" ) {
        return 3;
    }
    else if (str == "&") {
        return 2;
    }
    else if (str == "|") {
        return 1;
    }
    else if (str == "(") {
        return 0;
    }
    else {
        return 0;
    }
}

const int RET_INVALID_EXPRESSION = -1;

bool isOperator(string str) {
    string logic = str;
    for_each(logic.begin(), logic.end(), [](char& c) {
        c = toupper(c);
    });
    return logic == "==" || logic == "=" || logic == "<" || logic == "<=" || logic == ">=" || logic == ">" || logic == "!=" || logic == "LT" || logic == "LE" || logic == "GT" || logic == "GE" || logic == "NE" || logic == "EQ";
}

bool isBooleanOperator(string str) {
    return str == "&" || str == "|";
}

bool isEntryOrCloseParen(string str) {
    return (!isOperator(str) && !isBooleanOperator(str) && (str != "(")) || (str == ")");
    // if its an entry, its not an operator or open parenthesis. 
}


int Table::select(std::string query, std::vector<std::vector<std::string>>& records) const  {
    if (!good()) {
        return -1;
    }
    records.clear(); // sets records to be an empty vector
    StringParser parser(query);
    string s, column, logic, valueToCompare;
    double temp;
    bool isNumericalComparison;
    int counter = 0;
    vector<string> infix;
    while (parser.getNextField(s)) {
        infix.push_back(s);
    }
    // first convert infix to postfix
    vector<string> postfix;
    stack<string> operatorStack;
    
    stack<vector<vector<string>>> toBeCombined;
    
    string prevstr = "|";  // pretend the previous character was an operator

    for (size_t k = 0; k < infix.size(); k++)
    {
        string str = infix[k];
        if (str == "(") {
            if(isEntryOrCloseParen(prevstr)) return RET_INVALID_EXPRESSION;
            operatorStack.push(str);
        }
        else if (str == ")") {
            if(!isEntryOrCloseParen(prevstr)) return RET_INVALID_EXPRESSION;
            for (;;)
            {
                if (operatorStack.empty())
                    return RET_INVALID_EXPRESSION;  // too many ')'
                string s = operatorStack.top();
                operatorStack.pop();
                if (s == "(")
                    break;
                postfix.push_back(s);
            }
        }
        else if (str == "&" || str == "|" || isOperator(str)) {
            if(!isEntryOrCloseParen(prevstr)) return RET_INVALID_EXPRESSION;
            
            while ( ! operatorStack.empty()  &&
                       precedence(str) <= precedence(operatorStack.top()) )
            {
                postfix.push_back(operatorStack.top());
                operatorStack.pop();
            }
            operatorStack.push(str);
        }
        else {
//            if ((!isOperator(prevstr) && !isBooleanOperator(prevstr) && (prevstr != "(")) || (prevstr == ")"))  {
//                return -1;
//            }
            if(isEntryOrCloseParen(prevstr)) return RET_INVALID_EXPRESSION;
            postfix.push_back(str);
        }
        prevstr = str;
    }
      // end of expression; pop remaining operators

    if(!isEntryOrCloseParen(prevstr)) return RET_INVALID_EXPRESSION;
    
    while ( ! operatorStack.empty())
    {
        string s = operatorStack.top();
        operatorStack.pop();
        if (s == "(")
            return RET_INVALID_EXPRESSION;  // too many '('
        postfix.push_back(s);
    }
    if (postfix.empty())
        return RET_INVALID_EXPRESSION;  // empty expression

        // postfix now contains the converted expression
        // Now evaluate the postfix expression

      stack<string> operandStack;
      for (size_t k = 0; k < postfix.size(); k++)
      {
          string str = postfix[k];
          isNumericalComparison = false;
          if (isOperator(str)) {
              logic = str;
              for_each(logic.begin(), logic.end(), [](char& c) {
                  c = toupper(c); // convert logic to uppercase
              });
              if (logic == "==" || logic == "=" || logic == "<" || logic == "<=" || logic == ">=" || logic == ">" || logic == "!=") {
                  isNumericalComparison = false; // these operators are for strings
              }
              else if (logic == "LT" || logic == "LE" || logic == "GT" || logic == "GE" || logic == "NE" || logic == "EQ" ) {
                  isNumericalComparison = true; // these operators are for numbers
              }
              else if (logic == "&" || logic == "|") {
                  return RET_INVALID_EXPRESSION;
              }
              if(operandStack.empty()) {
                  return -1;
              }
              valueToCompare = operandStack.top();
              operandStack.pop();
              if (isNumericalComparison && !stringToDouble(valueToCompare, temp)) { //if we are making a numerical comparison but valueToCompare is not a number, invalid query
                  return -1;
              }
              if(operandStack.empty()) {
                  return -1;
              }
              column = operandStack.top();
              operandStack.pop();
              if (std::find(m_colNames.begin(), m_colNames.end(), column) == m_colNames.end()) {
                  return -1; // column name is invalid
              }
              enum logicOperator {
                  LT, LE, GT, GE, NE, EQ
              };
              
              logicOperator op;
              
              if (logic == "==" || logic == "=" || logic == "EQ" || logic == "Eq" || logic == "eQ" || logic == "eq") {
                  op = EQ;
              }
              else if (logic == "<" || logic == "LT" || logic == "Lt" || logic == "lT" || logic == "lt"){
                  op = LT;
              }
              else if (logic == "<=" || logic == "LE" || logic == "Le" || logic == "lE" || logic == "le"){
                  op = LE;
              }
              else if (logic == ">" || logic == "GT" || logic == "Gt" || logic == "gT" || logic == "gt"){
                  op = GT;
              }
              else if (logic == ">=" || logic == "GE" || logic == "Ge" || logic == "gE" || logic == "ge"){
                  op = GE;
              }
              else if (logic == "!=" || logic == "NE" || logic == "Ne" || logic == "nE" || logic == "ne"){
                  op = NE;
              }
              else {
                  return -1; // error with logical operator
              }

              int indexOfColumn = 0;
              for (int i = 0; i < m_colNames.size(); i++) {
                  if (m_colNames[i] == column) {
                      indexOfColumn = i;
                      break;
                  }
              }
              vector<vector<string>> fromTable;
              if (op == LT ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp < value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] < valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
              else if (op == LE ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp <= value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] <= valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
              else if (op == GT ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp > value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] > valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
              else if (op == GE ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp >= value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] >= valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
              else if (op == NE ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp != value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] != valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
              else if (op == EQ ) {
                  for (int bucket = 0; bucket < MAX_BUCKETS; bucket++){
                      for (int i = 0; i < m_buckets[bucket].size(); i++) {
                          if (isNumericalComparison) {
                              if (!stringToDouble(m_buckets[bucket][i][indexOfColumn], temp)) {
                                  counter++;
                                  continue;
                              }
                              else {
                                  double value;
                                  stringToDouble(m_buckets[bucket][i][indexOfColumn], temp);
                                  stringToDouble(valueToCompare, value);
                                  if(temp == value) {
                                      fromTable.push_back(m_buckets[bucket][i]);
                                  }
                              }
                          }
                          else if (m_buckets[bucket][i][indexOfColumn] == valueToCompare) {
                              fromTable.push_back(m_buckets[bucket][i]);
                          }
                      }
                  }
              }
//              if (!fromTable.empty())
                  toBeCombined.push(fromTable);
          }
          else if (isBooleanOperator(str)) {
              if (toBeCombined.empty()) {
                  return RET_INVALID_EXPRESSION;
              }
              vector<vector<string>> operand1 = toBeCombined.top();
              toBeCombined.pop();
              std::sort(operand1.begin(), operand1.end());
              if (toBeCombined.empty()) {
                  return RET_INVALID_EXPRESSION;
              }
              vector<vector<string>> operand2 = toBeCombined.top();
              toBeCombined.pop();
              std::sort(operand2.begin(), operand2.end());
              vector<vector<string>> combined(operand1.size()+operand2.size());
              vector<vector<string>>::iterator it;
              if (str == "&") {
                  it = set_intersection(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), combined.begin());
                  combined.resize(it-combined.begin());
                  toBeCombined.push(combined);
              }
              else if (str == "|") {
                  it = set_union(operand1.begin(), operand1.end(), operand2.begin(), operand2.end(), combined.begin());
                  combined.resize(it-combined.begin());
                  toBeCombined.push(combined);
              }
          }
          else {
              operandStack.push(str);
          }
      }
    if (toBeCombined.size() != 1)  // Impossible!
        return RET_INVALID_EXPRESSION;  // pretend it's an invalid expression
    records = toBeCombined.top(); // replaces records with the combined vector
    toBeCombined.pop();
    return counter;
}




