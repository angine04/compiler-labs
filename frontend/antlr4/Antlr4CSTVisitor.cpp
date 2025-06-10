///
/// @file Antlr4CSTVisitor.cpp
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

#include <string>

#include "Antlr4CSTVisitor.h"
#include "AST.h"
#include "AttrType.h"

#define Instanceof(res, type, var) auto res = dynamic_cast<type>(var)

/// @brief 构造函数
MiniCCSTVisitor::MiniCCSTVisitor()
{}

/// @brief 析构函数
MiniCCSTVisitor::~MiniCCSTVisitor()
{}

/// @brief 遍历CST产生AST
/// @param root CST语法树的根结点
/// @return AST的根节点
ast_node * MiniCCSTVisitor::run(MiniCParser::CompileUnitContext * root)
{
    return std::any_cast<ast_node *>(visitCompileUnit(root));
}

/// @brief 非终结运算符compileUnit的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitCompileUnit(MiniCParser::CompileUnitContext * ctx)
{
    // compileUnit: (funcDef | varDecl)* EOF

    // 请注意这里必须先遍历全局变量后遍历函数。肯定可以确保全局变量先声明后使用的规则，但有些情况却不能检查出。
    // 事实上可能函数A后全局变量B后函数C，这时在函数A中是不能使用变量B的，需要报语义错误，但目前的处理不会。
    // 因此在进行语义检查时，可能追加检查行号和列号，如果函数的行号/列号在全局变量的行号/列号的前面则需要报语义错误
    // TODO 请追加实现。

    ast_node * temp_node;
    ast_node * compileUnitNode = create_contain_node(ast_operator_type::AST_OP_COMPILE_UNIT);

    // 可能多个变量，因此必须循环遍历
    for (auto varCtx: ctx->varDecl()) {

        // 变量函数定义
        temp_node = std::any_cast<ast_node *>(visitVarDecl(varCtx));
        (void) compileUnitNode->insert_son_node(temp_node);
    }

    // 可能有多个函数，因此必须循环遍历
    for (auto funcCtx: ctx->funcDef()) {

        // 变量函数定义
        temp_node = std::any_cast<ast_node *>(visitFuncDef(funcCtx));
        (void) compileUnitNode->insert_son_node(temp_node);
    }

    return compileUnitNode;
}

/// @brief 非终结运算符funcDef的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitFuncDef(MiniCParser::FuncDefContext * ctx)
{
    // 识别的文法产生式：funcDef : (T_INT | T_VOID) T_ID T_L_PAREN formalParamList? T_R_PAREN block;

    // 函数返回类型，终结符
    type_attr funcReturnType;
    if (ctx->T_INT()) {
        funcReturnType = {BasicType::TYPE_INT, (int64_t) ctx->T_INT()->getSymbol()->getLine()};
    } else if (ctx->T_VOID()) {
        funcReturnType = {BasicType::TYPE_VOID, (int64_t) ctx->T_VOID()->getSymbol()->getLine()};
    }

    // 创建函数名的标识符终结符节点，终结符
    char * id = strdup(ctx->T_ID()->getText().c_str());

    var_id_attr funcId{id, (int64_t) ctx->T_ID()->getSymbol()->getLine()};

    // 形参结点处理
    ast_node * formalParamsNode = nullptr;
    if (ctx->formalParamList()) {
        formalParamsNode = std::any_cast<ast_node *>(visitFormalParamList(ctx->formalParamList()));
    }

    // 遍历block结点创建函数体节点，非终结符
    auto blockNode = std::any_cast<ast_node *>(visitBlock(ctx->block()));

    // 创建函数定义的节点，孩子有类型，函数名，语句块和形参
    // create_func_def函数内会释放funcId中指向的标识符空间，切记，之后不要再释放，之前一定要是通过strdup函数或者malloc分配的空间
    return create_func_def(funcReturnType, funcId, blockNode, formalParamsNode);
}

/// @brief 非终结运算符block的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitBlock(MiniCParser::BlockContext * ctx)
{
    // 识别的文法产生式：block : T_L_BRACE blockItemList? T_R_BRACE';
    if (!ctx->blockItemList()) {
        // 语句块没有语句

        // 为了方便创建一个空的Block节点
        return create_contain_node(ast_operator_type::AST_OP_BLOCK);
    }

    // 语句块含有语句

    // 内部创建Block节点，并把语句加入，这里不需要创建Block节点
    return visitBlockItemList(ctx->blockItemList());
}

/// @brief 非终结运算符blockItemList的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitBlockItemList(MiniCParser::BlockItemListContext * ctx)
{
    // 识别的文法产生式：blockItemList : blockItem +;
    // 正闭包 循环 至少一个blockItem
    auto block_node = create_contain_node(ast_operator_type::AST_OP_BLOCK);

    for (auto blockItemCtx: ctx->blockItem()) {

        // 非终结符，需遍历
        auto blockItem = std::any_cast<ast_node *>(visitBlockItem(blockItemCtx));

        // 插入到块节点中
        (void) block_node->insert_son_node(blockItem);
    }

    return block_node;
}

/// @brief 非终结运算符blockItem的遍历
/// @param ctx CST上下文
///
std::any MiniCCSTVisitor::visitBlockItem(MiniCParser::BlockItemContext * ctx)
{
    // 识别的文法产生式：blockItem : statement | varDecl
    if (ctx->statement()) {
        // 语句识别
        // dispatch to the appropriate labeled alternative of statement
        return visit(ctx->statement());
    } else if (ctx->varDecl()) {
        return visitVarDecl(ctx->varDecl());
    }

    return nullptr;
}

/// @brief 非终结运算符expr的遍历
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitExpr(MiniCParser::ExprContext * ctx)
{
    //识别产生式：expr: addExpr;
    // Our new g4 is: expr: addExpr;
    return visit(ctx->addExpr()); // Dispatch to the next level of precedence
}

// Implement new pass-through visitors
std::any MiniCCSTVisitor::visitPassToMulExpr(MiniCParser::PassToMulExprContext * ctx)
{
    return visit(ctx->mulExpr());
}

std::any MiniCCSTVisitor::visitPassToUnaryExpr(MiniCParser::PassToUnaryExprContext * ctx)
{
    return visit(ctx->unaryExpr());
}

std::any MiniCCSTVisitor::visitPassToPrimaryExpr(MiniCParser::PassToPrimaryExprContext * ctx)
{
    return visit(ctx->primaryExpr());
}

// Implement Parenthesized Expression
std::any MiniCCSTVisitor::visitParenthesizedExpr(MiniCParser::ParenthesizedExprContext * ctx)
{
    return visit(ctx->expr()); // Evaluate the inner expression
}

// Implement Integer Atom (which delegates to IntegerLiteral)
std::any MiniCCSTVisitor::visitIntegerAtom(MiniCParser::IntegerAtomContext * ctx)
{
    return visit(ctx->integerLiteral());
}

// Implement LVal Atom (delegates to LVal)
std::any MiniCCSTVisitor::visitLValAtom(MiniCParser::LValAtomContext * ctx)
{
    return visit(ctx->lVal());
}

// Implement IntegerLiteral to handle different bases
std::any MiniCCSTVisitor::visitIntegerLiteral(MiniCParser::IntegerLiteralContext * ctx)
{
    antlr4::Token * token;
    uint32_t val;
    std::string text;
    int64_t lineNo;

    if (ctx->T_HEX_LITERAL()) {
        token = ctx->T_HEX_LITERAL()->getSymbol();
        text = token->getText();
        lineNo = token->getLine();
        // Remove "0x" or "0X" prefix for stoul
        val = static_cast<uint32_t>(std::stoul(text.substr(2), nullptr, 16));
    } else if (ctx->T_OCT_LITERAL()) {
        token = ctx->T_OCT_LITERAL()->getSymbol();
        text = token->getText();
        lineNo = token->getLine();
        // std::stoul with base 8 handles strings like "0123" directly if the '0' is part of the octal number
        // If T_OCT_LITERAL rule is '0'[0-7]+, text will be like "012", "077"
        val = static_cast<uint32_t>(std::stoul(text, nullptr, 8));
    } else if (ctx->T_DEC_LITERAL()) {
        token = ctx->T_DEC_LITERAL()->getSymbol();
        text = token->getText();
        lineNo = token->getLine();
        val = static_cast<uint32_t>(std::stoul(text, nullptr, 10));
    } else {
        // Should not happen if grammar is correct and complete
        // Consider throwing an error or returning a specific error AST node
        return nullptr;
    }

    digit_int_attr val_attr = {val, lineNo};
    return std::any(ast_node::New(val_attr));
}

// Implement Negation Expression
std::any MiniCCSTVisitor::visitNegationExpr(MiniCParser::NegationExprContext * ctx)
{
    auto operand = std::any_cast<ast_node *>(visit(ctx->unaryExpr()));
    if (!operand) {
        // Error handling or specific logging if needed
        return nullptr;
    }
    return std::any(create_contain_node(ast_operator_type::AST_OP_NEG, operand));
}

// Restore/Confirm visitLVal if it was removed by the previous edit block comment
std::any MiniCCSTVisitor::visitLVal(MiniCParser::LValContext * ctx)
{
    //识别文法产生式：lVal: T_ID;
    // 获取ID的名字
    auto varIdText = ctx->T_ID()->getText();
    char * varId = strdup(varIdText.c_str());

    // 获取行号
    int64_t lineNo = (int64_t) ctx->T_ID()->getSymbol()->getLine();

    // Assuming var_id_attr has only {char* id, int64_t lineno}
    var_id_attr id_attr = {varId, lineNo};

    return std::any(ast_node::New(id_attr));
}

// Implement Function Call Atom
std::any MiniCCSTVisitor::visitFunctionCallAtom(MiniCParser::FunctionCallAtomContext * ctx)
{
    char * funcName = strdup(ctx->T_ID()->getText().c_str());
    int64_t lineNo = ctx->T_ID()->getSymbol()->getLine();
    // Assuming var_id_attr has only {char* id, int64_t lineno}
    var_id_attr func_id_attr = {funcName, lineNo};
    ast_node * func_name_node = ast_node::New(func_id_attr);

    ast_node * paramListNode = nullptr;
    if (ctx->realParamList()) {
        paramListNode = std::any_cast<ast_node *>(visit(ctx->realParamList()));
    } else {
        // Use AST_OP_FUNC_REAL_PARAMS for empty args list node type
        paramListNode = create_contain_node(ast_operator_type::AST_OP_FUNC_REAL_PARAMS);
    }

    return std::any(create_func_call(func_name_node, paramListNode));
}

// Ensure visitRealParamList is implemented correctly to return an AST_OP_FUNC_REAL_PARAMS node
std::any MiniCCSTVisitor::visitRealParamList(MiniCParser::RealParamListContext * ctx)
{
    //识别文法产生式：realParamList: expr (T_COMMA expr)*;
    // Use AST_OP_FUNC_REAL_PARAMS for the argument list container node type
    ast_node * argsNode = create_contain_node(ast_operator_type::AST_OP_FUNC_REAL_PARAMS);

    for (auto exprCtx: ctx->expr()) {
        auto argNode = std::any_cast<ast_node *>(visit(exprCtx));
        if (argNode) {
            (void) argsNode->insert_son_node(argNode);
        }
    }
    return std::any(argsNode);
}

// Implement Add/Sub Expression
std::any MiniCCSTVisitor::visitAddSubExpr(MiniCParser::AddSubExprContext * ctx)
{
    // Corresponds to grammar: addExpr (T_ADD | T_SUB) mulExpr
    ast_node * left = std::any_cast<ast_node *>(visit(ctx->addExpr()));  // visit the left addExpr
    ast_node * right = std::any_cast<ast_node *>(visit(ctx->mulExpr())); // visit the right mulExpr

    if (!left || !right) {
        // Error in operand parsing
        return nullptr;
    }

    ast_operator_type op_type;
    if (ctx->T_ADD()) {
        op_type = ast_operator_type::AST_OP_ADD;
    } else if (ctx->T_SUB()) {
        op_type = ast_operator_type::AST_OP_SUB;
    } else {
        // Should not happen based on grammar rule for this labeled alternative
        return nullptr;
    }

    return std::any(create_contain_node(op_type, left, right));
}

// Implement Mul/Div/Mod Expression
std::any MiniCCSTVisitor::visitMulDivModExpr(MiniCParser::MulDivModExprContext * ctx)
{
    // Corresponds to grammar: mulExpr (T_MUL | T_DIV | T_MOD) unaryExpr
    ast_node * left = std::any_cast<ast_node *>(visit(ctx->mulExpr()));    // visit the left mulExpr
    ast_node * right = std::any_cast<ast_node *>(visit(ctx->unaryExpr())); // visit the right unaryExpr

    if (!left || !right) {
        // Error in operand parsing
        return nullptr;
    }

    ast_operator_type op_type;
    if (ctx->T_MUL()) {
        op_type = ast_operator_type::AST_OP_MUL;
    } else if (ctx->T_DIV()) {
        op_type = ast_operator_type::AST_OP_DIV;
    } else if (ctx->T_MOD()) {
        op_type = ast_operator_type::AST_OP_MOD;
    } else {
        // Should not happen
        return nullptr;
    }

    return std::any(create_contain_node(op_type, left, right));
}

std::any MiniCCSTVisitor::visitAssignStatement(MiniCParser::AssignStatementContext * ctx)
{
    // 识别文法产生式：assignStatement: lVal T_ASSIGN expr T_SEMICOLON

    // 赋值左侧左值Lval遍历产生节点
    auto lvalNode = std::any_cast<ast_node *>(visitLVal(ctx->lVal()));

    // 赋值右侧expr遍历
    auto exprNode = std::any_cast<ast_node *>(visitExpr(ctx->expr()));

    // 创建一个AST_OP_ASSIGN类型的中间节点，孩子为Lval和Expr
    return ast_node::New(ast_operator_type::AST_OP_ASSIGN, lvalNode, exprNode, nullptr);
}

std::any MiniCCSTVisitor::visitBlockStatement(MiniCParser::BlockStatementContext * ctx)
{
    // 识别文法产生式 blockStatement: block

    return visitBlock(ctx->block());
}

std::any MiniCCSTVisitor::visitVarDecl(MiniCParser::VarDeclContext * ctx)
{
    // varDecl: basicType varDef (T_COMMA varDef)* T_SEMICOLON;

    // 声明语句节点
    ast_node * stmt_node = create_contain_node(ast_operator_type::AST_OP_DECL_STMT);

    // 类型节点
    type_attr typeAttr = std::any_cast<type_attr>(visitBasicType(ctx->basicType()));

    for (auto & varCtx: ctx->varDef()) {
        // 变量定义节点（可能是ID节点或初始化节点）
        ast_node * var_node = std::any_cast<ast_node *>(visitVarDef(varCtx));

        // 创建类型节点
        ast_node * type_node = create_type_node(typeAttr);

        ast_node * decl_node;
        if (var_node->node_type == ast_operator_type::AST_OP_VAR_INIT) {
            // 初始化节点，直接使用并设置类型
            decl_node = var_node;
            decl_node->type = type_node->type;
        } else {
            // 普通ID节点，创建变量声明节点
            decl_node = ast_node::New(ast_operator_type::AST_OP_VAR_DECL, type_node, var_node, nullptr);
        }

        // 插入到变量声明语句
        (void) stmt_node->insert_son_node(decl_node);
    }

    return stmt_node;
}

std::any MiniCCSTVisitor::visitVarDef(MiniCParser::VarDefContext * ctx)
{
    // varDef: T_ID (T_ASSIGN expr)?;

    auto varId = ctx->T_ID()->getText();

    // 获取行号
    int64_t lineNo = (int64_t) ctx->T_ID()->getSymbol()->getLine();

    // 检查是否有初始化表达式
    if (ctx->expr()) {
        // 有初始化值，创建变量ID节点
        ast_node * id_node = ast_node::New(varId, lineNo);

        // 访问初始化表达式
        ast_node * expr_node = std::any_cast<ast_node *>(visitExpr(ctx->expr()));

        // 创建初始化节点
        return ast_node::New(ast_operator_type::AST_OP_VAR_INIT, id_node, expr_node, nullptr);
    } else {
        // 没有初始化值，返回普通的ID节点
    return ast_node::New(varId, lineNo);
    }
}

std::any MiniCCSTVisitor::visitBasicType(MiniCParser::BasicTypeContext * ctx)
{
    // basicType: T_INT;
    type_attr attr{BasicType::TYPE_VOID, -1};
    if (ctx->T_INT()) {
        attr.type = BasicType::TYPE_INT;
        attr.lineno = (int64_t) ctx->T_INT()->getSymbol()->getLine();
    }

    return attr;
}

std::any MiniCCSTVisitor::visitExpressionStatement(MiniCParser::ExpressionStatementContext * ctx)
{
    // 识别文法产生式  expr ? T_SEMICOLON #expressionStatement;
    if (ctx->expr()) {
        // 表达式语句

        // 遍历expr非终结符，创建表达式节点后返回
        return visitExpr(ctx->expr());
    } else {
        // 空语句

        // 直接返回空指针，需要再把语句加入到语句块时要注意判断，空语句不要加入
        return nullptr;
    }
}

///
/// @brief 非终结运算符statement中的returnStatement的遍历
/// @param ctx CST上下文
///
std::any MiniCCSTVisitor::visitReturnStatement(MiniCParser::ReturnStatementContext * ctx)
{
    // 识别的文法产生式：returnStatement -> T_RETURN expr T_SEMICOLON

    // 非终结符，表达式expr遍历
    auto exprNode = std::any_cast<ast_node *>(visitExpr(ctx->expr()));

    // 创建返回节点，其孩子为Expr
    return create_contain_node(ast_operator_type::AST_OP_RETURN, exprNode);
}

/// @brief 非终结符FormalParamList的分析
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitFormalParamList(MiniCParser::FormalParamListContext * ctx)
{
    // 识别文法产生式：formalParamList: formalParam (T_COMMA formalParam)*;
    ast_node * paramsNode = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS);

    for (auto paramCtx: ctx->formalParam()) {
        auto paramNode = std::any_cast<ast_node *>(visitFormalParam(paramCtx));
        if (paramNode) {
            (void) paramsNode->insert_son_node(paramNode);
        }
    }
    return std::any(paramsNode);
}

/// @brief 非终结符FormalParam的分析
/// @param ctx CST上下文
std::any MiniCCSTVisitor::visitFormalParam(MiniCParser::FormalParamContext * ctx)
{
    // 识别文法产生式：formalParam: basicType T_ID;

    // 获取类型信息
    type_attr typeAttr = std::any_cast<type_attr>(visitBasicType(ctx->basicType()));

    // 获取参数名
    char * paramName = strdup(ctx->T_ID()->getText().c_str());
    int64_t lineNo = (int64_t) ctx->T_ID()->getSymbol()->getLine();

    var_id_attr paramId{paramName, lineNo};

    // 创建类型节点
    ast_node * type_node = create_type_node(typeAttr);

    // 创建参数名节点
    ast_node * id_node = ast_node::New(paramId);

    // 创建形参节点
    return ast_node::New(ast_operator_type::AST_OP_FUNC_FORMAL_PARAM, type_node, id_node, nullptr);
}