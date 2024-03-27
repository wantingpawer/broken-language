#include "common.h"

int main(int argc, char* argv[]){

    for(int i = 0; i < argc - 1; i++){

        std::cout << "Now lexing " << argv[i+1] << std::endl;
        LexerList* lexList = lex(argv[i+1]);
        lexList->printList();

        #ifdef DEBUG
        lexList->printList();
        #endif // DEBUG

        delete lexList;
    }

    cfgReader *cfg = new cfgReader("cfg.txt", "terminals");

    return 0;
}
