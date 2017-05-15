
#include "lexer.h"

using namespace std;


// This lexer is based on the talk by Rob Pike: https://www.youtube.com/watch?v=HxaD_trXwRE


// http://stackoverflow.com/questions/23737449/recursive-typedef-function-definition-stdfunction-returning-its-own-type
template< typename... T >
struct RecursiveHelper
{
    typedef std::function< RecursiveHelper(T...) > type;
    RecursiveHelper( type f ) : func(f) {}
    operator type () { return func; }
    type func;
};

struct Lexer;
typedef RecursiveHelper<Lexer&>::type stateFn;


struct Lexer {
    Lexer(string code) : start(0), pos(0), items(), input(code) {}

    vector<Item> items;
    string input;
    int start, pos;

    void emit(Type t) {
        items.push_back(Item(t, input.substr(start, pos-start)));
        start = pos;   
        auto v = items[items.size()-1]; cerr << "emit token: " << int(v.typ) << " \"" << v.val << "\"" << endl;     
    }

    char next() {
        if (pos == input.size()) {
            return 0;
        }
        return input[pos++];
    }

    void ignore() {
        start = pos;
    }

    void backup() {
        --pos;
    }

    char peek() {
        auto c = next();
        backup();
        return c;
    }

    bool accept(string chars) {
        if (chars.find_first_of(next()) != -1) {
            return true;
        }
        backup();
        return false;
    }

    void acceptRun(string chars) {
        while (chars.find_first_of(next()) != -1) {
        }
        backup();
    }
};


stateFn lexText(Lexer& l);
stateFn lexOpenMeta(Lexer& l);
stateFn lexCloseMeta(Lexer& l);
stateFn lexItem(Lexer& l);
stateFn lexWord(Lexer& l);
stateFn lexNumber(Lexer& l);

// http://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-string-starts-with-a-certain-string-and-convert-a-sub
bool hasPrefix(string str, string prefix) {
    // cerr << "cmp " << prefix << " -> " << (strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0) << endl; 
    return strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}


const string openMeta = "<?";
const string closeMeta = "?>";

const map<string, Type> reserved = {
    {"function",    Type::itemFunction},
    {"var",         Type::itemVar},
    {"if",          Type::itemIf},
    {"else",        Type::itemElse},
    {"while",       Type::itemWhile},
    {"for",         Type::itemFor},
    {"in",          Type::itemIn},
    {"break",       Type::itemBreak},
    {"continue",    Type::itemContinue},
    {"return",      Type::itemReturn} 
};


vector<Item> Lex(string code) {
    Lexer l(code);
    
    for (stateFn state = lexText; state != nullptr; ) {
        state = state(l);
    }
    
    return l.items;
}


stateFn lexText(Lexer& l) {
    for (;;) {
        if (hasPrefix(l.input.substr(l.pos), openMeta)) {
            if (l.pos > l.start) {
                l.emit(Type::text);
            }
            return lexOpenMeta;
        }
        if (l.next() == 0) {
            break;
        }
    }
    if (l.pos > l.start) {
        l.emit(Type::text);
    }
    return nullptr;
}

stateFn lexOpenMeta(Lexer& l) {
    l.pos += openMeta.size();
    l.emit(Type::openMeta);
    return lexItem;
}

stateFn lexCloseMeta(Lexer& l) {
    l.pos += closeMeta.size();
    l.emit(Type::closeMeta);
    return lexText;
}

stateFn lexItem(Lexer& l) {
    for (;;) {
        if (hasPrefix(l.input.substr(l.pos), closeMeta)) {
            return lexCloseMeta;
        }
        char c = l.next();
        if (c == 0) {
            return nullptr; // note unclosed actions are ok
        }
        else if (c == ' ' || c == '\n') {
            l.ignore();
        }
        else if (c == '(') {
            l.emit(Type::openParen);
        }
        else if (c == ')') {
            l.emit(Type::closeParen);
        }
        else if (c == '{') {
            l.emit(Type::openCurly);
        }
        else if (c == '}') {
            l.emit(Type::closeCurly);
        }
        else if (c == '[') {
            l.emit(Type::openSquare);
        }
        else if (c == ']') {
            l.emit(Type::closeSquare);
        }
        else if (isalpha(c)) {
            l.backup();
            return lexWord;
        }
        else if (isdigit(c) || c == '+' || c == '-') {
            l.backup();
            return lexNumber;
        }
    }
}

stateFn lexWord(Lexer& l) {
    l.acceptRun("abcdefghijklmnopqrstuvwxyz0123456789");
    try {
        auto t = reserved.at(l.input.substr(l.start, l.pos-l.start));
        l.emit(t);
    }
    catch (exception) {
        l.emit(Type::identifier);
    }
    return lexItem;
}

stateFn lexNumber(Lexer& l) {
    l.accept("+-");
    l.acceptRun("0123456789");
    if (l.accept(".")) {
        l.acceptRun("0123456789");
    }
    l.emit(Type::number);
    return lexItem;
}

