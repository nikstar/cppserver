
#include "mjs.h"
#include "lexer.h"

using namespace std;

struct Parser {
    Parser(vector<Item> input) : input(input), start(0), pos(0) {}

    vector<Item> input;
    int start, pos;

    Item next() {
        if (pos == input.size()) {
            return Item(0, "");
        }
        return input[pos++];
    }

    void backup() {
        --pos;
    }

    Item peek() {
        auto c = next();
        backup();
        return c;
    }
}

class Node {
public:
    std::string id;
    
    void run(Context & ctx) = 0;
}





Node *Parse(vector<Item> input) {
    Parser p(input);
    return parseProgram(p);
}

Node *parseProgram(Parser& p) {
    Node *n = new Node();
    Nood *c = nullptr;
    for (;;) {
        auto item = p.peek();
        if (!item) { break; }
        if (item.typ == Type::itemFunction) {
            c = parseFunctionDefinition(p);
        } else {
            c = parseStatement(p);
        }
        if (!c) { break; }
        n.sub.push_back(c);
    }
    return n;
}

Node *parseFunctionDefinition(Parser& p) {
    Item item = p.next();
    if (item.typ != Type::itemFunction) {
        throw "parseFunctionDefinition: unexpected token";
    }
    item = p.next();
    if (item.typ != Type::identifier) {
        throw "parseFunctionDefinition: unexpected token";
    }
    string name = item.val;
    vector<string> args;
    item = p.next();
    if (item.typ != Type::openParen) {
        throw "parseFunctionDefinition: unexpected token";
    }
    while (p.peek().typ != Type::closeParen) {
        item = p.next();
        if (item.typ != Type::identifier) {
            throw "parseFunctionDefinition: unexpected token";
        }
        args.push_back(item.val);
        //if (p.peek.typ == Type::comma) { p.next() }
    }
    p.next();
    auto block = parseBlock();
    return new FunctionDefinition(name, args, block);
}

Node *parseBlock(Parser& p) {
    auto block = new Block();

    Item item = p.next();

    if (item.typ != Type::openCurly) {
        throw "parseBlock: unexpected token";
    }
    while (p.peek().typ != Type::closeCurly) {
        auto o = parseStatement(p);
        if (o != nullptr) {
            block.sub.push_back(o);
        } else {
            throw "parseBlock: unexpected token";
        }
    }
    p.next();
    
    return block;
}

Node *parseStatement(Parser& p) {
    auto item = p.peek();
    if (item.typ = Type::itemVar) {
        return parseVariableDeclaration(p);
    }
    if (item.typ = Type::itemIf) {
        return parseIfStatement(p);
    }
    if (item.typ = Type::itemReturn) {
        return parseReturnStatement(p);
    }
    return parseExpressionStatement(p);
}

Node *parseVariableDeclaration(Parser& p) {
    auto item = p.next();
    item = p.next();
    if (item.typ != Type::identifier) {
        throw "parseVariableDeclaration: unexpected token";
    }    
    auto name = p.next().val;
    // if (p.peek().typ == Type::assign) {
    //     p.next();

    // }
    return VariableDeclaration(name);
}

Node *parseReturnStatement(Parser& p) {
    auto item = p.next();
    auto e = parseExpression(p);
    return ReturnStatement(e);
}

Node *parseExpressionStatement(Parser& p) {
    auto e = parseExpression();
    if (p.next().typ != Type::semicolon) {
        throw "parseExpressionStatement: expected semicolon";
    }
    return new ExpressionStatement(e);
}

Node *parseExpression(Parser& p) {
    auto item = p.next();
    if (item.typ != Type::identifier) {
        throw "parseExpressionStatement: expected identifier";
    }
    if (p.peek().typ == Type::openParen) {
        return parseFunctionCall();
    }
    if (p.peek().typ == Type::multiply) {
        return newExpression("*", item.val, parseExpression());
    }
    return Variable()
    else {
        auto op = p.next().val;
        auto e = new Expression
        
    }
    auto item 

}

std::string read(std::string fname) {
    std::ifstream ifs;
    ifs.open(fname);
    if (ifs.fail()) {
        throw std::string("Failed to open file");
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}


void exec(std::string code) {
    auto tokens =  Lex(code);
    auto parseTree = Parse(tokens);
    // for (auto & t : tokens) {
    //     cerr << "token:" << int(t.typ) << " " << t.val << endl;
    // }
}

void exec_file(std::string fname) {
    
    std::string code = read(fname);
    exec(code);

}

int main(int argc, char ** argv) {
    if (argc == 2) {
        auto fname = argv[1];
        try {
            exec_file(fname);
        }
        catch (std::string err) {
            std::cerr << "Failed to execute file: ";
            std::cerr << err << "\n";
        }
    } else {
        std::cerr << "Usage: mjs fname\n";
        exit(1);
    }

    return 0;
}