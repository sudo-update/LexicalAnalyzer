#include "topdownsyntaxanalyzer.h"
#include "parsetree.h"
#include <iostream>
using namespace std;

ParseTree * TopDownSyntaxAnalyzer::createParseTree() {

    parseTree = new ParseTree();

    bool hasErrors = false;
    while(!hasErrors) {
        currentNode = parseTree->getRoot(); // always add statements to the root node
        if (lexicalScanner.isFinished())
            break;

        hasErrors = !isStatement();
        if (!hasErrors)
            cout << "processed statement successfully" << endl;
        else cout << "processing statement with failure" << endl;
    }

    cout << "-------Parse Tree Code------------------" << endl;
    parseTree->printNodes(false);
    cout << "-------Parse Tree Rules------------------" << endl;
    parseTree->printRules();
    cout << "-------Parse Tree -----------------------" << endl;
    parseTree->printTree();
    return parseTree;
}

bool TopDownSyntaxAnalyzer::isStatement() {
    Node * parent = startNonTerminal("<Statement> -> <Assign> | <Declaration>");
    Record * nextToken = getNextToken();
    if (nextToken == nullptr) return false;
    backup();

    if(isType(*nextToken) && isDeclaration()) {
        finishNonTerminal(parent);
        return true;
    }
    if(isId(*nextToken) && isAssignment()) {
        finishNonTerminal(parent);
        return true;
    }
    if(isWhile(*nextToken) && isWhileTopDown()) {
        finishNonTerminal(parent);
        return true;
    }
    if(isIf(*nextToken) && isIfTopDown()) {
        finishNonTerminal(parent);
        return true;
    }
    backup();
    cancelNonTerminal(parent);
    return false;
}
bool TopDownSyntaxAnalyzer::isNumberTopDown() {
  Record * record = getNextToken();
  Node * parent = startNonTerminal("<NUM> -> number");
  if (record == nullptr)
    return false;
  if (isNumber(*record)) {
    currentNode->add(new Node(*record));
    finishNonTerminal(parent);
    return true;
  }
  cancelNonTerminal(parent);
  return false;
}
bool TopDownSyntaxAnalyzer::isWhileTopDown() {
  print("<while statement> -> <WHILE>  <Conditional> <Do> <StatementList> <WhileEnd|EndDo> || <While> <Conditional> { <StatementList> }");
  Record * record = getNextToken();
  Node * parent = startNonTerminal("<while statement> -> <WHILE>  <Conditional> <Do> <StatementList> <WhileEnd|EndDo> || <While> <Conditional> { <StatementList> }");
  if (record == nullptr) {return false;}
  if (isWhile(*record)) {
    /*make sure we do getnextToken in isConditionalTopDown function*/
    if (isConditionalTopDown()) {
      Record * record = getNextToken();
      // we have verified while-conditional-...
      if (isDo(*record)) { //first variation do-statements-enddo/whileend
        //make sure we implement isStatementList using getNextToken
        if (isStatementList()) {
          Record * record = getNextToken();
          if (isEndDo(*record) || isWhileEnd(*record)) {
            currentNode->add(new Node(*record));
            print("<while statement> -> <WHILE>  <Conditional> <Do> <StatementList> <WhileEnd|EndDo>");
            finishNonTerminal(parent);
            return true;
          }else {backup();}
          currentNode->add(new Node(*record));
        }
        currentNode->add(new Node(*record));
      }else {backup();}
      if (isOpenBracket(*record)) {//second variation { <StatementList> }
        if(isStatementList()) {
          Record * record = getNextToken();
          if(isCloseBracket(*record)) {
            currentNode->add(new Node(*record));
            print("while statement -> <While> <Conditional> { <StatementList> }");
            currentNode->add(new Node(*record));
            finishNonTerminal(parent);
            return true;
          }else {backup();}
          currentNode->add(new Node(*record));
        }
        currentNode->add(new Node(*record));
      }else {backup();}
      currentNode->add(new Node(*record));
    }
    currentNode->add(new Node(*record));
  }
  backup();
  cancelNonTerminal(parent);
  return false;
}
bool TopDownSyntaxAnalyzer::isIfTopDown() {
  print("<if statement> -> <IF> <Conditional> <Do|Then> <StatementList> <EndDo|EndIf> || <IF> <Conditional> { <StatementList> }");
  Record * record = getNextToken();
  Node * parent = startNonTerminal("<if statement> -> <IF> <Conditional> <Do|Then> <StatementList> <EndDo|EndIf> || <IF> <Conditional> { <StatementList> }");
  if (record == nullptr) {return false;}
  if (isIf(*record)) {
    if (isConditionalTopDown()) {
      Record * record = getNextToken();
      // we have verified if-conditional-...
      if (isDo(*record)||isThen(*record)) { //first variation do/then-statements-enddo/endif
        //make sure we implement isStatementList using getNextToken
        if (isStatementList()) {
          Record * record = getNextToken();
          if (isEndDo(*record) || isEndIf(*record)) {
            currentNode->add(new Node(*record));
            print("<if statement> -> <IF> <Conditional> <Do|Then> <StatementList> <EndDo|EndIf>");
            finishNonTerminal(parent);
            return true;
          }else {backup();}
          currentNode->add(new Node(*record));
        }
        currentNode->add(new Node(*record));
      }else {backup();}
      if (isOpenBracket(*record)) {//second variation { <StatementList> }
        if(isStatementList()) {
          Record * record = getNextToken();
          if(isCloseBracket(*record)) {
            currentNode->add(new Node(*record));
            print("if statement -> <IF> <Conditional> { <StatementList> }");
            currentNode->add(new Node(*record));
            finishNonTerminal(parent);
            return true;
          }else {backup();}
          currentNode->add(new Node(*record));
        }
        currentNode->add(new Node(*record));
      }else {backup();}
      currentNode->add(new Node(*record));
    }
    currentNode->add(new Node(*record));
  }
  backup();
  cancelNonTerminal(parent);
  return false;
}
bool TopDownSyntaxAnalyzer::isStatementList() {
  Node * parent = startNonTerminal("<StatementList> -> <Statement> <MoreStatements>");
  if(isStatement()) {
    if(isMoreStatements()){
      print("<StatementList> -> <Statement> <MoreStatements>");
      finishNonTerminal(parent);
      return true;
    }
  }
  backup();
  cancelNonTerminal(parent);
  return false;
}
bool TopDownSyntaxAnalyzer::isMoreStatements() {
  Node * parent = startNonTerminal("<MoreStatements> -> ; <Statement> <MoreStatements> | epsilon");
  Record * record = getNextToken();
  if(isSemiColon(*record)) {
    if (isStatement()) {
      if(isMoreStatements()) {
        print("<MoreStatements> -> ; <Statement> <MoreStatements>");
        finishNonTerminal(parent);
        return true;
      }
      // else {
      //   Record * record = getNextToken();
      //   if (isSemiColon(*record)) {
      //     print("MoreStatements -> ; <Statement> <MoreStatements>");
      //     currentNode->add(new Node(*record));
      //     finishNonTerminal(parent);
      //     return true;
      //   }
      //  else {backup();}
      // }
    }
    print("<MoreStatements> -> epsilon");
    currentNode->add(new Node(*record));
    finishNonTerminal(parent);
    return true; //just ; is accepted
  }
  backup();
  cancelNonTerminal(parent);
  return false;
}
bool TopDownSyntaxAnalyzer::isConditionalTopDown() {
    Node * parent = startNonTerminal("<Conditional> -> <Expression> <operator> <Expression>");
    Record * record = getNextToken();
    if (isE()) {
        Record * token = getNextToken();
        if (token == nullptr) return false;
        if (isOperator(*token)) {
            currentNode->add(new Node(token));
            if (isE()) {
                finishNonTerminal(parent);
                return true;
            }
        }
        else {backup();}
    }
    backup();
    cancelNonTerminal(parent);
    return false;
}
bool TopDownSyntaxAnalyzer::isDeclaration() {
    print("<Declaration> -> <Type><ID>");
    print("<Type> -> float | int | bool");
    print("<ID> -> identifier");
    Node * declarationNode = startNonTerminal("<Declaration> -> <Type><ID>");
    if (isTypeTopDown()) {
        if (isIdentifier()) {
            // Record * record = getNextToken();
            // if (record == nullptr)
            //     return false;
            // if (record->lexeme == ";") {
                currentNode->add(new Node(record));
                print("<Declaration> -> <Type><ID>");
                finishNonTerminal(declarationNode);
                return true; //success / processed
            //}
        }
    }
    cancelNonTerminal(declarationNode);
    return false;
}
bool TopDownSyntaxAnalyzer::isTypeTopDown() {
  Record * record = getNextToken();
  Node * parent = startNonTerminal("<Type> -> float | int | bool");
  if (record == nullptr)
    return false;
  if (isType(*record)) {
    currentNode->add(new Node(*record));
    finishNonTerminal(parent);
    return true;
  }
  cancelNonTerminal(parent);
  return false;
}

/**
 * Determines if the next token is an identifier   -- (Sean) changed this to match ^^ isTypeTopDown
 */
bool TopDownSyntaxAnalyzer::isIdentifier() {
    Record * record = getNextToken();
    Node * parent = startNonTerminal("<ID> -> identifier");
    if (record == nullptr) {return false;}
    if(isId(*record)) {
      currentNode->add(new Node(*record));
      finishNonTerminal(parent);
      return true;
    }
    cancelNonTerminal(parent);
    return false;
}

Node * TopDownSyntaxAnalyzer::startNonTerminal(const string & name) {
    Node * parent = currentNode;
    currentNode = new Node(name);
    return parent;
}

void TopDownSyntaxAnalyzer::finishNonTerminal(Node * parent) {
    parent->add(currentNode);
    currentNode = parent;
}

void TopDownSyntaxAnalyzer::cancelNonTerminal(Node * parent) {
    delete currentNode;
    currentNode = parent;
}

bool TopDownSyntaxAnalyzer::isQ(){
    Record * record = getNextToken();
    if (record == nullptr)
        return false;
    Node * parent = startNonTerminal("<ExpressionPrime> -> +<Term><ExpressionPrime> | -<Term><ExpressionPrime> | epsilon"/*, *record*/);

    if (record->lexeme == "+") {
        currentNode->add(new Node(*record));
        if (isT()) {
            if (isQ()) {
                //cout << " Q -> +TQ" << endl;
                print("<ExpressionPrime> -> +<Term><ExpressionPrime>");
                finishNonTerminal(parent);
                return true;
            }
        }
    } else if (record->lexeme == "-") {
        currentNode->add(new Node(*record));
        if (isT()) {
            if (isQ()) {
                //cout << " Q -> +TQ" << endl;
                print("<ExpressionPrime> -> -<Term><ExpressionPrime>");
                finishNonTerminal(parent);
                return true;
            }
        }
    } else if (record->lexeme == ")" || record->lexeme == ";") {
        backup();
        //cout << " Q -> epsilon" << endl;
        print("<ExpressionPrime> -> epsilon");
        finishNonTerminal(parent);
        return true;

    }
    cancelNonTerminal(parent);
    return false;
}

bool TopDownSyntaxAnalyzer::isT() {
    //print(" <Term> -> <Factor><TermPrime>");
    Node * parent = startNonTerminal("<Term> -> <Factor><TermPrime>");
    if (isF()) {
        if (isR()) {
            //cout << " T -> FR" << endl;
            print("<Term> -> <Factor><TermPrime>");
            finishNonTerminal(parent);
            return true;
        }
    }
    cancelNonTerminal(parent);
    return false;
}

bool TopDownSyntaxAnalyzer::isR() {
    Record * record = getNextToken();
    if (record == nullptr)
        return false;

    Node * parent = startNonTerminal("<TermPrime> -> *<Factor><TermPrime> | /<Factor><TermPrime> | epsilon");

    if (record->lexeme == "*") {
        currentNode->add(new Node(*record));
        if (isF()) {
            if (isR()) {
                //cout << *record << endl;
                //cout << " R -> *FR" << endl;
                print("<TermPrime> -> *<Factor><TermPrime>");
                finishNonTerminal(parent);
                return true;
            }
        }
    } else if(record->lexeme == "/") {
        currentNode->add(new Node(*record));
        if (isF()) {
            if (isR()) {
                print("<TermPrime> -> /<Factor><TermPrime>");
                finishNonTerminal(parent);
                return true;
            }
        }
    } else if (record->lexeme == "+" || record->lexeme == "-" || record->lexeme == ")" || record->lexeme == ";") {
        //cout << *record << endl;
        print("<TermPrime> -> epsilon");
        backup();
        finishNonTerminal(parent);
        return true;
    }
    cancelNonTerminal(parent);
    return false;
}

bool TopDownSyntaxAnalyzer::isF() {
    Record * record = getNextToken();
    if (record == nullptr)
        return false;
    backup();
    Node * parent = startNonTerminal("<Factor> -> (<Expression>) | <ID> | <NUM>");
    //print(" <Factor> -> <Identifier>");
    if (isId(*record) && isIdentifier()) {
        //cout << *record << endl;
        //cout << " F -> id" << endl;
        print("<Factor> -> <Identifier>");
        finishNonTerminal(parent);
        return true;
    }
    if (isNumber(*record) && isNumberTopDown()) {
      print("<Factor> -> <NUM>");
      finishNonTerminal(parent);
      return true;
    }
    if (record->lexeme == "(") {
            getNextToken();
            currentNode->add(new Node(*record));
            if (isE()) {
                Record * record = getNextToken();
                if (record == nullptr)
                    return false;

                if (record->lexeme == ")") {
                    currentNode->add(new Node(*record));
                    //cout << " F -> (E)" << endl;
                    print("<Factor> -> (<Expression>)");
                    finishNonTerminal(parent);
                    return true;
                }
            }
    }

    cancelNonTerminal(parent);
    return false;
}

bool TopDownSyntaxAnalyzer::isE() {
    //print(" <Expression> -> <Term><ExpressionPrime>");
    //cout << *currentLexeme << endl;
    Node * parent = startNonTerminal("<Expression> -> <Term><ExpressionPrime>");
    if (isT()) {
        if (isQ()) {
            /*Record * record = getNextToken();
            if (record == nullptr)
                return false;

            // the ; optionally ends a statement
            if (record->lexeme != ";")
                backup();
*/
            //cout << " E -> TQ" << endl;
            print("<Expression> -> <Term><ExpressionPrime>");
            finishNonTerminal(parent);
            return true;
        }
    }
    cancelNonTerminal(parent);
    return false;
}

bool TopDownSyntaxAnalyzer::isAssignment() {
    //cout << *currentLexeme << endl;
    //print(" <Assign> -> <ID> = <Expression>");
    Node * parent = startNonTerminal("<Assign> -> <ID> = <Expression>");
    if (isIdentifier()) {
        Record * record = getNextToken();
        if (record == nullptr)
            return false;
        //cout << * record << endl;
        if (record->lexeme == "=") {
            currentNode->add(new Node(*record));
            if(isE()) {
                // Record * record = getNextToken();
                // if (record == nullptr)
                //     return false;
                // cout << * record << endl;
                // // the ; optionally ends a statement
                // if (record->lexeme != ";")
                //     backup();
                //
                // //cout << " <Assign> -> <ID> = <Expression>;" << endl;
                // //print("<Assign> -> <ID> = <Expression>");
                // currentNode->add(new Node(*record));
                finishNonTerminal(parent);
                return true;
            }
        }
    }
    cancelNonTerminal(parent);
    return false;
}
