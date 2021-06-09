//
//  main.cpp
//  Project4
//
//  Created by Edmund Zhi on 5/31/21.
//

#include <iostream>
#include "Table.h"
#include <cassert>
using namespace std;

void testInsert() {
    vector<string> cols = { "customer", "product", "price", "location" };
    Table t("customer", cols);
    assert(t.good());
    assert(t.insert("Patel 12345 42.54 Westwood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
    assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
    assert( ! t.insert("Figueroa 54321 59.95"));
    vector<vector<string>> v;
    t.find("Patel", v);
    assert(v.size() == 2);
    vector<vector<string>> expected = {
        { "Patel", "12345", "42.54", "Westwood" },
        { "Patel", "67890", "142.75", "Hollywood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
    
    assert(t.select("location < Westwood", v) == 0);
    assert(v.size() == 2);
    expected = {
        { "Hoang", "12345", "30.46", "Santa Monica" },
        { "Patel", "67890", "142.75", "Hollywood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
}

void testSelect() {

    vector<string> cols = { "item name", "price" };
    Table t("item name", cols);
    assert(t.good());
    assert(t.insert("'chocolate bar' 1.69"));
    assert(t.insert("coffee 7.99"));
    assert(t.insert("hummus 3.49"));
    vector<vector<string>> v;
//    assert(t.select("price LT 5.", v) == 0);
    assert(t.select("price LT 5", v) == 0);
    assert(v.size() == 2);  // chocolate bar and hummus
    assert(t.insert("pretzels 1.W9"));
    assert(t.select("price LT H", v) == -1);
    assert(v.size() == 0);
    assert(t.select("price LT 5", v) == 1);  // 1 because pretzels 1.W9
    assert(v.size() == 2);  // chocolate bar and hummus
}

void test3() {
    vector<string> cols = { "N", "Z" };
    Table t("Z", cols);
    assert(t.good());
    assert(t.insert("UCLA 90095"));
    assert(t.insert("Caltech 91125"));
    vector<vector<string>> v;
    t.find("90095", v);
    assert(v.size() == 1);
    assert(v[0][0] == "UCLA"  &&  v[0][1] == "90095");
    assert(t.select("Z > 90210", v) == 0);
    assert(v.size() == 1);
    assert(v[0][0] == "Caltech"  &&  v[0][1] == "91125");

    vector<string> uclacols = { "last name", "first name", "major", "UID", "GPA" };
    Table ucla("UID", uclacols);
    assert(ucla.good());
    assert(ucla.insert("Bruin Jose 'COG SCI' 304567890 3.4"));
    assert(ucla.insert("Bruin Josephine 'COM SCI' 605432109 3.8"));
    assert(ucla.insert("Trojan Tommy LOSING 000000000 1.7"));
      // Troy brought a wooden horse full of Greek soldiers inside the
      // city walls and lost the Trojan War.  How strange to look up to
      // gullible losers as role models.
    assert(ucla.select("GPA GE 3.2", v) == 0);
    assert(v.size() == 2);
    assert(v[0][0] == v[1][0]  &&  v[0][1] != v[1][1]);
}

void testBonus() {
    vector<string> cols = { "customer", "product", "price", "location" };
    Table t("customer", cols);
    assert(t.good());
    assert(t.insert("Patel 12345 42.54 Westwood"));
    assert(t.insert("O'Reilly 34567     4.99 Westwood   "));
    assert(t.insert("   Hoang  12345 30.46 'Santa Monica' "));
    assert(t.insert("Patel\t67890\t142.75  \t \t\t  \tHollywood"));
    assert( ! t.insert("Figueroa 54321 59.95"));
    vector<vector<string>> v;
    t.find("Patel", v);
    assert(v.size() == 2);
    vector<vector<string>> expected = {
        { "Patel", "12345", "42.54", "Westwood" },
        { "Patel", "67890", "142.75", "Hollywood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
    
    assert(t.select("( location == Westwood | location == Hollywood | location == 'Santa Monica' ) '&' price LT 100", v) == 0);
    assert(v.size() == 3);
    assert(t.select("( location == Westwood | ) '&' price LT 100", v) == -1);
    assert(v.size() == 0);
    assert(t.select("( location == Westwood ) '&' price LT 100", v) == 0);
    assert(t.select("( location == Westwood | location == Hollywood ) '&' price LT 100", v) == 0);

    assert(v.size() == 2);
    expected = {
        { "O'Reilly", "34567", "4.99", "Westwood" },
        { "Patel", "12345", "42.54", "Westwood" }
    };
    assert((v[0] == expected[0]  &&  v[1] == expected[1])  ||
           (v[0] == expected[1]  &&  v[1] == expected[0]) );
    assert(t.select("(customer = Patel)", v) == -1);
    assert(t.select("product = 12345 location != Westwood", v) == -1);
    assert(t.select("location = ( Westwood | Hollywood )", v) == -1);
    assert(t.select("", v) == -1);
    assert(t.select( " ", v) == -1);
    assert(t.select(" location < ", v) == -1);
    assert(t.select(" location Westwood", v) == -1);
    assert(t.select("location < < ", v) == -1);
    assert(t.select("location < Westwood <", v) == -1);
    assert(t.select(" location Westwood = ", v) == -1);

    assert(t.select("customer = Patel", v) == 0);
    assert(t.select("customer = Edmund", v) == 0);
    assert(v.size() == 0);
    assert(t.select("( customer = Patel ')'", v) == 0);
    assert(t.select("price GE 20  &  location != Westwood", v) == 0);

}

void testEdgeCases() {
    vector<string> cols = { "&", "|", "(", ")" };
    Table t("&", cols);
    assert(!t.good());
    assert(!t.insert("Patel 12345 42.54 Westwood"));
    cols = { "customer", "product", "price", "location" };
    Table t2("test", cols);
    assert(!t2.good());
    assert(!t2.insert("Patel 12345 42.54 Westwood"));
    cols = { };
    Table t3("test", cols);
    assert(!t3.good());
    assert(!t3.insert("Patel 12345 42.54 Westwood"));
    vector<vector<string>> v;
    assert(t3.select("", v) == -1);
    assert(t3.select("location < ", v) == -1);
    assert(t3.select("location Westwood", v) == -1);
}

int main() {
//     insert code here...
    testInsert();
    testSelect();
    test3();
    testBonus();
    testEdgeCases();
    cerr << "Passed All Tests" << endl;
}

