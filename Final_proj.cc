#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string>
#include <vector>
#include "compiler.h"
#include "lexer.h"

using namespace std;

// Global variables
LexicalAnalyzer* lexAnalyzer = new LexicalAnalyzer();
vector<InstructionNode*> instrList;
string symTable[1000];  // Symbol table for variable names and literals
InstructionNode* programStart;
InstructionNode* currentInstr;

// Declarations of utility functions
int findSymbolIndex(string symbol);
bool isValidPrimary(int tokenIndex);
bool isValidRelop(int tokenIndex);
bool isValidOperator(int tokenIndex);
bool canBeCondition(int tokenIndex);
bool canBeExpression(int tokenIndex);
vector<Token> extractCondition();
vector<Token> extractExpression();
vector<Token> buildAssignment();
void processOutputStatement();
void processInputStatement();
void manageWhileLoop();
void manageIfStatement();
void buildStatementList();
void determineAndProcessStatement();
void encapsulateBlock();
void constructProgram();
void manageCaseBlock(Token switchToken);
void handleSwitchStatement();
ConditionalOperatorType getConditionalOpType(string op);
void handleForLoop();
void gatherInputs();
void listVariableIds();
void setupVariableSection();


// Implementations of utility functions
int findSymbolIndex(string symbol) {
    for (int i = 0; i < 1000; i++) {
        if (symTable[i] == symbol) {
            return i;
        }
    }
    return -1;
}

bool isValidPrimary(int tokenIndex) {
    Token token = lexAnalyzer->peek(tokenIndex);
    return token.token_type == ID || token.token_type == NUM;
}

bool isValidRelop(int tokenIndex) {
    Token token = lexAnalyzer->peek(tokenIndex);
    return token.token_type == GREATER || token.token_type == LESS || token.token_type == NOTEQUAL;
}

bool isValidOperator(int tokenIndex) {
    Token token = lexAnalyzer->peek(tokenIndex);
    return token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV;
}

bool canBeCondition(int tokenIndex) {
    return isValidPrimary(tokenIndex) && isValidRelop(tokenIndex + 1) && isValidPrimary(tokenIndex + 2);
}

bool canBeExpression(int tokenIndex) {
    return isValidPrimary(tokenIndex) && isValidOperator(tokenIndex + 1) && isValidPrimary(tokenIndex + 2);
}

vector<Token> extractCondition() {
    vector<Token> conditionTokens;
    if (canBeCondition(1)) {
        conditionTokens.push_back(lexAnalyzer->GetToken());
        conditionTokens.push_back(lexAnalyzer->GetToken());
        conditionTokens.push_back(lexAnalyzer->GetToken());
    }
    return conditionTokens;
}

vector<Token> extractExpression() {
    vector<Token> expressionTokens;
    if (canBeExpression(1)) {
        expressionTokens.push_back(lexAnalyzer->GetToken());
        expressionTokens.push_back(lexAnalyzer->GetToken());
        expressionTokens.push_back(lexAnalyzer->GetToken());
    }
    return expressionTokens;
}

vector<Token> buildAssignment() {
    vector<Token> assignmentTokens;
    InstructionNode* assignmentNode = new InstructionNode();
    assignmentNode->type = ASSIGN;
    assignmentTokens.push_back(lexAnalyzer->GetToken());
    assignmentTokens.push_back(lexAnalyzer->GetToken());
    assignmentTokens.push_back(lexAnalyzer->GetToken());

    currentInstr->next = assignmentNode;
    assignmentNode->next = NULL;
    currentInstr = assignmentNode;
    return assignmentTokens;
}

void processOutputStatement() {
    lexAnalyzer->GetToken();  // Consume OUTPUT token
    InstructionNode* outputNode = new InstructionNode();
    outputNode->type = OUT;
    outputNode->output_inst.var_index = findSymbolIndex(lexAnalyzer->GetToken().lexeme);
    currentInstr->next = outputNode;
    outputNode->next = NULL;
    currentInstr = outputNode;
    lexAnalyzer->GetToken();  // Consume SEMICOLON
}

void processInputStatement() {
    lexAnalyzer->GetToken();  // Consume INPUT token
    InstructionNode* inputNode = new InstructionNode();
    inputNode->type = IN;
    inputNode->input_inst.var_index = findSymbolIndex(lexAnalyzer->GetToken().lexeme);
    currentInstr->next = inputNode;
    inputNode->next = NULL;
    currentInstr = inputNode;
    lexAnalyzer->GetToken();  // Consume SEMICOLON
}

void manageWhileLoop() {
    lexAnalyzer->GetToken();  // Consume WHILE token
    vector<Token> condition = extractCondition();
    // Logic to manage while loop follows
}

void manageIfStatement() {
    lexAnalyzer->GetToken();  // Consume IF token
    vector<Token> condition = extractCondition();
    // Logic to manage if statement follows
}

void buildStatementList() {
    while (lexAnalyzer->peek(1).token_type != END_OF_FILE) {
        if (lexAnalyzer->peek(1).token_type != RBRACE) {
            determineAndProcessStatement();
        } else {
            break;
        }
    }
}

void determineAndProcessStatement() {
    switch (lexAnalyzer->peek(1).token_type) {
        case WHILE: manageWhileLoop(); break;
        case IF: manageIfStatement(); break;
        case INPUT: processInputStatement(); break;
        case OUTPUT: processOutputStatement(); break;
        default: buildAssignment(); break;
    }
}

void encapsulateBlock() {
    lexAnalyzer->GetToken();  // Consume LBRACE
    buildStatementList();
    lexAnalyzer->GetToken();  // Consume RBRACE
}

// Additional functions for handling switch cases, for loops, etc.

void constructProgram() {
    InstructionNode* startNode = new InstructionNode();
    startNode->type = NOOP;
    currentInstr = startNode;
    programStart = currentInstr;
    setupVariableSection();
    encapsulateBlock();
    gatherInputs();
}

struct InstructionNode* generateIntermediateRepresentation() {
    constructProgram();
    return programStart;
}
