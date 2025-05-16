
// Generated from /Users/zheyu/Repositories/exp04-minic-expr/frontend/antlr4/MiniC.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "MiniCVisitor.h"


/**
 * This class provides an empty implementation of MiniCVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  MiniCBaseVisitor : public MiniCVisitor {
public:

  virtual std::any visitCompileUnit(MiniCParser::CompileUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFuncDef(MiniCParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock(MiniCParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockItemList(MiniCParser::BlockItemListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockItem(MiniCParser::BlockItemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarDecl(MiniCParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBasicType(MiniCParser::BasicTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVarDef(MiniCParser::VarDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReturnStatement(MiniCParser::ReturnStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignStatement(MiniCParser::AssignStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlockStatement(MiniCParser::BlockStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpressionStatement(MiniCParser::ExpressionStatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpr(MiniCParser::ExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPassToMulExpr(MiniCParser::PassToMulExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAddSubExpr(MiniCParser::AddSubExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPassToUnaryExpr(MiniCParser::PassToUnaryExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMulDivModExpr(MiniCParser::MulDivModExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNegationExpr(MiniCParser::NegationExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPassToPrimaryExpr(MiniCParser::PassToPrimaryExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParenthesizedExpr(MiniCParser::ParenthesizedExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntegerAtom(MiniCParser::IntegerAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLValAtom(MiniCParser::LValAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunctionCallAtom(MiniCParser::FunctionCallAtomContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntegerLiteral(MiniCParser::IntegerLiteralContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRealParamList(MiniCParser::RealParamListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLVal(MiniCParser::LValContext *ctx) override {
    return visitChildren(ctx);
  }


};

