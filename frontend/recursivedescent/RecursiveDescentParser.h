///
/// @file RecursiveDescentParser.h
/// @brief 递归下降分析法实现的语法分析后产生抽象语法树的头文件
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#pragma once

#include "AST.h"
#include "AttrType.h"

/// @brief Token类型
enum RDTokenType {
    T_EMPTY = -2,
    T_ERR = -1,
    T_EOF = 0,

    T_DEC_LITERAL, // Decimal integer literal
    T_HEX_LITERAL, // Hexadecimal integer literal
    T_OCT_LITERAL, // Octal integer literal

    T_INT,
    T_VOID,
    T_ID,

    T_L_PAREN,
    T_R_PAREN,
    T_L_BRACE,
    T_R_BRACE,
    T_L_BRACKET,    // [
    T_R_BRACKET,    // ]
    T_SEMICOLON,
    T_COMMA,

    T_RETURN,
    T_ASSIGN,
    T_ADD,
    T_SUB,
    T_MUL, // Multiplication
    T_DIV, // Division
    T_MOD, // Modulus

    // 比较运算符
    T_LT,  // <
    T_LE,  // <=
    T_GT,  // >
    T_GE,  // >=
    T_EQ,  // ==
    T_NE,  // !=

    // 逻辑运算符
    T_LOGICAL_AND,  // &&
    T_LOGICAL_OR,   // ||
    T_LOGICAL_NOT,  // !

    // 控制流关键字
    T_IF,
    T_ELSE,
    T_WHILE,
    T_BREAK,
    T_CONTINUE
};

/// @brief 词法与语法分析数据交互的Token的值类型
union RDSType {
    ast_node * node;
    digit_int_attr integer_num; // 整型字面量
    digit_real_attr float_num;  // 实数字面量
    var_id_attr var_id;         // 标识符（变量名）
    type_attr type;             // 类型
};

/// @brief 词法与语法分析数据交互的Token的值
extern RDSType rd_lval;

///
/// @brief 采用递归下降分析法实现词法与语法分析生成抽象语法树
/// @return ast_node* 空指针失败，否则成功
///
ast_node * rd_parse();
