///
/// @file Antlr4CSTVisitor.h
/// @brief Antlr4的具体语法树的遍历产生AST
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#pragma once

#include "AST.h"
#include "MiniCBaseVisitor.h"

/// @brief 遍历具体语法树产生抽象语法树
class MiniCCSTVisitor : public MiniCBaseVisitor {

public:
    /// @brief 构造函数
    MiniCCSTVisitor();

    /// @brief 析构函数
    virtual ~MiniCCSTVisitor();

    /// @brief 遍历CST产生AST
    /// @param root CST语法树的根结点
    /// @return AST的根节点
    ast_node * run(MiniCParser::CompileUnitContext * root);

protected:
    /* 下面的函数都是从MiniCBaseVisitor继承下来的虚拟函数，需要重载实现 */

    /// @brief 非终结运算符compileUnit的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitCompileUnit(MiniCParser::CompileUnitContext * ctx) override;

    /// @brief 非终结运算符funcDef的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitFuncDef(MiniCParser::FuncDefContext * ctx) override;

    /// @brief 非终结运算符block的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitBlock(MiniCParser::BlockContext * ctx) override;

    /// @brief 非终结运算符blockItemList的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitBlockItemList(MiniCParser::BlockItemListContext * ctx) override;

    /// @brief 非终结运算符blockItem的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitBlockItem(MiniCParser::BlockItemContext * ctx) override;

    /// @brief 非终结运算符statement中的returnStatement的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitReturnStatement(MiniCParser::ReturnStatementContext * ctx) override;

    /// @brief 非终结运算符expr的遍历
    /// @param ctx CST上下文
    /// @return AST的节点
    std::any visitExpr(MiniCParser::ExprContext * ctx) override;

    ///
    /// @brief 内部产生的非终结符assignStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitAssignStatement(MiniCParser::AssignStatementContext * ctx) override;

    ///
    /// @brief 内部产生的非终结符blockStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitBlockStatement(MiniCParser::BlockStatementContext * ctx) override;

    ///
    /// @brief 非终结符ExpressionStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitExpressionStatement(MiniCParser::ExpressionStatementContext * context) override;

    // 新增的控制流语句访问方法
    ///
    /// @brief 非终结符IfStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitIfStatement(MiniCParser::IfStatementContext * ctx) override;

    ///
    /// @brief 非终结符WhileStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitWhileStatement(MiniCParser::WhileStatementContext * ctx) override;

    ///
    /// @brief 非终结符BreakStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitBreakStatement(MiniCParser::BreakStatementContext * ctx) override;

    ///
    /// @brief 非终结符ContinueStatement的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitContinueStatement(MiniCParser::ContinueStatementContext * ctx) override;

    // 新增的逻辑表达式访问方法
    ///
    /// @brief 逻辑或运算符表达式的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitLogicalOrOpExpr(MiniCParser::LogicalOrOpExprContext * ctx) override;

    ///
    /// @brief 传递到LogicalAndExpr的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitPassToLogicalAndExpr(MiniCParser::PassToLogicalAndExprContext * ctx) override;

    ///
    /// @brief 逻辑与运算符表达式的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitLogicalAndOpExpr(MiniCParser::LogicalAndOpExprContext * ctx) override;

    ///
    /// @brief 传递到EqualityExpr的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitPassToEqualityExpr(MiniCParser::PassToEqualityExprContext * ctx) override;

    ///
    /// @brief 相等比较运算符表达式的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitEqualityOpExpr(MiniCParser::EqualityOpExprContext * ctx) override;

    ///
    /// @brief 传递到RelationalExpr的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitPassToRelationalExpr(MiniCParser::PassToRelationalExprContext * ctx) override;

    ///
    /// @brief 关系比较运算符表达式的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitRelationalOpExpr(MiniCParser::RelationalOpExprContext * ctx) override;

    ///
    /// @brief 传递到AddExpr的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitPassToAddExpr(MiniCParser::PassToAddExprContext * ctx) override;

    ///
    /// @brief 逻辑非运算符表达式的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitLogicalNotExpr(MiniCParser::LogicalNotExprContext * ctx) override;

    // 现有表达式方法
    std::any visitPassToMulExpr(MiniCParser::PassToMulExprContext * ctx) override;
    std::any visitAddSubExpr(MiniCParser::AddSubExprContext * ctx) override;
    std::any visitPassToUnaryExpr(MiniCParser::PassToUnaryExprContext * ctx) override;
    std::any visitMulDivModExpr(MiniCParser::MulDivModExprContext * ctx) override;
    std::any visitNegationExpr(MiniCParser::NegationExprContext * ctx) override;
    std::any visitPassToPrimaryExpr(MiniCParser::PassToPrimaryExprContext * ctx) override;
    std::any visitParenthesizedExpr(MiniCParser::ParenthesizedExprContext * ctx) override;
    std::any visitIntegerAtom(MiniCParser::IntegerAtomContext * ctx) override;
    std::any visitLValAtom(MiniCParser::LValAtomContext * ctx) override;
    std::any visitFunctionCallAtom(MiniCParser::FunctionCallAtomContext * ctx) override;
    std::any visitIntegerLiteral(MiniCParser::IntegerLiteralContext * ctx) override;

    ///
    /// @brief 非终结符LVal的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitLVal(MiniCParser::LValContext * ctx) override;

    ///
    /// @brief 非终结符VarDecl的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitVarDecl(MiniCParser::VarDeclContext * ctx) override;

    ///
    /// @brief 非终结符VarDecl的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitVarDef(MiniCParser::VarDefContext * ctx) override;

    ///
    /// @brief 非终结符BasicType的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitBasicType(MiniCParser::BasicTypeContext * ctx) override;

    ///
    /// @brief 非终结符RealParamList的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitRealParamList(MiniCParser::RealParamListContext * ctx) override;

    ///
    /// @brief 非终结符FormalParamList的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitFormalParamList(MiniCParser::FormalParamListContext * ctx) override;

    ///
    /// @brief 非终结符FormalParam的分析
    /// @param ctx CST上下文
    /// @return std::any AST的节点
    ///
    std::any visitFormalParam(MiniCParser::FormalParamContext * ctx) override;
};
