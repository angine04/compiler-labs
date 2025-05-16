
// Generated from /Users/zheyu/Repositories/exp04-minic-expr/frontend/antlr4/MiniC.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  MiniCParser : public antlr4::Parser {
public:
  enum {
    T_L_PAREN = 1, T_R_PAREN = 2, T_SEMICOLON = 3, T_L_BRACE = 4, T_R_BRACE = 5, 
    T_ASSIGN = 6, T_COMMA = 7, T_ADD = 8, T_SUB = 9, T_MUL = 10, T_DIV = 11, 
    T_MOD = 12, T_RETURN = 13, T_INT = 14, T_ID = 15, T_HEX_LITERAL = 16, 
    T_OCT_LITERAL = 17, T_DEC_LITERAL = 18, WS = 19
  };

  enum {
    RuleCompileUnit = 0, RuleFuncDef = 1, RuleBlock = 2, RuleBlockItemList = 3, 
    RuleBlockItem = 4, RuleVarDecl = 5, RuleBasicType = 6, RuleVarDef = 7, 
    RuleStatement = 8, RuleExpr = 9, RuleAddExpr = 10, RuleMulExpr = 11, 
    RuleUnaryExpr = 12, RulePrimaryExpr = 13, RuleIntegerLiteral = 14, RuleRealParamList = 15, 
    RuleLVal = 16
  };

  explicit MiniCParser(antlr4::TokenStream *input);

  MiniCParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~MiniCParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class CompileUnitContext;
  class FuncDefContext;
  class BlockContext;
  class BlockItemListContext;
  class BlockItemContext;
  class VarDeclContext;
  class BasicTypeContext;
  class VarDefContext;
  class StatementContext;
  class ExprContext;
  class AddExprContext;
  class MulExprContext;
  class UnaryExprContext;
  class PrimaryExprContext;
  class IntegerLiteralContext;
  class RealParamListContext;
  class LValContext; 

  class  CompileUnitContext : public antlr4::ParserRuleContext {
  public:
    CompileUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<FuncDefContext *> funcDef();
    FuncDefContext* funcDef(size_t i);
    std::vector<VarDeclContext *> varDecl();
    VarDeclContext* varDecl(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CompileUnitContext* compileUnit();

  class  FuncDefContext : public antlr4::ParserRuleContext {
  public:
    FuncDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_INT();
    antlr4::tree::TerminalNode *T_ID();
    antlr4::tree::TerminalNode *T_L_PAREN();
    antlr4::tree::TerminalNode *T_R_PAREN();
    BlockContext *block();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FuncDefContext* funcDef();

  class  BlockContext : public antlr4::ParserRuleContext {
  public:
    BlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_L_BRACE();
    antlr4::tree::TerminalNode *T_R_BRACE();
    BlockItemListContext *blockItemList();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockContext* block();

  class  BlockItemListContext : public antlr4::ParserRuleContext {
  public:
    BlockItemListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<BlockItemContext *> blockItem();
    BlockItemContext* blockItem(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockItemListContext* blockItemList();

  class  BlockItemContext : public antlr4::ParserRuleContext {
  public:
    BlockItemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    StatementContext *statement();
    VarDeclContext *varDecl();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlockItemContext* blockItem();

  class  VarDeclContext : public antlr4::ParserRuleContext {
  public:
    VarDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BasicTypeContext *basicType();
    std::vector<VarDefContext *> varDef();
    VarDefContext* varDef(size_t i);
    antlr4::tree::TerminalNode *T_SEMICOLON();
    std::vector<antlr4::tree::TerminalNode *> T_COMMA();
    antlr4::tree::TerminalNode* T_COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarDeclContext* varDecl();

  class  BasicTypeContext : public antlr4::ParserRuleContext {
  public:
    BasicTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_INT();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BasicTypeContext* basicType();

  class  VarDefContext : public antlr4::ParserRuleContext {
  public:
    VarDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_ID();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VarDefContext* varDef();

  class  StatementContext : public antlr4::ParserRuleContext {
  public:
    StatementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    StatementContext() = default;
    void copyFrom(StatementContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  BlockStatementContext : public StatementContext {
  public:
    BlockStatementContext(StatementContext *ctx);

    BlockContext *block();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AssignStatementContext : public StatementContext {
  public:
    AssignStatementContext(StatementContext *ctx);

    LValContext *lVal();
    antlr4::tree::TerminalNode *T_ASSIGN();
    ExprContext *expr();
    antlr4::tree::TerminalNode *T_SEMICOLON();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ExpressionStatementContext : public StatementContext {
  public:
    ExpressionStatementContext(StatementContext *ctx);

    antlr4::tree::TerminalNode *T_SEMICOLON();
    ExprContext *expr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ReturnStatementContext : public StatementContext {
  public:
    ReturnStatementContext(StatementContext *ctx);

    antlr4::tree::TerminalNode *T_RETURN();
    ExprContext *expr();
    antlr4::tree::TerminalNode *T_SEMICOLON();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  StatementContext* statement();

  class  ExprContext : public antlr4::ParserRuleContext {
  public:
    ExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AddExprContext *addExpr();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExprContext* expr();

  class  AddExprContext : public antlr4::ParserRuleContext {
  public:
    AddExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    AddExprContext() = default;
    void copyFrom(AddExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PassToMulExprContext : public AddExprContext {
  public:
    PassToMulExprContext(AddExprContext *ctx);

    MulExprContext *mulExpr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  AddSubExprContext : public AddExprContext {
  public:
    AddSubExprContext(AddExprContext *ctx);

    AddExprContext *addExpr();
    MulExprContext *mulExpr();
    antlr4::tree::TerminalNode *T_ADD();
    antlr4::tree::TerminalNode *T_SUB();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  AddExprContext* addExpr();
  AddExprContext* addExpr(int precedence);
  class  MulExprContext : public antlr4::ParserRuleContext {
  public:
    MulExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    MulExprContext() = default;
    void copyFrom(MulExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PassToUnaryExprContext : public MulExprContext {
  public:
    PassToUnaryExprContext(MulExprContext *ctx);

    UnaryExprContext *unaryExpr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  MulDivModExprContext : public MulExprContext {
  public:
    MulDivModExprContext(MulExprContext *ctx);

    MulExprContext *mulExpr();
    UnaryExprContext *unaryExpr();
    antlr4::tree::TerminalNode *T_MUL();
    antlr4::tree::TerminalNode *T_DIV();
    antlr4::tree::TerminalNode *T_MOD();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  MulExprContext* mulExpr();
  MulExprContext* mulExpr(int precedence);
  class  UnaryExprContext : public antlr4::ParserRuleContext {
  public:
    UnaryExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    UnaryExprContext() = default;
    void copyFrom(UnaryExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PassToPrimaryExprContext : public UnaryExprContext {
  public:
    PassToPrimaryExprContext(UnaryExprContext *ctx);

    PrimaryExprContext *primaryExpr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  NegationExprContext : public UnaryExprContext {
  public:
    NegationExprContext(UnaryExprContext *ctx);

    antlr4::tree::TerminalNode *T_SUB();
    UnaryExprContext *unaryExpr();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  UnaryExprContext* unaryExpr();

  class  PrimaryExprContext : public antlr4::ParserRuleContext {
  public:
    PrimaryExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    PrimaryExprContext() = default;
    void copyFrom(PrimaryExprContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  ParenthesizedExprContext : public PrimaryExprContext {
  public:
    ParenthesizedExprContext(PrimaryExprContext *ctx);

    antlr4::tree::TerminalNode *T_L_PAREN();
    ExprContext *expr();
    antlr4::tree::TerminalNode *T_R_PAREN();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  FunctionCallAtomContext : public PrimaryExprContext {
  public:
    FunctionCallAtomContext(PrimaryExprContext *ctx);

    antlr4::tree::TerminalNode *T_ID();
    antlr4::tree::TerminalNode *T_L_PAREN();
    antlr4::tree::TerminalNode *T_R_PAREN();
    RealParamListContext *realParamList();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  LValAtomContext : public PrimaryExprContext {
  public:
    LValAtomContext(PrimaryExprContext *ctx);

    LValContext *lVal();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  IntegerAtomContext : public PrimaryExprContext {
  public:
    IntegerAtomContext(PrimaryExprContext *ctx);

    IntegerLiteralContext *integerLiteral();

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  PrimaryExprContext* primaryExpr();

  class  IntegerLiteralContext : public antlr4::ParserRuleContext {
  public:
    IntegerLiteralContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_HEX_LITERAL();
    antlr4::tree::TerminalNode *T_OCT_LITERAL();
    antlr4::tree::TerminalNode *T_DEC_LITERAL();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IntegerLiteralContext* integerLiteral();

  class  RealParamListContext : public antlr4::ParserRuleContext {
  public:
    RealParamListContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExprContext *> expr();
    ExprContext* expr(size_t i);
    std::vector<antlr4::tree::TerminalNode *> T_COMMA();
    antlr4::tree::TerminalNode* T_COMMA(size_t i);


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RealParamListContext* realParamList();

  class  LValContext : public antlr4::ParserRuleContext {
  public:
    LValContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *T_ID();


    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LValContext* lVal();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool addExprSempred(AddExprContext *_localctx, size_t predicateIndex);
  bool mulExprSempred(MulExprContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

