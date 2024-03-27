#pragma once
#include "common.h"

class cfgReader{

public:
    cfgReader(std::string cfgFile, std::string terminalsFile);
    cfgReader();

private:
    std::string cfgFile;
    std::string terminalsFile;

    //      <TokenID,       (Token, terminal?, nullable?)>
    std::map<int, std::tuple<std::string, bool, bool>> tokenNumbers;

    //                      <LHS, RHS>
    std::vector<std::tuple<int, std::vector<int>>> cfgRules;

    inline bool getNullable(int i){
        return std::get<2>(tokenNumbers.find(i)->second);
    }

    inline void makeNotNull(int i){
        tokenNumbers[std::get<0>(cfgRules[i])] = std::make_tuple(std::get<0>(tokenNumbers.find(std::get<0>(cfgRules[i]))->second),
                                                                 std::get<1>(tokenNumbers.find(std::get<0>(cfgRules[i]))->second), false);
    }

    //Small helper function to find map entries by value
    inline int findByValue(std::string value){
        for(size_t i = 0; i < tokenNumbers.size(); i++) if(std::get<0>(tokenNumbers.find(i)->second) == value) return i;
        return -1;
    }

    void populateTokenNumbersAndRules();

    void determineNullables();

    #ifdef DEBUG
    void printDebugStuff();
    #endif // DEBUG

    void parseCfg();
};
