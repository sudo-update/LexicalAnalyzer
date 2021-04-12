// Syntax Analyzer
#pragma once
#include <list>
#include <vector>
#include "lexicalscanner.h"
#include "parsetree.h"
#include "symboltable.h"

using namespace std;

class SyntaxAnalyzer {
protected:
    vector<Record> lexemes;
    int currentLexeme;
    int previousLexeme;
    bool printProduction = true;
    string currentProduction = "";
    LexicalScanner & lexicalScanner;
    ErrorHandler & errorHandler;
    SymbolTable & symbolTable;
public:

    SyntaxAnalyzer(LexicalScanner & lexicalScanner, SymbolTable & symbolTable, ErrorHandler & errorHandler) 
    : lexicalScanner(lexicalScanner), symbolTable(symbolTable), lexemes(), currentLexeme(0), errorHandler(errorHandler) {

    }

    Record * getNextToken() {
        if (currentLexeme < lexemes.size()) {
            Record & token = lexemes[currentLexeme++];
            return &token;
        } else {
            Record token = lexicalScanner.lexer();

            if (token.accepted && token.lexeme.length() > 0) {
                cout << token << endl;
                lexemes.push_back(token);
                Record & token = lexemes[currentLexeme++];
                return &token;
            }
            return NULL; // maybe this should throw an exception instead?
        }
    }

        Record * lookAhead() {
        Record * token = getNextToken();
        backup();
        return token;
    }

    Record * getCurrentToken() {
        if (currentLexeme > 0) {
            return &lexemes[currentLexeme-1];
        } else {
            return nullptr;
        }
    }

    void backup() {
        if (currentLexeme != 0) {
            currentLexeme--;
        }
    }

    void print(const string & rule) {
        if (printProduction) {
            cout << "     " << rule << endl;
        }
    }

    virtual ParseTree * createParseTree() = 0;
    bool isIf(const Record & lexeme);
    bool isWhile(const Record & lexeme);
    bool isType(const Record & lexeme);
    bool isId(const Record & lexeme);
    bool isNumber(const Record & lexeme);

    vector<Record> & getTokenList() {
        return lexemes;
    }
};
