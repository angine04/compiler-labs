///
/// @file RecursiveDescentParser.cpp
/// @brief 递归下降分析法实现的语法分析后产生抽象语法树的实现
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
#include <stdarg.h>

#include "AST.h"
#include "AttrType.h"
#include "RecursiveDescentFlex.h"
#include "RecursiveDescentParser.h"
#include "Type.h"
#include "Type.h"

// 定义全局变量给词法分析使用，用于填充值
RDSType rd_lval;

// 词法识别的记号值，原始文本字符串
extern std::string tokenValue;

// 语法分析过程中的错误数目
static int errno_num = 0;

// 语法分析过程中的LookAhead，指向下一个Token
static RDTokenType lookaheadTag = RDTokenType::T_EMPTY;

static ast_node * Block();
static ast_node * expr(); // Ensure expr is forward declared
static ast_node * Expression();
static ast_node * logicalOrExpr();
static ast_node * logicalAndExpr();
static ast_node * equalityExpr();
static ast_node * relationalExpr();
static ast_node * unaryExpr();
static ast_node * Term();
static ast_node * Factor();

///
/// @brief 继续检查LookAhead指向的记号是否是T，用于符号的FIRST集合或Follow集合判断
///
#define _(T) || (lookaheadTag == T)

///
/// @brief 第一个检查LookAhead指向的记号是否属于C，用于符号的FIRST集合或Follow集合判断
/// 如判断是否是T_ID，或者T_INT，可结合F和_两个宏来实现，即F(T_ID) _(T_INT)
///
#define F(C) (lookaheadTag == C)

///
/// @brief lookahead指向下一个Token
///
static void advance()
{
    lookaheadTag = (RDTokenType) rd_flex();
}

///
/// @brief flag若匹配则跳过Token，使得LookAhead指向下一个Token
/// @param tag 是否匹配指定的Tag
/// @return true：匹配，false：未匹配
///
static bool match(RDTokenType tag)
{
    bool result = false;

    if (F(tag)) {

        result = true;

        // 匹配，则向前获取下一个Token
        advance();
    }

    return result;
}

///
/// @brief 语法错误输出
/// @param format 格式化字符串，和printf的格式化字符串一样
///
static void semerror(const char * format, ...)
{
    char logStr[1024];

    va_list ap;
    va_start(ap, format);

    // 利用vsnprintf函数将可变参数按照一定的格式，格式化为一个字符串。
    vsnprintf(logStr, sizeof(logStr), format, ap);

    va_end(ap);

    printf("Line(%d): %s\n", rd_line_no, logStr);

    errno_num++;
}

///
/// @brief 实参列表语法分析，文法: realParamList: expr (T_COMMA expr)*;
/// @return ast_node* 实参列表节点
///
static void realParamList(ast_node * realParamsNode)
{
    // 实参表达式expr识别
    ast_node * param_node = expr();
    if (!param_node) {

        // 不是合法的实参
        return;
    }

    // 实参作为孩子插入到父节点realParamsNode中
    (void) realParamsNode->insert_son_node(param_node);

    // 后续是一个闭包(T_COMMA expr)*，即循环，不断的识别逗号与实参
    for (;;) {

        // 识别逗号
        if (match(T_COMMA)) {

            // 识别实参
            param_node = expr();

            (void) realParamsNode->insert_son_node(param_node);
        } else {

            // 不是逗号，则说明没有实参，可终止循环

            break;
        }
    }
}

///
/// @brief 识别ID尾部符号，可以是括号，代表函数调用；可以是中括号，代表数组（暂不支持）；可以是空串，代表简单变量
/// 其文法为 idTail: T_L_PAREN realParamList? T_R_PAREN | ε
/// @return ast_node*
///
static ast_node * idTail(var_id_attr & id)
{
    // 标识符节点
    ast_node * node = ast_node::New(id);

    // 对于字符型字面量的字符串空间需要释放，因词法用到了strdup进行了字符串空间的分配
    free(id.id);
    id.id = nullptr;

    if (match(T_L_PAREN)) {

        // 函数调用，idTail: T_L_PAREN realParamList? T_R_PAREN

        ast_node * realParamsNode = create_contain_node(ast_operator_type::AST_OP_FUNC_REAL_PARAMS);

        if (match(T_R_PAREN)) {

            // 被调用函数没有实参，返回一个空的实参清单节点
            return realParamsNode;
        }

        // 识别实参列表
        realParamList(realParamsNode);

        if (!match(T_R_PAREN)) {
            semerror("函数调用缺少右括号");
        }

        // 创建函数调用节点
        node = create_func_call(node, realParamsNode);
    } else {
        // 变量ID，idTail -> ε

        // 前面已经创建，可直接使用，无动作
    }

    return node;
}

///
/// @brief 表达式文法 expr : addExp, 现在支持完整的表达式层次
/// @return AST的节点
static ast_node * expr()
{
    return logicalOrExpr();
}

/// @brief returnStatement -> T_RETURN expr T_SEMICOLON
/// @return AST的节点
static ast_node * returnStatement()
{

    if (match(T_RETURN)) {

        // return语句的First集合元素为T_RETURN
        // 若匹配，则说明是return语句

        ast_node * expr_node = expr();

        if (!match(T_SEMICOLON)) {

            // 返回语句后没有分号
            semerror("返回语句后没有分号");
        }

        return create_contain_node(ast_operator_type::AST_OP_RETURN, expr_node);
    }

    return nullptr;
}

/// 识别表达式尾部符号，文法： assignExprStmtTail : T_ASSIGN expr | ε
static ast_node * assignExprStmtTail(ast_node * left_node)
{
    if (match(T_ASSIGN)) {

        // 赋值运算符，说明含有赋值运算

        // 必须要求左侧节点必须存在
        if (!left_node) {

            // 没有左侧节点，则语法错误
            semerror("赋值语句的左侧表达式不能为空");

            return nullptr;
        }

        // 赋值运算符右侧表达式分析识别
        ast_node * right_node = expr();

        return create_contain_node(ast_operator_type::AST_OP_ASSIGN, left_node, right_node);
    } else if (F(T_SEMICOLON)) {
        // 看是否满足assignExprStmtTail的Follow集合。在Follow集合中则正常结束，否则出错
        // 空语句
    }

    return left_node;
}

///
/// @brief 赋值语句或表达式语句识别，文法：assignExprStmt : expr assignExprStmtTail
/// @return ast_node*
///
static ast_node * assignExprStmt()
{
    // 识别表达式，目前还不知道是否是表达式语句或赋值语句
    ast_node * expr_node = expr();

    return assignExprStmtTail(expr_node);
}

// 前向声明
static ast_node * ifStatement();
static ast_node * whileStatement();
static ast_node * breakStatement();
static ast_node * continueStatement();

///
/// @brief 语句的识别，其文法为：
/// statement: returnStatement | block | T_SEMICOLON | assignExprStmt T_SEMICOLON | ifStatement | whileStatement | breakStatement | continueStatement
/// returnStatement : T_RETURN expr T_SEMICOLON
/// assignExprStmt : expr assignExprStmtTail
/// assignExprStmtTail : T_ASSIGN expr | ε
/// ifStatement : T_IF T_L_PAREN expr T_R_PAREN statement (T_ELSE statement)?
/// whileStatement : T_WHILE T_L_PAREN expr T_R_PAREN statement
/// breakStatement : T_BREAK T_SEMICOLON
/// continueStatement : T_CONTINUE T_SEMICOLON
///
/// @return AST的节点
///
static ast_node * statement()
{
    ast_node * node = nullptr;
    if (F(T_RETURN)) {
        node = returnStatement();
    } else if (F(T_L_BRACE)) {
        node = Block();
    } else if (F(T_SEMICOLON)) {
        advance();
    } else if (F(T_IF)) {
        node = ifStatement();
    } else if (F(T_WHILE)) {
        node = whileStatement();
    } else if (F(T_BREAK)) {
        node = breakStatement();
    } else if (F(T_CONTINUE)) {
        node = continueStatement();
    } else if (F(T_ID) _(T_L_PAREN) _(T_DEC_LITERAL) _(T_HEX_LITERAL) _(T_OCT_LITERAL) _(T_SUB)) { // Updated condition
        node = assignExprStmt();
        if (!match(T_SEMICOLON)) {
            semerror("语句后缺少分号");
        }
    }
    return node;
}

///
/// @brief 处理单个变量定义（支持初始化）
/// @param type 变量类型
/// @param id 变量标识符
/// @return 变量定义节点（普通声明或初始化节点）
///
static ast_node * processVarDef(type_attr & type, var_id_attr & id)
{
    if (match(T_ASSIGN)) {
        // 有初始化值
        ast_node * expr_node = logicalOrExpr();
        if (!expr_node) {
            semerror("变量初始化缺少表达式");
            return nullptr;
        }

        // 创建变量ID节点
        ast_node * id_node = ast_node::New(id);

        // 创建初始化节点
        ast_node * init_node = create_contain_node(ast_operator_type::AST_OP_VAR_INIT, id_node, expr_node);
        init_node->type = typeAttr2Type(type);

        return init_node;
    } else {
        // 普通变量声明
        return createVarDeclNode(type, id);
    }
}

///
/// @brief 变量定义列表语法识别，支持变量初始化
/// 其文法：varDeclList : T_COMMA T_ID (T_ASSIGN Expression)? varDeclList | T_SEMICOLON
/// @param vardeclstmt_node 变量声明语句节点，所有的变量节点应该加到该节点中
///
static void varDeclList(ast_node * vardeclstmt_node)
{
    if (match(T_COMMA)) {

        // 匹配成功，定义列表中有逗号

        // 检查是否是标识符
        if (F(T_ID)) {

            // 定义列表中定义的变量，支持初始化
            var_id_attr id = rd_lval.var_id;
            advance(); // 跳过T_ID

            // 从变量声明语句节点中获取类型信息
            Type * stmt_type = vardeclstmt_node->type;
            type_attr type;
            if (stmt_type->toString() == "i32") {
                type.type = BasicType::TYPE_INT;
            } else {
                type.type = BasicType::TYPE_VOID;
            }
            type.lineno = id.lineno;

            // 处理变量定义（可能包含初始化）
            ast_node * var_node = processVarDef(type, id);
            if (var_node) {
                vardeclstmt_node->insert_son_node(var_node);
            }

            // 递归调用，不断追加变量定义
            varDeclList(vardeclstmt_node);
        } else {
            semerror("逗号后必须是标识符");
        }
    } else if (match(T_SEMICOLON)) {
        // 匹配成功，则说明只有前面的一个变量或者变量定义，正常结束
    } else {
        semerror("非法记号: %d", (int) lookaheadTag);

        // 忽略该记号，继续检查
        advance();

        // 继续检查后续的变量
        varDeclList(vardeclstmt_node);
    }
}

///
/// @brief 局部变量的识别，支持数组声明，其文法为：
/// varDecl : T_INT T_ID (T_L_BRACKET T_DEC_LITERAL T_R_BRACKET)* (T_ASSIGN Expression)? varDeclList
///
/// @return ast_node* 局部变量声明节点
///
static ast_node * varDecl()
{
    if (F(T_INT)) {

        // 这里必须复制，而不能引用，因为rd_lval为全局，下一个记号识别后要被覆盖
        type_attr type = rd_lval.type;

        // 跳过int类型的记号，指向下一个Token
        advance();

        // 检测是否是标识符
        if (F(T_ID)) {

            // 获取变量ID
            var_id_attr id = rd_lval.var_id;

            // 跳过标识符记号，指向下一个Token
            advance();

            // 创建变量声明语句节点
            ast_node * stmt_node = create_contain_node(ast_operator_type::AST_OP_DECL_STMT);
            stmt_node->type = typeAttr2Type(type);

            // 处理数组维度
            ast_node * var_node = ast_node::New(id);
            bool is_array = false;
            
            while (F(T_L_BRACKET)) {
                advance(); // 消费 '['
                if (F(T_DEC_LITERAL)) {
                    digit_int_attr array_size_attr = rd_lval.integer_num;
                    ast_node * size_node = ast_node::New(array_size_attr);
                    advance(); // 消费数字
                    if (!match(T_R_BRACKET)) {
                        semerror("数组声明缺少右中括号");
                        if (var_node) ast_node::Delete(var_node);
                        if (size_node) ast_node::Delete(size_node);
                        return nullptr;
                    }
                    // 创建数组声明节点
                    var_node = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, var_node, size_node);
                    is_array = true;
                } else {
                    semerror("数组声明缺少大小");
                    if (var_node) ast_node::Delete(var_node);
                    return nullptr;
                }
            }
            
            // 处理初始化（如果有）
            if (match(T_ASSIGN)) {
                ast_node * init_expr = logicalOrExpr();
                if (!init_expr) {
                    semerror("变量初始化缺少表达式");
                    if (var_node) ast_node::Delete(var_node);
                    return nullptr;
                }
                // 创建初始化节点
                var_node = create_contain_node(ast_operator_type::AST_OP_VAR_INIT, var_node, init_expr);
            } else if (!is_array) {
                // 普通变量声明
                var_node = create_contain_node(ast_operator_type::AST_OP_VAR_DECL, ast_node::New(typeAttr2Type(type)), var_node);
            }
            
            // 设置类型信息
            if (var_node) {
                var_node->type = typeAttr2Type(type);
                stmt_node->insert_son_node(var_node);
            }

            // 处理后续的变量声明
            varDeclList(stmt_node);

            return stmt_node;

        } else {
            semerror("类型后要求的记号为标识符");
            // 这里忽略继续检查下一个记号，为便于一次可检查出多个错误
            // 当然可以直接退出循环，一旦有错就不再检查语法错误。
        }
    }

    return nullptr;
}

///
/// @brief 块中的项目识别，其文法为：
/// blockItem: statement | varDecl
/// varDecl : T_INT T_ID varDeclList
/// statement:T_RETURN expr T_SEMICOLON | lVal T_ASSIGN expr T_SEMICOLON | block | expr? T_SEMICOLON
/// varDecl的FIRST集合为{T_INT}，
/// statement的FIRST集合为{T_RETURN，T_ID，T_L_PAREN，T_SEMICOLON}
/// FIRST集合不交，可正常识别
/// 最终的文法为：
/// blockItem: statement | varDecl
/// varDecl : T_INT T_ID varDeclList
/// statement:T_RETURN expr T_SEMICOLON | lVal T_ASSIGN expr T_SEMICOLON | block | expr? T_SEMICOLON
/// @return 返回AST的节点
///
static ast_node * BlockItem()
{
    if (F(T_INT)) {
        return varDecl();
    } else {
        return statement();
    }
}

///
/// @brief 块内语句列表识别，文法为BlockItemList : BlockItem+
/// @return AST的节点
///
static void BlockItemList(ast_node * blockNode)
{
    for (;;) {

        // 如果是右大括号，则结束循环，提升效率
        if (F(T_R_BRACE)) {
            break;
        }

        // 遍历BlockItem
        ast_node * itemNode = BlockItem();
        if (itemNode) {
            blockNode->insert_son_node(itemNode);
        } else {
            // 没有，则结束
            break;
        }
    }
}

///
/// @brief 语句块识别，文法：Block -> T_L_BRACE BlockItemList? T_R_BRACE
/// @return AST的节点
///
static ast_node * Block()
{
    if (match(T_L_BRACE)) {

        // 创建语句块节点
        ast_node * blockNode = create_contain_node(ast_operator_type::AST_OP_BLOCK);

        // 空的语句块
        if (match(T_R_BRACE)) {
            return blockNode;
        }

        // 块内语句列表识别
        BlockItemList(blockNode);

        // 没有匹配左大括号，则语法错误
        if (!match(T_R_BRACE)) {
            semerror("缺少右大括号");
        }

        // 正常
        return blockNode;
    }

    // 语法解析失败
    return nullptr;
}

///
/// @brief 形参解析: formalParam: basicType T_ID
/// @return ast_node* 形参节点
///
static ast_node * formalParam()
{
    if (F(T_INT) _(T_VOID)) {
        type_attr type = rd_lval.type;
        advance();

        if (F(T_ID)) {
            var_id_attr id = rd_lval.var_id;
            advance();

            // 创建类型节点
            ast_node * type_node = create_type_node(type);

            // 创建变量ID节点
            ast_node * id_node = ast_node::New(id);

            // 释放字符串空间
            free(id.id);

            // 创建形参节点
            return create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAM, type_node, id_node);
        } else {
            semerror("形参缺少标识符");
        }
    } else {
        semerror("形参缺少类型");
    }

    return nullptr;
}

///
/// @brief 形参列表解析: formalParamList: formalParam (T_COMMA formalParam)*
/// @return ast_node* 形参列表节点
///
static ast_node * formalParamList()
{
    // 创建形参列表节点
    ast_node * params_node = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS);

    // 解析第一个形参
    ast_node * param = formalParam();
    if (!param) {
        ast_node::Delete(params_node);
        return nullptr;
    }

    params_node->insert_son_node(param);

    // 解析后续形参 (T_COMMA formalParam)*
    while (match(T_COMMA)) {
        param = formalParam();
        if (!param) {
            ast_node::Delete(params_node);
            return nullptr;
        }
        params_node->insert_son_node(param);
    }

    return params_node;
}

///
/// @brief 文法分析：idtail : varDeclList | T_L_PAREN formalParamList? T_R_PAREN block
/// @param type 类型 变量类型或函数返回值类型
/// @param id 标识符 变量名或者函数名
///
static ast_node * idtail(type_attr & type, var_id_attr & id)
{
    if (match(T_L_PAREN)) {
        // 函数定义

        ast_node * formalParamsNode = nullptr;

        // 检查是否有形参
        if (F(T_INT) _(T_VOID)) {
            // 有形参，解析形参列表
            formalParamsNode = formalParamList();
            if (!formalParamsNode) {
                return nullptr;
            }
        }

        if (match(T_R_PAREN)) {
            // 识别block
            ast_node * blockNode = Block();

            // 创建函数定义的节点，孩子有类型，函数名，语句块和形参
            // create_func_def函数内会释放id中指向的标识符空间，切记，之后不要再释放，之前一定要是通过strdup函数或者malloc分配的空间
            return create_func_def(type, id, blockNode, formalParamsNode);
        } else {
            semerror("函数定义缺少右小括号");
            if (formalParamsNode) {
                ast_node::Delete(formalParamsNode);
            }
        }

        return nullptr;
    }

    // 这里只能是变量定义
    // 支持数组声明和变量初始化：T_ID (T_L_BRACKET T_DEC_LITERAL T_R_BRACKET)* (T_ASSIGN Expression)?

    // 创建变量声明语句节点
    ast_node * stmt_node = create_contain_node(ast_operator_type::AST_OP_DECL_STMT);
    stmt_node->type = typeAttr2Type(type);

    // 处理数组维度
    ast_node * var_node = ast_node::New(id);
    bool is_array = false;
    
    while (F(T_L_BRACKET)) {
        advance(); // 消费 '['
        if (F(T_DEC_LITERAL)) {
            digit_int_attr array_size_attr = rd_lval.integer_num;
            ast_node * size_node = ast_node::New(array_size_attr);
            advance(); // 消费数字
            if (!match(T_R_BRACKET)) {
                semerror("数组声明缺少右中括号");
                if (var_node) ast_node::Delete(var_node);
                if (size_node) ast_node::Delete(size_node);
                return nullptr;
            }
            // 创建数组声明节点
            var_node = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, var_node, size_node);
            is_array = true;
        } else {
            semerror("数组声明缺少大小");
            if (var_node) ast_node::Delete(var_node);
            return nullptr;
        }
    }
    
    // 处理初始化（如果有）
    if (match(T_ASSIGN)) {
        ast_node * init_expr = logicalOrExpr();
        if (!init_expr) {
            semerror("变量初始化缺少表达式");
            if (var_node) ast_node::Delete(var_node);
            return nullptr;
        }
        // 创建初始化节点
        var_node = create_contain_node(ast_operator_type::AST_OP_VAR_INIT, var_node, init_expr);
    } else if (!is_array) {
        // 普通变量声明
        var_node = create_contain_node(ast_operator_type::AST_OP_VAR_DECL, ast_node::New(typeAttr2Type(type)), var_node);
    }
    
    // 设置类型信息
    if (var_node) {
        var_node->type = typeAttr2Type(type);
        stmt_node->insert_son_node(var_node);
    }

    // 处理后续的变量声明
    varDeclList(stmt_node);

    return stmt_node;
}

// 编译单元识别，也就是文法的开始符号
// 其文法（antlr4中定义的）：
// compileUnit: (funcDef | varDecl)* EOF
// funcDef: T_INT T_ID T_L_PAREN T_R_PAREN block
// varDecl: basicType varDef (T_COMMA varDef)* T_SEMICOLON
// 因funcDef的First集合为T_INT，varDecl的First集合也为T_INT，不可区分，不是LL(1)文法，
// 再看第二个记号，第二个都是标识符，也不可区分
// 再检查第三个记号，funcDef为左小括号，变量声明可以为逗号，可以为等号，可以为分号，可以区分
// 因此可改造为 compileUnit : { T_INT T_ID idtail }，其中大括号代表闭包，类似上面的*
// idtail : varDeclList | T_L_PAREN T_R_PAREN block
// varDeclList可以定义多个变量，每次都增加一个逗号和标识符，直到最后一个记号为分号，即
// varDeclList : T_COMMA T_ID <varDeclList> | T_SEMICOLON
// 因此需要对文法进行改造，以便能够适合LL(1)文法的识别
// 因此改造后的文法为：
// 文法：compileUnit -> { T_INT T_ID idtail } EOF
// idtail : varDeclList | T_L_PAREN T_R_PAREN block
// varDeclList : T_COMMA T_ID varDeclList | T_SEMICOLON
// 闭包代表一个循环，可以0以上的循环，最后一个为EOF
static ast_node * compileUnit()
{
    // 创建AST的根节点，编译单元运算符
    ast_node * cu_node = create_contain_node(ast_operator_type::AST_OP_COMPILE_UNIT);

    for (;;) {
        // match匹配并LookAhead往前挪动
        if (F(T_INT) _(T_VOID)) {
            type_attr type = rd_lval.type;
            // 跳过当前的记号，指向下一个记号
            advance();
            // 检测是否是标识符
            if (F(T_ID)) {
                // 获取标识符的值和定位信息
                var_id_attr id = rd_lval.var_id;
                // 跳过当前的记号，指向下一个记号
                advance();
                // 函数定义的开头为int
                ast_node * node = idtail(type, id);
                // 加入到父节点中，node为空时insert_son_node内部进行了忽略
                (void) cu_node->insert_son_node(node);
            } else {
                semerror("CompileUnit: Expected T_ID after T_INT, got %d (%s)", lookaheadTag, tokenValue.c_str());
                break;
            }
        } else if (F(T_EOF)) {
            // 文件解析完毕
            break;
        } else {
            // If not T_INT (start of varDecl/funcDef) and not T_EOF, then it's an error at this level.
            if (lookaheadTag != RDTokenType::T_EMPTY &&
                lookaheadTag !=
                    RDTokenType::T_ERR) { // Avoid erroring on already reported lexer error or initial empty state
                semerror("CompileUnit: Expected T_INT (for declaration/definition) or T_EOF, got token %d (%s)",
                         lookaheadTag,
                         tokenValue.c_str());
            }
            break;
        }
    }
    return cu_node;
}

///
/// @brief 采用递归下降分析法实现词法与语法分析生成抽象语法树
/// @return ast_node* 空指针失败，否则成功
///
ast_node * rd_parse()
{
    errno_num = 0; // Reset global error count
    advance();     // Get the first token

    ast_node * astRoot = compileUnit(); // Parse the compilation unit

    if (errno_num > 0) { // Check if any error occurred during parsing
        // If astRoot was created but an error occurred, it might be partially formed.
        // Depending on AST node ownership, cleanup might be needed here or handled by caller.
        // For now, just return nullptr as per original logic for error indication.
        return nullptr;
    }
    return astRoot;
}

///
/// @brief 重命名并修改自旧的unaryExp()，支持数组访问
/// @return ast_node*
///
static ast_node * Factor()
{
    ast_node * node = nullptr;

    if (lookaheadTag == RDTokenType::T_DEC_LITERAL || lookaheadTag == RDTokenType::T_HEX_LITERAL ||
        lookaheadTag == RDTokenType::T_OCT_LITERAL) {
        // integer literal
        // rd_lval.integer_num is already populated by rd_flex()
        node = ast_node::New(rd_lval.integer_num);
        advance(); // consume the literal token
    } else if (lookaheadTag == RDTokenType::T_ID) {
        // variable, array access, or function call
        var_id_attr id_attr_val = rd_lval.var_id; // Copy before advance, as rd_lval is global
        advance();                                // consume T_ID
        
        // 创建初始ID节点
        node = ast_node::New(id_attr_val);
        
        // 处理数组访问或函数调用
        while (F(T_L_BRACKET) || F(T_L_PAREN)) {
            if (F(T_L_BRACKET)) {
                // 数组访问
                advance(); // consume '['
                ast_node * index_node = logicalOrExpr(); // 使用完整的表达式解析
                if (!index_node) {
                    semerror("数组访问缺少索引表达式");
                    if (node) ast_node::Delete(node);
                    return nullptr;
                }
                if (!match(T_R_BRACKET)) {
                    semerror("数组访问缺少右中括号");
                    if (node) ast_node::Delete(node);
                    if (index_node) ast_node::Delete(index_node);
                    return nullptr;
                }
                // 创建数组访问节点
                node = create_contain_node(ast_operator_type::AST_OP_ARRAY_REF, node, index_node);
            } else if (F(T_L_PAREN)) {
                // 函数调用
                advance(); // consume '('
                ast_node * params_node = create_contain_node(ast_operator_type::AST_OP_FUNC_REAL_PARAMS);
                realParamList(params_node);
                if (!match(T_R_PAREN)) {
                    semerror("函数调用缺少右括号");
                    if (node) ast_node::Delete(node);
                    if (params_node) ast_node::Delete(params_node);
                    return nullptr;
                }
                // 创建函数调用节点
                node = create_contain_node(ast_operator_type::AST_OP_FUNC_CALL, node, params_node);
                break; // 函数调用后不再处理数组访问
            }
        }
    } else if (lookaheadTag == RDTokenType::T_L_PAREN) {
        advance(); // consume '('
        node = logicalOrExpr(); // 使用完整的表达式解析
        if (!match(RDTokenType::T_R_PAREN)) {
            semerror("Factor: Missing )");
            // Potentially return error node or handle error recovery
            if (node) {
                ast_node::Delete(node);
                node = nullptr;
            }
        }
    } else {
        semerror("Factor: Unexpected token %d (%s)", lookaheadTag, tokenValue.c_str());
    }
    return node;
}

///
/// @brief 重命名并修改自旧的expr()，现在作为算术表达式层
/// @return ast_node*
///
static ast_node * Expression()
{
    ast_node * node = Term(); // Get the first term

    while (lookaheadTag == RDTokenType::T_ADD || lookaheadTag == RDTokenType::T_SUB) {
        RDTokenType op_token = lookaheadTag;
        advance(); // consume operator
        ast_node * right_operand = Term();

        if (!right_operand) {
            semerror("Expression: Missing right operand for operator");
            if (node) {
                ast_node::Delete(node);
                node = nullptr;
            }
            break;
        }

        ast_operator_type ast_op;
        if (op_token == RDTokenType::T_ADD)
            ast_op = ast_operator_type::AST_OP_ADD;
        else
            ast_op = ast_operator_type::AST_OP_SUB;

        node = create_contain_node(ast_op, node, right_operand);
    }
    return node;
}

///
/// @brief 重命名并修改自旧的Term()
/// @return ast_node*
///
static ast_node * Term()
{
    ast_node * node = unaryExpr(); // Get the first unary expression

    while (lookaheadTag == RDTokenType::T_MUL || lookaheadTag == RDTokenType::T_DIV ||
           lookaheadTag == RDTokenType::T_MOD) {
        RDTokenType op_token = lookaheadTag;
        advance(); // consume operator
        ast_node * right_operand = unaryExpr();

        if (!right_operand) {
            semerror("Term: Missing right operand for operator");
            if (node) {
                ast_node::Delete(node);
                node = nullptr;
            }
            break;
        }

        ast_operator_type ast_op;
        if (op_token == RDTokenType::T_MUL)
            ast_op = ast_operator_type::AST_OP_MUL;
        else if (op_token == RDTokenType::T_DIV)
            ast_op = ast_operator_type::AST_OP_DIV;
        else
            ast_op = ast_operator_type::AST_OP_MOD; // T_MOD

        node = create_contain_node(ast_op, node, right_operand);
    }
    return node;
}

///
/// @brief if语句的识别
/// @return AST的节点
///
static ast_node * ifStatement()
{
    if (match(T_IF)) {
        // 左括号
        if (!match(T_L_PAREN)) {
            semerror("if语句缺少左括号");
            return nullptr;
        }
        
        // 条件表达式
        ast_node * cond_node = logicalOrExpr();
        if (!cond_node) {
            semerror("if语句缺少条件表达式");
            return nullptr;
        }
        
        // 右括号
        if (!match(T_R_PAREN)) {
            semerror("if语句缺少右括号");
            if (cond_node) ast_node::Delete(cond_node);
            return nullptr;
        }
        
        // then语句
        ast_node * then_node = statement();
        if (!then_node) {
            semerror("if语句缺少then语句");
            if (cond_node) ast_node::Delete(cond_node);
            return nullptr;
        }
        
        // 检查是否有else
        if (F(T_ELSE)) {
            advance(); // 消费else
            ast_node * else_node = statement();
            if (!else_node) {
                semerror("else语句缺少语句体");
                if (cond_node) ast_node::Delete(cond_node);
                if (then_node) ast_node::Delete(then_node);
                return nullptr;
            }
            
            // 创建带else的if节点
            return create_contain_node(ast_operator_type::AST_OP_IF, cond_node, then_node, else_node);
        } else {
            // 创建不带else的if节点
            return create_contain_node(ast_operator_type::AST_OP_IF, cond_node, then_node);
        }
    }
    return nullptr;
}

///
/// @brief while语句的识别
/// @return AST的节点
///
static ast_node * whileStatement()
{
    if (match(T_WHILE)) {
        // 左括号
        if (!match(T_L_PAREN)) {
            semerror("while语句缺少左括号");
            return nullptr;
        }
        
        // 条件表达式
        ast_node * cond_node = logicalOrExpr();
        if (!cond_node) {
            semerror("while语句缺少条件表达式");
            return nullptr;
        }
        
        // 右括号
        if (!match(T_R_PAREN)) {
            semerror("while语句缺少右括号");
            if (cond_node) ast_node::Delete(cond_node);
            return nullptr;
        }
        
        // 循环体语句
        ast_node * body_node = statement();
        if (!body_node) {
            semerror("while语句缺少循环体");
            if (cond_node) ast_node::Delete(cond_node);
            return nullptr;
        }
        
        // 创建while节点
        return create_contain_node(ast_operator_type::AST_OP_WHILE, cond_node, body_node);
    }
    return nullptr;
}

///
/// @brief break语句的识别
/// @return AST的节点
///
static ast_node * breakStatement()
{
    if (match(T_BREAK)) {
        if (!match(T_SEMICOLON)) {
            semerror("break语句缺少分号");
            return nullptr;
        }
        return create_contain_node(ast_operator_type::AST_OP_BREAK);
    }
    return nullptr;
}

///
/// @brief continue语句的识别
/// @return AST的节点
///
static ast_node * continueStatement()
{
    if (match(T_CONTINUE)) {
        if (!match(T_SEMICOLON)) {
            semerror("continue语句缺少分号");
            return nullptr;
        }
        return create_contain_node(ast_operator_type::AST_OP_CONTINUE);
    }
    return nullptr;
}

// 前向声明扩展表达式函数
static ast_node * logicalOrExpr();
static ast_node * logicalAndExpr();
static ast_node * equalityExpr();
static ast_node * relationalExpr();
static ast_node * unaryExpr();

///
/// @brief 逻辑或表达式
/// @return AST的节点
///
static ast_node * logicalOrExpr()
{
    ast_node * node = logicalAndExpr();
    
    while (F(T_LOGICAL_OR)) {
        advance(); // 消费 ||
        ast_node * right = logicalAndExpr();
        if (!right) {
            semerror("逻辑或运算符缺少右操作数");
            if (node) ast_node::Delete(node);
            return nullptr;
        }
        node = create_contain_node(ast_operator_type::AST_OP_LOGICAL_OR, node, right);
    }
    return node;
}

///
/// @brief 逻辑与表达式
/// @return AST的节点
///
static ast_node * logicalAndExpr()
{
    ast_node * node = equalityExpr();
    
    while (F(T_LOGICAL_AND)) {
        advance(); // 消费 &&
        ast_node * right = equalityExpr();
        if (!right) {
            semerror("逻辑与运算符缺少右操作数");
            if (node) ast_node::Delete(node);
            return nullptr;
        }
        node = create_contain_node(ast_operator_type::AST_OP_LOGICAL_AND, node, right);
    }
    return node;
}

///
/// @brief 相等性表达式
/// @return AST的节点
///
static ast_node * equalityExpr()
{
    ast_node * node = relationalExpr();
    
    while (F(T_EQ) || F(T_NE)) {
        RDTokenType op = lookaheadTag;
        advance(); // 消费 == 或 !=
        ast_node * right = relationalExpr();
        if (!right) {
            semerror("比较运算符缺少右操作数");
            if (node) ast_node::Delete(node);
            return nullptr;
        }
        
        ast_operator_type ast_op = (op == T_EQ) ? ast_operator_type::AST_OP_EQ : ast_operator_type::AST_OP_NE;
        node = create_contain_node(ast_op, node, right);
    }
    return node;
}

///
/// @brief 关系表达式
/// @return AST的节点
///
static ast_node * relationalExpr()
{
    ast_node * node = Expression(); // 使用现有的算术表达式
    
    while (F(T_LT) || F(T_LE) || F(T_GT) || F(T_GE)) {
        RDTokenType op = lookaheadTag;
        advance(); // 消费比较运算符
        ast_node * right = Expression();
        if (!right) {
            semerror("关系运算符缺少右操作数");
            if (node) ast_node::Delete(node);
            return nullptr;
        }
        
        ast_operator_type ast_op;
        switch(op) {
            case T_LT: ast_op = ast_operator_type::AST_OP_LT; break;
            case T_LE: ast_op = ast_operator_type::AST_OP_LE; break;
            case T_GT: ast_op = ast_operator_type::AST_OP_GT; break;
            case T_GE: ast_op = ast_operator_type::AST_OP_GE; break;
            default: ast_op = ast_operator_type::AST_OP_LT; break;
        }
        node = create_contain_node(ast_op, node, right);
    }
    return node;
}

///
/// @brief 一元表达式（支持逻辑非）
/// @return AST的节点
///
static ast_node * unaryExpr()
{
    if (F(T_LOGICAL_NOT)) {
        advance(); // 消费 !
        ast_node * operand = unaryExpr(); // 递归处理一元表达式
        if (!operand) {
            semerror("逻辑非运算符缺少操作数");
            return nullptr;
        }
        return create_contain_node(ast_operator_type::AST_OP_LOGICAL_NOT, operand);
    } else if (F(T_SUB)) {
        advance(); // 消费 -
        ast_node * operand = unaryExpr();
        if (!operand) {
            semerror("负号运算符缺少操作数");
            return nullptr;
        }
        return create_contain_node(ast_operator_type::AST_OP_NEG, operand);
    } else {
        return Factor();
    }
}
