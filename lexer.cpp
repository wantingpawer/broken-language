#include "common.h"

#ifdef DEBUG //Only include this code if the DEBUG preprocessor constant is set

//Print the list of current tokens
void LexerList::printList(){
        LexerToken *currentToken = head;
        while(currentToken != tail){
            std::cout << ((currentToken->token == "\n") ? "\\n" : currentToken->token) << " " << currentToken->length << " " << currentToken->lineNumber << " " << currentToken->type << std:: endl;
            currentToken = currentToken->next;
        }
        std::cout << ((currentToken->token == "\n") ? "\\n" : currentToken->token) << " " << currentToken->length << " " << currentToken->lineNumber << " " << currentToken->type << std:: endl;
    }
#endif //DEBUG

void LexerList::createToken(std::string token, size_t length, int lineNumber, int type){

        //Create a new token to add to the list
        LexerToken *newToken = new LexerToken;

        //Set token attributes
        newToken->token = token;
        newToken->length = length;
        newToken->lineNumber = lineNumber;
        newToken->type = type;

        //If there's no head (i.e. this is the first token), set this to be both the first and last token
        if(head == nullptr){
            head = newToken;
            tail = newToken;
            return;
        }

        //Set the last tokens next pointer to point to the new token, and set the new token to be the tail
        tail->next = newToken;
        tail = newToken;
    }

LexerList::LexerList(std::string keywordsFile, std::string symbolsFile){

    //Set the head and tail of the lexer token list to be null
    head = nullptr;
    tail = nullptr;

    //Open the keywords and symbols files
    std::ifstream keywords (keywordsFile);
    std::ifstream symbols (symbolsFile);

    //If either file couldn't be opened, throw an error
    if(!keywords.is_open() || !symbols.is_open()) throw std::invalid_argument("Invalid/missing keywords and/or symbols file");

    std::string buffer;

    //Add every keyword to the keyword vector
    while(keywords.peek() != EOF){
        std::getline(keywords, buffer);
        this->keywords.push_back(buffer);
    }

    //Add every symbol to the symbol vector
    while(symbols.peek() != EOF){
        std::getline(symbols, buffer);
        this->symbols.push_back(buffer);
    }
}

LexerList* lex(std::string file){
    //Open file
    std::ifstream source (file);
    std::string buffer;

    LexerList *lexList = new LexerList(DEFAULT_KEYWORDS_FILE, DEFAULT_SYMBOLS_FILE);

    if(!source.is_open()) { std::cerr << "Couldn't open file " << file << std::endl; throw std::invalid_argument("Unable to open file");}

    //Remove comments from source code
    while(source.peek() != EOF){

        //Get a character
        char current = source.get();

        //If the character + next character result in a single line comment, skip till the next newline
        if(current == DEFAULT_COMMENT_VALUE[0] && source.peek() == DEFAULT_COMMENT_VALUE[1]){
            while(source.get() != '\n');
            buffer += '\n'; //Emit the newline that was skipped
            continue;
        }

        //If char + next char is a multiline comment, skip until the multiline comment ends
        if(current == MULTILINE_COMMENT_START[0] && source.peek() == MULTILINE_COMMENT_START[1]){
            while(!(source.get() == MULTILINE_COMMENT_END[0] && source.peek() == MULTILINE_COMMENT_END[1]));
            source.ignore(); //Ignore the second character for multiline end
            continue;
        }

        //Emit the current value if it's not contained within a comment
        buffer += current;
    }

    int lineNumber = 1;

    std::string lexeme;
    int type = 0;

    //For each character in the buffer
    for(size_t i = 0; i < buffer.size(); i++){
        char c = buffer[i];

        //Determine type if not already determined
        if(type == 0){

            //If newline increment line number
            if(c == '\n') lineNumber++;

            //Ignore whitespace
            if(c == '\n' || c == ' ' || c == '\t') continue;

            //Integer/float
            else if(c >= '0' && c <= '9') type = 1;

            //Keyword/identifier
            else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) type = 3;

            //String
            else if(c == '"') type = 5;

            //Symbol
            else type = 6;
        }

        switch(type){

            //If lexeme is integer
            case 1:

                //If still a number
                if(c >= '0' && c <= '9') lexeme += c;

                //If there's a decimal point, int becomes floating point
                else if(c == '.'){lexeme += c; type = 2;}

                else{
                    //Emit lexeme
                    lexList->addToken(lexeme, lineNumber, DEFAULT_INT_TYPE);
                    if(c != '\n' && c != ' ') i--; //Only skip character if it's whitespace
                    type = 0;
                    lexeme = "";
                }
                break;

            //If lexeme is floating point
            case 2:

                //If still a number
                if(c >= '0' && c <= '9') lexeme += c;

                //A number with two decimal points is invalid
                else if(c == '.'){std::cerr << "Invalid integer at line " << lineNumber << std::endl; throw std::invalid_argument("Invalid integer");}

                else{
                    lexList->addToken(lexeme, lineNumber, DEFAULT_FLOAT_TYPE); //Emit lexeme
                    if(c != '\n' && c != ' ') i--;
                    type = 0;
                    lexeme = "";
                }
                break;

            //If lexeme is a keyword/identifier
            case 3:

                //If character is an uppercase or lowercase letter or a digit
                if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') lexeme += c;
                else{
                    //Identify whether lexeme is keyword or identifier and emit corresponding lexeme
                    lexList->addToken(lexeme, lineNumber, (lexList->checkKeyword(lexeme) ? DEFAULT_KEYWORD_TYPE : DEFAULT_IDENTIFIER_TYPE));
                    type = 0;
                    lexeme = "";
                    if(c != '\n' && c != ' ') i--;
                }
                break;

            //If lexeme is a string
            case 5:
                lexeme += c;
                if(lexeme != "\"" && c == '"'){
                    lexList->addToken(lexeme, lineNumber, DEFAULT_STRING_TYPE);
                    type = 0;
                    lexeme = "";
                }
                break;

            //If lexeme is a symbol
            case 6:
                lexeme += c;
                if(lexList->checkSymbol(lexeme)){
                    lexList->addToken(lexeme, lineNumber, DEFAULT_SYMBOL_TYPE);
                    type = 0;
                    lexeme = "";
                }
                break;
        }

        //If newline increment line number
        if(c == '\n') lineNumber++;
    }

    //This covers edgecase where last token is a keyword or identifier and makes sure it's added as a lexeme
    if(type == 3){
        lexList->addToken(lexeme, lineNumber, (lexList->checkKeyword(lexeme) ? DEFAULT_KEYWORD_TYPE : DEFAULT_IDENTIFIER_TYPE));
        lexeme = "";
    }

    if(lexeme != "") { std::cerr << "Lexeme not fully parsed: " << lexeme; throw std::invalid_argument("Lexeme not fully parsed"); }

    source.close();

    return lexList;
}
