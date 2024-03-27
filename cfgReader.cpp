#include "common.h"

void cfgReader::parseCfg(){
    populateTokenNumbersAndRules();
    determineNullables();

    #ifdef DEBUG
    printDebugStuff();
    #endif // DEBUG
}

void cfgReader::determineNullables(){
    std::ifstream source(this->terminalsFile);
    std::string buffer;

    int word;
    bool null;

    while(getline(source, buffer)){
        std::string temp;

        std::cout << "BUFFER: " << buffer << std::endl;

        std::istringstream iss(buffer);
        getline(iss, temp, ',');

        word = findByValue(temp);
        getline(iss, temp);

        null = temp == "T" ? true : false;

        if(word != -1) tokenNumbers[word] = std::make_tuple(std::get<0>(tokenNumbers.find(word)->second), true, null);
    }

    for(size_t i = 0; i < cfgRules.size(); i++){
        bool nullable = false;
        for(size_t j = 0; j < std::get<1>(cfgRules[i]).size(); j++) if(getNullable(std::get<1>(cfgRules[i])[j])) nullable = true;
        if(!nullable) makeNotNull(i);
    }
}

void cfgReader::populateTokenNumbersAndRules(){
    std::ifstream source(this->cfgFile);
    std::string buffer;

    if(!source.is_open()) { std::cerr << "Unable to find grammar file: " << cfgFile << std::endl; throw std::invalid_argument("Unable to find grammar");}

    while(source.peek() != EOF){
        std::string line;
        std::getline(source, line);

        std::string word = "";
        bool identifying = false;

        //Start on left hand side
        bool LHS = true;
        int interimLHS;
        std::vector<int> interimRHS = {};

        char c;

        for(size_t i = 0; i < line.size(); i++){
            c = line[i];

            //If done identifying last token and just passed ::= to indicate start of right hand side
            if(identifying == false && LHS && c == '=' && line[i-1] == ':' && line[i-2] == ':') LHS = false;

            //If at the start of something that makes up a token
            if((c == '<' || c == '"') && identifying == false){
                identifying = true;
                continue;
            }

            //Whilst identifying a token
            if(identifying){

                //Once at the end of the token
                if(c == '>' || c == '"'){
                    identifying = false;

                    //Long complicated line that basically just inserts the token into tokenNumbers
                    if(findByValue(word) == -1) tokenNumbers.insert(std::pair<int, std::tuple<std::string, bool, bool>>(tokenNumbers.size(), std::make_tuple(word, c == '"', c != '"')));

                    #ifdef DEBUG
                    std::cout << tokenNumbers.find(findByValue(word))->first << " " << std::get<0>(tokenNumbers.find(findByValue(word))->second) << std::endl;
                    #endif // DEBUG

                    //If on left hand side
                    if(LHS) interimLHS = tokenNumbers.find(findByValue(word))->first;

                    //If on right hand side
                    else interimRHS.push_back(tokenNumbers.find(findByValue(word))->first);

                    word = "";
                    continue;
                }
                word += c;
            }
        }

        cfgRules.push_back(std::make_tuple(interimLHS, interimRHS));
    }
}

#ifdef DEBUG
void cfgReader::printDebugStuff(){
    std::cout << "--DEBUG STUFF--" << std::endl;
    for(size_t i = 0; i < tokenNumbers.size(); i++){
        std::cout << "TOKEN ID: " << tokenNumbers.find(i)->first << " IS TERM?: " << std::get<1>(tokenNumbers.find(i)->second)
                    << " IS NULLABLE? " << std::get<2>(tokenNumbers.find(i)->second) <<  std::endl;
    }

    for(size_t i = 0; i < cfgRules.size(); i++){
        std::cout << "LHS: " << std::get<0>(cfgRules[i]);

        for(size_t j = 0; j < std::get<1>(cfgRules[i]).size(); j++)
         std::cout << "\tRHS: " << std::get<1>(cfgRules[i])[j] << std::endl;
    }
}
#endif // DEBUG

cfgReader::cfgReader(std::string cfgFile, std::string terminalsFile){
    this->terminalsFile = terminalsFile;
    this->cfgFile = cfgFile;
    this->parseCfg();
}
