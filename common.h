#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <exception>
#include <map>

#include "lexer.h"
#include "cfgReader.h"

#define DEFAULT_COMMENT_VALUE "//"
#define MULTILINE_COMMENT_START "/*"
#define MULTILINE_COMMENT_END "*/"
#define DEFAULT_KEYWORDS_FILE "keywords.txt"
#define DEFAULT_SYMBOLS_FILE "symbols.txt"

#define DEFAULT_INT_TYPE 1
#define DEFAULT_FLOAT_TYPE 2
#define DEFAULT_KEYWORD_TYPE 3
#define DEFAULT_IDENTIFIER_TYPE 4
#define DEFAULT_STRING_TYPE 5
#define DEFAULT_SYMBOL_TYPE 6

#define DEBUG
