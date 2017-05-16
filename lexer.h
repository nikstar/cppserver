
#include <string>
#include <vector>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <vector>
#include <type_traits>
#include <functional>
#include <cstring>
#include <map>

using namespace std;


enum struct Type : int {
    text = 10,
    identifier = 11,
    number = 12,
    
    openMeta = 20,
    closeMeta,
    
    openParen = 30,
    closeParen,

    openCurly = 40,
    closeCurly,
    
    openSquare = 50,
    closeSquare,
    
    
    itemFunction = 60,
    itemVar,
    itemIf,
    itemElse,
    itemWhile,
    itemFor,
    itemIn,
    itemBreak,
    itemContinue,
    itemReturn,

    null = 0    
};

struct Item {
    Item(Type typ, string val) : typ(typ), val(val) {}
    Type typ;
    string val;

    operator bool() {
        return typ != Type::null;
    }
};


vector<Item> Lex(string code);

