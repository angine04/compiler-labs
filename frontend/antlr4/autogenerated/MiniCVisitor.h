
// Generated from /Users/zheyu/Repositories/exp04-minic-expr/frontend/antlr4/MiniC.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "MiniCParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by MiniCParser.
 */
class  MiniCVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by MiniCParser.
   */
    virtual std::any visitCompileUnit(MiniCParser::CompileUnitContext *context) = 0;

    virtual std::any visitFuncDef(MiniCParser::FuncDefContext *context) = 0;

    virtual std::any visitBlock(MiniCParser::BlockContext *context) = 0;

    virtual std::any visitBlockItemList(MiniCParser::BlockItemListContext *context) = 0;

    virtual std::any visitBlockItem(MiniCParser::BlockItemContext *context) = 0;

    virtual std::any visitVarDecl(MiniCParser::VarDeclContext *context) = 0;

    virtual std::any visitBasicType(MiniCParser::BasicTypeContext *context) = 0;

    virtual std::any visitVarDef(MiniCParser::VarDefContext *context) = 0;

    virtual std::any visitReturnStatement(MiniCParser::ReturnStatementContext *context) = 0;

    virtual std::any visitAssignStatement(MiniCParser::AssignStatementContext *context) = 0;

    virtual std::any visitBlockStatement(MiniCParser::BlockStatementContext *context) = 0;

    virtual std::any visitExpressionStatement(MiniCParser::ExpressionStatementContext *context) = 0;

    virtual std::any visitExpr(MiniCParser::ExprContext *context) = 0;

    virtual std::any visitPassToMulExpr(MiniCParser::PassToMulExprContext *context) = 0;

    virtual std::any visitAddSubExpr(MiniCParser::AddSubExprContext *context) = 0;

    virtual std::any visitPassToUnaryExpr(MiniCParser::PassToUnaryExprContext *context) = 0;

    virtual std::any visitMulDivModExpr(MiniCParser::MulDivModExprContext *context) = 0;

    virtual std::any visitNegationExpr(MiniCParser::NegationExprContext *context) = 0;

    virtual std::any visitPassToPrimaryExpr(MiniCParser::PassToPrimaryExprContext *context) = 0;

    virtual std::any visitParenthesizedExpr(MiniCParser::ParenthesizedExprContext *context) = 0;

    virtual std::any visitIntegerAtom(MiniCParser::IntegerAtomContext *context) = 0;

    virtual std::any visitLValAtom(MiniCParser::LValAtomContext *context) = 0;

    virtual std::any visitFunctionCallAtom(MiniCParser::FunctionCallAtomContext *context) = 0;

    virtual std::any visitIntegerLiteral(MiniCParser::IntegerLiteralContext *context) = 0;

    virtual std::any visitRealParamList(MiniCParser::RealParamListContext *context) = 0;

    virtual std::any visitLVal(MiniCParser::LValContext *context) = 0;


};

