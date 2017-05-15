
#include "mjs.h"
#include "lexer.h"

using namespace std;





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