///
/// @file IRGenerator.cpp
/// @brief AST遍历产生线性IR的源文件
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
#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <cassert> // For assert

#include "AST.h"
#include "Common.h"
#include "Function.h"
#include "IRCode.h"
#include "IRGenerator.h"
#include "Module.h"
#include "EntryInstruction.h"
#include "LabelInstruction.h"
#include "ExitInstruction.h"
#include "FuncCallInstruction.h"
#include "BinaryInstruction.h"
#include "MoveInstruction.h"
#include "GotoInstruction.h"
#include "IntegerType.h"
#include "BranchInstruction.h"
#include "ArrayType.h"
#include "PointerType.h"
#include "../Values/ArrayFormalParamLocalVariable.h"

/// @brief 构造函数
/// @param _root AST的根
/// @param _module 符号表
IRGenerator::IRGenerator(ast_node * _root, Module * _module) : root(_root), module(_module)
{
    /* 叶子节点 */
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_LITERAL_UINT] = &IRGenerator::ir_leaf_node_uint;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_VAR_ID] = &IRGenerator::ir_leaf_node_var_id;
    ast2ir_handlers[ast_operator_type::AST_OP_LEAF_TYPE] = &IRGenerator::ir_leaf_node_type;

    /* 表达式运算， 加减乘除模，取负 */
    ast2ir_handlers[ast_operator_type::AST_OP_SUB] = &IRGenerator::ir_sub;
    ast2ir_handlers[ast_operator_type::AST_OP_ADD] = &IRGenerator::ir_add;
    ast2ir_handlers[ast_operator_type::AST_OP_MUL] = &IRGenerator::ir_mul;
    ast2ir_handlers[ast_operator_type::AST_OP_DIV] = &IRGenerator::ir_div;
    ast2ir_handlers[ast_operator_type::AST_OP_MOD] = &IRGenerator::ir_mod;
    ast2ir_handlers[ast_operator_type::AST_OP_NEG] = &IRGenerator::ir_neg;

    /* New: Relational operators */
    ast2ir_handlers[ast_operator_type::AST_OP_LT] = &IRGenerator::ir_lt;
    ast2ir_handlers[ast_operator_type::AST_OP_LE] = &IRGenerator::ir_le;
    ast2ir_handlers[ast_operator_type::AST_OP_GT] = &IRGenerator::ir_gt;
    ast2ir_handlers[ast_operator_type::AST_OP_GE] = &IRGenerator::ir_ge;
    ast2ir_handlers[ast_operator_type::AST_OP_EQ] = &IRGenerator::ir_eq;
    ast2ir_handlers[ast_operator_type::AST_OP_NE] = &IRGenerator::ir_ne;

    /* If statement */
    ast2ir_handlers[ast_operator_type::AST_OP_IF] = &IRGenerator::ir_if_statement;

    /* 语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_ASSIGN] = &IRGenerator::ir_assign;
    ast2ir_handlers[ast_operator_type::AST_OP_RETURN] = &IRGenerator::ir_return;

    /* 函数调用 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_CALL] = &IRGenerator::ir_function_call;

    /* 函数定义 */
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_DEF] = &IRGenerator::ir_function_define;
    ast2ir_handlers[ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS] = &IRGenerator::ir_function_formal_params;

    /* 变量定义语句 */
    ast2ir_handlers[ast_operator_type::AST_OP_DECL_STMT] = &IRGenerator::ir_declare_statment;
    ast2ir_handlers[ast_operator_type::AST_OP_VAR_DECL] = &IRGenerator::ir_variable_declare;
    ast2ir_handlers[ast_operator_type::AST_OP_VAR_INIT] = &IRGenerator::ir_variable_initialize;

    /* 数组相关 */
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_DECL] = &IRGenerator::ir_array_declare;
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_REF] = &IRGenerator::ir_array_ref;
    ast2ir_handlers[ast_operator_type::AST_OP_ARRAY_DIM] = &IRGenerator::ir_array_dim;
    ast2ir_handlers[ast_operator_type::AST_OP_EMPTY_DIM] = &IRGenerator::ir_empty_dim;

    /* 语句块 */
    ast2ir_handlers[ast_operator_type::AST_OP_BLOCK] = &IRGenerator::ir_block;

    /* 编译单元 */
    ast2ir_handlers[ast_operator_type::AST_OP_COMPILE_UNIT] = &IRGenerator::ir_compile_unit;

    /* While statement */
    ast2ir_handlers[ast_operator_type::AST_OP_WHILE] = &IRGenerator::ir_while_statement;

    /* Break statement */
    ast2ir_handlers[ast_operator_type::AST_OP_BREAK] = &IRGenerator::ir_break_statement;

    /* Continue statement */
    ast2ir_handlers[ast_operator_type::AST_OP_CONTINUE] = &IRGenerator::ir_continue_statement;

    /* Logical operators (placeholder handlers as they are mainly used in generate_branch_for_condition) */
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_NOT] = &IRGenerator::ir_logical_not;
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_AND] = &IRGenerator::ir_logical_and;
    ast2ir_handlers[ast_operator_type::AST_OP_LOGICAL_OR] = &IRGenerator::ir_logical_or;
}

/// @brief 遍历抽象语法树产生线性IR，保存到IRCode中
/// @param root 抽象语法树
/// @param IRCode 线性IR
/// @return true: 成功 false: 失败
bool IRGenerator::run()
{
    ast_node * node;

    // 从根节点进行遍历
    node = ir_visit_ast_node(root);

    return node != nullptr;
}

/// @brief 根据AST的节点运算符查找对应的翻译函数并执行翻译动作
/// @param node AST节点
/// @return 成功返回node节点，否则返回nullptr
ast_node * IRGenerator::ir_visit_ast_node(ast_node * node)
{
    // 空节点
    if (nullptr == node) {
        return nullptr;
    }

    bool result;

    std::unordered_map<ast_operator_type, ast2ir_handler_t>::const_iterator pIter;
    pIter = ast2ir_handlers.find(node->node_type);
    if (pIter == ast2ir_handlers.end()) {
        // 没有找到，则说明当前不支持
        result = (this->ir_default)(node);
    } else {
        result = (this->*(pIter->second))(node);
    }

    if (!result) {
        // 语义解析错误，则出错返回
        node = nullptr;
    }

    return node;
}

/// @brief 未知节点类型的节点处理
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_default(ast_node * node)
{
    // 未知的节点
    printf("Unkown node(%d)\n", (int) node->node_type);
    return true;
}

/// @brief 编译单元AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_compile_unit(ast_node * node)
{
    module->setCurrentFunction(nullptr);

    for (auto son: node->sons) {

        // 遍历编译单元，要么是函数定义，要么是语句
        ast_node * son_node = ir_visit_ast_node(son);
        if (!son_node) {
            // TODO 自行追加语义错误处理
            return false;
        }
    }

    return true;
}

/// @brief 函数定义AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_define(ast_node * node)
{
    bool result;

    // 创建一个函数，用于当前函数处理
    if (module->getCurrentFunction()) {
        // 函数中嵌套定义函数，这是不允许的，错误退出
        // TODO 自行追加语义错误处理
        return false;
    }

    // 函数定义的AST包含四个孩子
    // 第一个孩子：函数返回类型
    // 第二个孩子：函数名字
    // 第三个孩子：形参列表
    // 第四个孩子：函数体即block
    ast_node * type_node = node->sons[0];
    ast_node * name_node = node->sons[1];
    ast_node * param_node = node->sons[2];
    ast_node * block_node = node->sons[3];

    // 先处理形参，收集形参信息
    std::vector<FormalParam *> formalParams;
    if (param_node) {
        for (auto paramNode: param_node->sons) {
            // 形参节点包含两个孩子：类型节点和名字节点（或数组声明节点）
            ast_node * typeNode = paramNode->sons[0];
            ast_node * secondNode = paramNode->sons[1];

            FormalParam * formalParam = nullptr;

            // 检查第二个孩子是否为数组声明
            if (secondNode->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
                // 数组形参
                ast_node * nameNode = secondNode->sons[0]; // 数组名
                ast_node * dimNode = secondNode->sons[1];  // 数组维度

                // 获取数组类型信息
                std::vector<int32_t> dimensions;
                if (extractDimensions(dimNode, dimensions)) {
                    // 创建原始数组类型用于显示
                    ArrayType * originalArrayType = ArrayType::getType(typeNode->type, dimensions);

                    // 为实际处理创建指针类型
                    Type * pointerType = PointerType::getType(typeNode->type);

                    // 创建数组形参对象
                    formalParam = new FormalParam(pointerType, nameNode->name, originalArrayType);
                } else {
                    printf("[ERROR] ir_function_define: Failed to extract array dimensions for parameter\n");
                    return false;
                }
            } else {
                // 普通形参
                ast_node * nameNode = secondNode;
                formalParam = new FormalParam(typeNode->type, nameNode->name);
            }

            if (formalParam) {
                formalParams.push_back(formalParam);
            }
        }
    }

    // 创建一个新的函数定义
    Function * newFunc = module->newFunction(name_node->name, type_node->type, formalParams);
    if (!newFunc) {
        // 新定义的函数已经存在，则失败返回。
        // TODO 自行追加语义错误处理
        return false;
    }

    // 当前函数设置有效，变更为当前的函数
    module->setCurrentFunction(newFunc);

    // 进入函数的作用域
    module->enterScope();

    // 获取函数的IR代码列表，用于后面追加指令用，注意这里用的是引用传值
    InterCode & irCode = newFunc->getInterCode();

    // 这里也可增加一个函数入口Label指令，便于后续基本块划分

    // 创建并加入Entry入口指令
    irCode.addInst(new EntryInstruction(newFunc));

    // 创建出口指令并不加入出口指令，等函数内的指令处理完毕后加入出口指令
    LabelInstruction * exitLabelInst = new LabelInstruction(newFunc);

    // 函数出口指令保存到函数信息中，因为在语义分析函数体时return语句需要跳转到函数尾部，需要这个label指令
    newFunc->setExitLabel(exitLabelInst);

    // 遍历形参，没有IR指令，不需要追加
    result = ir_function_formal_params(param_node);
    if (!result) {
        // 形参解析失败
        // TODO 自行追加语义错误处理
        return false;
    }
    node->blockInsts.addInst(param_node->blockInsts);

    // 新建一个Value，用于保存函数的返回值，如果没有返回值可不用申请
    LocalVariable * retValue = nullptr;
    if (!type_node->type->isVoidType()) {

        // 保存函数返回值变量到函数信息中，在return语句翻译时需要设置值到这个变量中
        retValue = static_cast<LocalVariable *>(module->newVarValue(type_node->type));

        // 为main函数初始化返回值为0，避免随机值问题
        if (name_node->name == "main") {
            ConstInt * zeroConst = module->newConstInt(0);
            MoveInstruction * initInst = new MoveInstruction(newFunc, retValue, zeroConst);
            node->blockInsts.addInst(initInst);
        }
    }
    newFunc->setReturnValue(retValue);

    // 函数内已经进入作用域，内部不再需要做变量的作用域管理
    block_node->needScope = false;

    // 遍历block
    result = ir_block(block_node);
    if (!result) {
        // block解析失败
        // TODO 自行追加语义错误处理
        return false;
    }

    // IR指令追加到当前的节点中
    node->blockInsts.addInst(block_node->blockInsts);

    // 此时，所有指令都加入到当前函数中，也就是node->blockInsts

    // node节点的指令移动到函数的IR指令列表中
    irCode.addInst(node->blockInsts);

    // 添加函数出口Label指令，主要用于return语句跳转到这里进行函数的退出
    irCode.addInst(exitLabelInst);

    // 函数出口指令
    irCode.addInst(new ExitInstruction(newFunc, retValue));

    // 恢复成外部函数
    module->setCurrentFunction(nullptr);

    // 退出函数的作用域
    module->leaveScope();

    return true;
}

/// @brief 形式参数AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_formal_params(ast_node * node)
{
    // 如果没有形参节点，直接返回true
    if (!node) {
        return true;
    }

    // 获取当前函数
    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc) {
        return false;
    }

    // 获取函数的形参列表（已经在函数创建时添加）
    std::vector<FormalParam *> & formalParams = currentFunc->getParams();

    // 遍历AST中的形参节点，为每个形参创建对应的局部变量
    for (size_t i = 0; i < node->sons.size() && i < formalParams.size(); ++i) {
        ast_node * paramNode = node->sons[i];

        // 形参节点包含两个孩子：类型节点和名字节点（或数组声明节点）
        ast_node * typeNode = paramNode->sons[0];
        ast_node * secondNode = paramNode->sons[1];

        // 获取对应的形参对象
        FormalParam * formalParam = formalParams[i];

        // 检查第二个孩子是否为数组声明
        if (secondNode->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
            // 数组形参：创建特殊的局部变量声明
            printf("[DEBUG] ir_function_formal_params: Processing array parameter\n");

            ast_node * nameNode = secondNode->sons[0]; // 数组名
            ast_node * dimNode = secondNode->sons[1];  // 数组维度

            // 获取数组类型信息
            std::vector<int32_t> dimensions;
            if (extractDimensions(dimNode, dimensions)) {
                // 创建指针类型用于实际处理
                Type * pointerType = PointerType::getType(typeNode->type);
                
                // 为数组形参创建局部变量，用于在函数内部使用
                LocalVariable * localVar = static_cast<LocalVariable *>(module->newVarValue(pointerType, nameNode->name));
                if (!localVar) {
                    return false;
                }

                // 生成赋值指令，将形参值赋给局部变量
                MoveInstruction * moveInst = new MoveInstruction(currentFunc, localVar, formalParam);
                node->blockInsts.addInst(moveInst);
            } else {
                printf("[ERROR] ir_function_formal_params: Failed to extract array dimensions for parameter\n");
                return false;
            }

        } else {
            // 普通形参：处理为普通局部变量
            printf("[DEBUG] ir_function_formal_params: Processing regular parameter\n");

            ast_node * nameNode = secondNode;

            // 为形参创建局部变量，用于在函数内部使用
            LocalVariable * localVar =
                static_cast<LocalVariable *>(module->newVarValue(typeNode->type, nameNode->name));
            if (!localVar) {
                return false;
            }

            // 生成赋值指令，将形参值赋给局部变量
            // 这里使用Move指令将形参值复制到局部变量
            MoveInstruction * moveInst = new MoveInstruction(currentFunc, localVar, formalParam);
            node->blockInsts.addInst(moveInst);
        }
    }

    return true;
}

/// @brief 函数调用AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_function_call(ast_node * node)
{
    std::vector<Value *> realParams;

    // 获取当前正在处理的函数
    Function * currentFunc = module->getCurrentFunction();

    // 函数调用的节点包含两个节点：
    // 第一个节点：函数名节点
    // 第二个节点：实参列表节点

    std::string funcName = node->sons[0]->name;
    int64_t lineno = node->sons[0]->line_no;

    ast_node * paramsNode = node->sons[1];

    // 根据函数名查找函数，看是否存在。若不存在则出错
    // 这里约定函数必须先定义后使用
    auto calledFunction = module->findFunction(funcName);
    if (nullptr == calledFunction) {
        minic_log(LOG_ERROR, "函数(%s)未定义或声明", funcName.c_str());
        return false;
    }

    // 当前函数存在函数调用
    currentFunc->setExistFuncCall(true);

    // 如果没有孩子，也认为是没有参数
    if (!paramsNode->sons.empty()) {

        int32_t argsCount = (int32_t) paramsNode->sons.size();

        // 当前函数中调用函数实参个数最大值统计，实际上是统计实参传参需在栈中分配的大小
        // 因为目前的语言支持的int和float都是四字节的，只统计个数即可
        if (argsCount > currentFunc->getMaxFuncCallArgCnt()) {
            currentFunc->setMaxFuncCallArgCnt(argsCount);
        }

        // 遍历参数列表，孩子是表达式
        // 这里自左往右计算表达式
        for (auto son: paramsNode->sons) {

            // 遍历Block的每个语句，进行显示或者运算
            ast_node * temp = ir_visit_ast_node(son);
            if (!temp) {
                return false;
            }

            realParams.push_back(temp->val);
            node->blockInsts.addInst(temp->blockInsts);
        }
    }

    // TODO 这里请追加函数调用的语义错误检查，这里只进行了函数参数的个数检查等，其它请自行追加。
    if (realParams.size() != calledFunction->getParams().size()) {
        // 函数参数的个数不一致，语义错误
        minic_log(LOG_ERROR, "第%lld行的被调用函数(%s)未定义或声明", (long long) lineno, funcName.c_str());
        return false;
    }

    // 返回调用有返回值，则需要分配临时变量，用于保存函数调用的返回值
    Type * type = calledFunction->getReturnType();

    FuncCallInstruction * funcCallInst = new FuncCallInstruction(currentFunc, calledFunction, realParams, type);

    // 创建函数调用指令
    node->blockInsts.addInst(funcCallInst);

    // 函数调用结果Value保存到node中，可能为空，上层节点可利用这个值
    node->val = funcCallInst;

    return true;
}

/// @brief 语句块（含函数体）AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_block(ast_node * node)
{
    // 进入作用域
    if (node->needScope) {
        module->enterScope();
    }

    std::vector<ast_node *>::iterator pIter;
    for (pIter = node->sons.begin(); pIter != node->sons.end(); ++pIter) {

        // 遍历Block的每个语句，进行显示或者运算
        ast_node * temp = ir_visit_ast_node(*pIter);
        if (!temp) {
            return false;
        }

        node->blockInsts.addInst(temp->blockInsts);
    }

    // 离开作用域
    if (node->needScope) {
        module->leaveScope();
    }

    return true;
}

/// @brief 整数加法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_add(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 加法节点，左结合，先计算左节点，后计算右节点

    // 加法的左边操作数
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        // 某个变量没有定值
        return false;
    }

    // 加法的右边操作数
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    BinaryInstruction * addInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_ADD_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(addInst);

    node->val = addInst;

    return true;
}

/// @brief 整数减法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_sub(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    // 减法节点，左结合，先计算左节点，后计算右节点
    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left) {
        return false;
    }

    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right) {
        return false;
    }

    BinaryInstruction * subInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_SUB_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(subInst);
    node->val = subInst;

    return true;
}

/// @brief 赋值AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_assign(ast_node * node)
{
    ast_node * son1_node = node->sons[0];
    ast_node * son2_node = node->sons[1];

    // 赋值节点，自右往左运算

    // 赋值运算符的左侧操作数
    ast_node * left = ir_visit_ast_node(son1_node);
    if (!left) {
        // 某个变量没有定值
        // 这里缺省设置变量不存在则创建，因此这里不会错误
        return false;
    }

    // 赋值运算符的右侧操作数
    ast_node * right = ir_visit_ast_node(son2_node);
    if (!right) {
        // 某个变量没有定值
        return false;
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理

    // 创建临时变量保存IR的值，以及线性IR指令
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(left->blockInsts);

    Instruction * assignInst = nullptr;

    // 检查左操作数是否是数组访问（指针类型）
    if (son1_node->node_type == ast_operator_type::AST_OP_ARRAY_REF) {
        // 数组赋值：*ptr = value
        // 在DragonIR中，使用MoveInstruction进行内存写入，left->val是地址，right->val是值
        assignInst = new MoveInstruction(module->getCurrentFunction(), left->val, right->val);
        printf(
            "[DEBUG] ir_assign: Generated move instruction for array element assignment (address = %s, value = %s)\n",
            left->val->getIRName().c_str(),
            right->val->getIRName().c_str());
    } else {
        // 普通变量赋值：var = value
        assignInst = new MoveInstruction(module->getCurrentFunction(), left->val, right->val);
        printf("[DEBUG] ir_assign: Generated move instruction for variable assignment\n");
    }

    node->blockInsts.addInst(assignInst);

    // 这里假定赋值的类型是一致的
    node->val = assignInst;

    return true;
}

/// @brief return节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_return(ast_node * node)
{
    ast_node * right = nullptr;

    // return语句可能没有没有表达式，也可能有，因此这里必须进行区分判断
    if (!node->sons.empty()) {

        ast_node * son_node = node->sons[0];

        // 返回的表达式的指令保存在right节点中
        right = ir_visit_ast_node(son_node);
        if (!right) {

            // 某个变量没有定值
            return false;
        }
    }

    // 这里只处理整型的数据，如需支持实数，则需要针对类型进行处理
    Function * currentFunc = module->getCurrentFunction();

    // 返回值存在时则移动指令到node中
    if (right) {

        // 创建临时变量保存IR的值，以及线性IR指令
        node->blockInsts.addInst(right->blockInsts);

        // 返回值赋值到函数返回值变量上，然后跳转到函数的尾部
        node->blockInsts.addInst(new MoveInstruction(currentFunc, currentFunc->getReturnValue(), right->val));

        node->val = right->val;
    } else {
        // 没有返回值
        node->val = nullptr;
    }

    // 跳转到函数的尾部出口指令上
    node->blockInsts.addInst(new GotoInstruction(currentFunc, currentFunc->getExitLabel()));

    return true;
}

/// @brief 类型叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_type(ast_node * node)
{
    // 不需要做什么，直接从节点中获取即可。

    return true;
}

/// @brief 标识符叶子节点翻译成线性中间IR，变量声明的不走这个语句
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_var_id(ast_node * node)
{
    Value * val;

    // 查找ID型Value
    // 变量，则需要在符号表中查找对应的值

    val = module->findVarValue(node->name);

    node->val = val;

    return true;
}

/// @brief 无符号整数字面量叶子节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_leaf_node_uint(ast_node * node)
{
    ConstInt * val;

    // 新建一个整数常量Value
    val = module->newConstInt((int32_t) node->integer_val);

    node->val = val;

    return true;
}

/// @brief 变量声明语句节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_declare_statment(ast_node * node)
{
    bool result = false;

    for (auto & child: node->sons) {

        // 根据子节点类型处理变量声明或初始化
        printf("[DEBUG] ir_declare_statment: Processing child node type: %d\n", (int) child->node_type);
        if (child->node_type == ast_operator_type::AST_OP_VAR_DECL) {
            result = ir_variable_declare(child);
        } else if (child->node_type == ast_operator_type::AST_OP_VAR_INIT) {
            result = ir_variable_initialize(child);
        } else if (child->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
            printf("[DEBUG] ir_declare_statment: Calling ir_array_declare\n");
            result = ir_array_declare(child);
        } else {
            printf("[ERROR] ir_declare_statment: Unknown child node type: %d\n", (int) child->node_type);
            result = false;
        }

        if (!result) {
            break;
        }

        // 添加子节点的指令到当前节点
        node->blockInsts.addInst(child->blockInsts);
    }

    return result;
}

/// @brief 变量定声明节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_variable_declare(ast_node * node)
{
    // 共有两个孩子，第一个类型，第二个变量名

    // TODO 这里可强化类型等检查

    node->val = module->newVarValue(node->sons[0]->type, node->sons[1]->name);

    return true;
}

/// @brief 新增：整数乘法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mul(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left)
        return false;
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right)
        return false;

    BinaryInstruction * mulInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_MUL_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(mulInst);
    node->val = mulInst;

    return true;
}

/// @brief 新增：整数有符号除法AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_div(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left)
        return false;
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right)
        return false;

    // TODO: 考虑除零错误的处理，可以在语义分析或这里添加运行时检查

    BinaryInstruction * divInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_DIV_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(divInst);
    node->val = divInst;

    return true;
}

/// @brief 新增：整数有符号求余AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_mod(ast_node * node)
{
    ast_node * src1_node = node->sons[0];
    ast_node * src2_node = node->sons[1];

    ast_node * left = ir_visit_ast_node(src1_node);
    if (!left)
        return false;
    ast_node * right = ir_visit_ast_node(src2_node);
    if (!right)
        return false;

    // TODO: 考虑除零错误的处理

    BinaryInstruction * remInst = new BinaryInstruction(module->getCurrentFunction(),
                                                        IRInstOperator::IRINST_OP_REM_I,
                                                        left->val,
                                                        right->val,
                                                        IntegerType::getTypeInt());

    node->blockInsts.addInst(left->blockInsts);
    node->blockInsts.addInst(right->blockInsts);
    node->blockInsts.addInst(remInst);
    node->val = remInst;

    return true;
}

/// @brief 新增：单目取负AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_neg(ast_node * node)
{
    assert(node->node_type == ast_operator_type::AST_OP_NEG && "Node is not neg"); // Restored AST check
    ast_node * operand_node = node->sons[0];                                       // Restored AST check

    ast_node * visited_operand_node = ir_visit_ast_node(operand_node); // Restored AST check
    if (!visited_operand_node) {
        return false;
    }
    node->blockInsts.addInst(
        visited_operand_node->blockInsts); // Assumed addInst(InterCode) exists or will be fixed if error persists
    Value * original_operand_val = visited_operand_node->val;
    if (!original_operand_val) {
        printf("[ERROR] ir_neg: Operand for unary minus did not produce a value. Line: %ld\n", node->line_no);
        fflush(stdout);
        return false;
    }

    Type * operand_type = original_operand_val->getType();
    Value * operand_for_sub = nullptr;
    Function * currentFunc = module->getCurrentFunction();
    ConstInt * const_zero_i32 = module->newConstInt(0);

    if (!const_zero_i32) {
        printf("[ERROR] ir_neg: Failed to create i32 zero constant. Line: %ld\n", node->line_no);
        fflush(stdout);
        return false;
    }

    if (operand_type->isInt1Byte()) {
        printf("[DEBUG] ir_neg: Operand is i1 (%s). Converting to i32. Line: %ld\n",
               original_operand_val->getIRName().c_str(),
               node->line_no);
        fflush(stdout);

        LocalVariable * temp_i32_storage = static_cast<LocalVariable *>(
            currentFunc->newLocalVarValue(IntegerType::getTypeInt(), "neg_i1_to_i32_val")); // Restored naming
        if (!temp_i32_storage) {
            printf("[ERROR] ir_neg: Failed to create local variable for i1->i32 conversion. Line: %ld\n",
                   node->line_no);
            fflush(stdout);
            return false;
        }

        LabelInstruction * set_one_label = new LabelInstruction(currentFunc);
        LabelInstruction * set_zero_label = new LabelInstruction(currentFunc);
        LabelInstruction * continue_label = new LabelInstruction(currentFunc);

        node->blockInsts.addInst(
            new BranchInstruction(currentFunc, original_operand_val, set_one_label, set_zero_label));

        node->blockInsts.addInst(set_one_label);
        node->blockInsts.addInst(new MoveInstruction(currentFunc, temp_i32_storage, module->newConstInt(1)));
        node->blockInsts.addInst(new GotoInstruction(currentFunc, continue_label));

        node->blockInsts.addInst(set_zero_label);
        node->blockInsts.addInst(new MoveInstruction(currentFunc, temp_i32_storage, module->newConstInt(0)));
        node->blockInsts.addInst(new GotoInstruction(currentFunc, continue_label));

        node->blockInsts.addInst(continue_label);
        operand_for_sub = temp_i32_storage;

    } else if (operand_type->isInt32Type()) {
        operand_for_sub = original_operand_val;
    } else {
        printf("[ERROR] ir_neg: Operand for unary minus is not i1 or i32 (type: %s). Line: %ld\n",
               operand_type->toString().c_str(),
               node->line_no);
        fflush(stdout);
        return false;
    }

    Instruction * negInst = new BinaryInstruction(currentFunc,
                                                  IRInstOperator::IRINST_OP_SUB_I,
                                                  const_zero_i32,
                                                  operand_for_sub,
                                                  IntegerType::getTypeInt());

    node->blockInsts.addInst(negInst);
    node->val = negInst;

    printf("[DEBUG] ir_neg: Completed. Result value is %s. Line: %ld\n", node->val->getIRName().c_str(), node->line_no);
    fflush(stdout);
    return true;
}

/// @brief 新增：整数小于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_lt(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_LT_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：整数小于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_le(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_LE_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：整数大于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_gt(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_GT_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：整数大于等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_ge(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_GE_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：整数等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_eq(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_EQ_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：整数不等于比较AST节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_ne(ast_node * node)
{
    if (!node || node->sons.size() != 2)
        return false;
    ast_node * left_son_visited = ir_visit_ast_node(node->sons[0]);
    if (!left_son_visited || !left_son_visited->val)
        return false;
    ast_node * right_son_visited = ir_visit_ast_node(node->sons[1]);
    if (!right_son_visited || !right_son_visited->val)
        return false;

    Function * currentFunc = module->getCurrentFunction();
    if (!currentFunc)
        return false;

    Type * resultType = IntegerType::getTypeBool();
    Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                   IRInstOperator::IRINST_OP_CMP_NE_I,
                                                   left_son_visited->val,
                                                   right_son_visited->val,
                                                   resultType);
    node->blockInsts.addInst(left_son_visited->blockInsts);
    node->blockInsts.addInst(right_son_visited->blockInsts);
    node->blockInsts.addInst(cmp_inst);
    node->val = cmp_inst;
    return true;
}

/// @brief 新增：if语句翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_if_statement(ast_node * node)
{
    // Assuming ast_node has line_no. If not, remove node->line_no from this log.
    printf("[DEBUG] ir_if_statement: Entered for node type %d (line %ld approx)\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    if (!node || (node->sons.size() != 2 && node->sons.size() != 3)) {
        printf("[ERROR] ir_if_statement: Invalid node or son count (%zu)\n", node ? node->sons.size() : 0);
        fflush(stdout);
        return false;
    }

    Function * currentFunc = module->getCurrentFunction();
    printf("[DEBUG] ir_if_statement: currentFunc = %p\n", (void *) currentFunc);
    fflush(stdout);
    assert(currentFunc != nullptr);

    ast_node * condition_node = node->sons[0];
    ast_node * then_block_node = node->sons[1];
    ast_node * else_block_node = (node->sons.size() == 3) ? node->sons[2] : nullptr;
    printf("[DEBUG] ir_if_statement: condition_node=%p, then_block_node=%p, else_block_node=%p\n",
           (void *) condition_node,
           (void *) then_block_node,
           (void *) else_block_node);
    fflush(stdout);
    assert(condition_node != nullptr);
    assert(then_block_node != nullptr);
    // else_block_node can be nullptr

    // Create labels
    LabelInstruction * true_label = new LabelInstruction(currentFunc);
    printf("[DEBUG] ir_if_statement: true_label = %p (IR name: %s)\n",
           (void *) true_label,
           true_label->getIRName().c_str());
    fflush(stdout);
    assert(true_label != nullptr);

    LabelInstruction * false_label_for_bc = nullptr; // This will be the actual label passed to BranchInstruction
    LabelInstruction * actual_false_block_label =
        nullptr; // This is the label for the start of the else block if it exists

    LabelInstruction * endif_label = new LabelInstruction(currentFunc);
    printf("[DEBUG] ir_if_statement: endif_label = %p (IR name: %s)\n",
           (void *) endif_label,
           endif_label->getIRName().c_str());
    fflush(stdout);
    assert(endif_label != nullptr);

    if (else_block_node) {
        actual_false_block_label = new LabelInstruction(currentFunc);
        printf("[DEBUG] ir_if_statement: actual_false_block_label (for else block) = %p (IR name: %s)\n",
               (void *) actual_false_block_label,
               actual_false_block_label->getIRName().c_str());
        fflush(stdout);
        assert(actual_false_block_label != nullptr);
        false_label_for_bc = actual_false_block_label;
    } else {
        false_label_for_bc = endif_label; // For if-then, false condition goes to endif
    }
    printf("[DEBUG] ir_if_statement: false_label_for_bc (passed to BranchInst) = %p (IR name: %s)\n",
           (void *) false_label_for_bc,
           false_label_for_bc->getIRName().c_str());
    fflush(stdout);
    assert(false_label_for_bc != nullptr);

    // 1. Translate condition expression and generate branch
    printf("[DEBUG] ir_if_statement: Calling generate_branch_for_condition for condition_node %p\n",
           (void *) condition_node);
    fflush(stdout);
    generate_branch_for_condition(condition_node, true_label, false_label_for_bc, node->blockInsts);
    printf("[DEBUG] ir_if_statement: Returned from generate_branch_for_condition\n");
    fflush(stdout);

    // 2. Add true_label, translate then_block, and add goto endif_label
    printf("[DEBUG] ir_if_statement: Adding true_label %p to blockInsts\n", (void *) true_label);
    fflush(stdout);
    node->blockInsts.addInst(true_label);
    printf("[DEBUG] ir_if_statement: Visiting then_block_node %p\n", (void *) then_block_node);
    fflush(stdout);
    ast_node * then_block_visited = ir_visit_ast_node(then_block_node);
    printf("[DEBUG] ir_if_statement: then_block_visited = %p\n", (void *) then_block_visited);
    fflush(stdout);
    assert(then_block_visited != nullptr);
    node->blockInsts.addInst(then_block_visited->blockInsts);

    printf("[DEBUG] ir_if_statement: Creating GotoInstruction to endif_label %p\n", (void *) endif_label);
    fflush(stdout);
    GotoInstruction * gotoEndif = new GotoInstruction(currentFunc, endif_label);
    printf("[DEBUG] ir_if_statement: GotoInstruction created: %p\n", (void *) gotoEndif);
    fflush(stdout);
    node->blockInsts.addInst(gotoEndif);

    // 4. If there's an else_block, add actual_false_block_label and translate else_block
    if (else_block_node) {
        assert(actual_false_block_label != nullptr); // Should have been created
        printf("[DEBUG] ir_if_statement: Adding actual_false_block_label %p to blockInsts\n",
               (void *) actual_false_block_label);
        fflush(stdout);
        node->blockInsts.addInst(actual_false_block_label);

        printf("[DEBUG] ir_if_statement: Visiting else_block_node %p\n", (void *) else_block_node);
        fflush(stdout);
        ast_node * else_block_visited = ir_visit_ast_node(else_block_node);
        printf("[DEBUG] ir_if_statement: else_block_visited = %p\n", (void *) else_block_visited);
        fflush(stdout);
        assert(else_block_visited != nullptr);
        node->blockInsts.addInst(else_block_visited->blockInsts);
    }

    // 5. Add endif_label
    printf("[DEBUG] ir_if_statement: Adding endif_label %p to blockInsts\n", (void *) endif_label);
    fflush(stdout);
    node->blockInsts.addInst(endif_label);

    node->val = nullptr;
    printf("[DEBUG] ir_if_statement: Exiting successfully\n");
    fflush(stdout);
    return true;
}

void IRGenerator::generate_branch_for_condition(ast_node * condition_node,
                                                LabelInstruction * true_target,
                                                LabelInstruction * false_target,
                                                InterCode & instruction_list)
{
    assert(condition_node != nullptr);
    assert(true_target != nullptr);
    assert(false_target != nullptr);
    assert(module->getCurrentFunction() != nullptr &&
           "generate_branch_for_condition called outside of a function context");

    Function * currentFunc = module->getCurrentFunction();

    switch (condition_node->node_type) {
        case ast_operator_type::AST_OP_EQ:
        case ast_operator_type::AST_OP_NE:
        case ast_operator_type::AST_OP_LT:
        case ast_operator_type::AST_OP_LE:
        case ast_operator_type::AST_OP_GT:
        case ast_operator_type::AST_OP_GE: {
            // This logic is similar to what was in ir_... comparison functions
            // and parts of ir_if_statement before.
            ast_node * left_son_visited = ir_visit_ast_node(condition_node->sons[0]);
            if (!left_son_visited || !left_son_visited->val) {
                // Handle error or assert
                // For now, let's assume an error print and return or throw
                printf("[ERROR] generate_branch_for_condition: Left operand of comparison is null.\n");
                fflush(stdout);
                // Potentially throw an exception or return an error code if function signature allows
                return;
            }
            ast_node * right_son_visited = ir_visit_ast_node(condition_node->sons[1]);
            if (!right_son_visited || !right_son_visited->val) {
                printf("[ERROR] generate_branch_for_condition: Right operand of comparison is null.\n");
                fflush(stdout);
                return;
            }

            instruction_list.addInst(left_son_visited->blockInsts);
            instruction_list.addInst(right_son_visited->blockInsts);

            IRInstOperator op;
            switch (condition_node->node_type) {
                case ast_operator_type::AST_OP_EQ:
                    op = IRInstOperator::IRINST_OP_CMP_EQ_I;
                    break;
                case ast_operator_type::AST_OP_NE:
                    op = IRInstOperator::IRINST_OP_CMP_NE_I;
                    break;
                case ast_operator_type::AST_OP_LT:
                    op = IRInstOperator::IRINST_OP_CMP_LT_I;
                    break;
                case ast_operator_type::AST_OP_LE:
                    op = IRInstOperator::IRINST_OP_CMP_LE_I;
                    break;
                case ast_operator_type::AST_OP_GT:
                    op = IRInstOperator::IRINST_OP_CMP_GT_I;
                    break;
                case ast_operator_type::AST_OP_GE:
                    op = IRInstOperator::IRINST_OP_CMP_GE_I;
                    break;
                default: // Should not happen due to outer switch
                    printf("[FATAL] generate_branch_for_condition: Unexpected comparison node_type.\n");
                    fflush(stdout);
                    assert(false);
                    return;
            }

            Instruction * cmp_inst = new BinaryInstruction(currentFunc,
                                                           op,
                                                           left_son_visited->val,
                                                           right_son_visited->val,
                                                           IntegerType::getTypeBool());
            instruction_list.addInst(cmp_inst);

            BranchInstruction * branch_inst = new BranchInstruction(currentFunc, cmp_inst, true_target, false_target);
            instruction_list.addInst(branch_inst);
            break;
        }

        case ast_operator_type::AST_OP_LOGICAL_NOT: {
            printf("[DEBUG] generate_branch_for_condition: Handling AST_OP_LOGICAL_NOT\n");
            fflush(stdout);
            assert(condition_node->sons.size() == 1 && "Logical NOT should have one operand");
            ast_node * operand_node = condition_node->sons[0];
            // Recursively call, but swap true and false targets
            generate_branch_for_condition(operand_node, false_target, true_target, instruction_list);
            break;
        }

        case ast_operator_type::AST_OP_LOGICAL_AND: {
            printf("[DEBUG] generate_branch_for_condition: Handling AST_OP_LOGICAL_AND\n");
            fflush(stdout);
            assert(condition_node->sons.size() == 2 && "Logical AND should have two operands");
            ast_node * expr1_node = condition_node->sons[0];
            ast_node * expr2_node = condition_node->sons[1];

            LabelInstruction * check_expr2_label = new LabelInstruction(currentFunc);
            assert(check_expr2_label != nullptr);

            // Evaluate expr1. If false, jump to overall false_target (short-circuit).
            // If true, jump to check_expr2_label to evaluate expr2.
            generate_branch_for_condition(expr1_node, check_expr2_label, false_target, instruction_list);

            // Add the label for evaluating expr2
            instruction_list.addInst(check_expr2_label);
            printf("[DEBUG] generate_branch_for_condition (AND): Added check_expr2_label %s\n",
                   check_expr2_label->getIRName().c_str());
            fflush(stdout);

            // Evaluate expr2. Its truthiness determines the overall result.
            generate_branch_for_condition(expr2_node, true_target, false_target, instruction_list);
            break;
        }

        case ast_operator_type::AST_OP_LOGICAL_OR: {
            printf("[DEBUG] generate_branch_for_condition: Handling AST_OP_LOGICAL_OR\n");
            fflush(stdout);
            assert(condition_node->sons.size() == 2 && "Logical OR should have two operands");
            ast_node * expr1_node = condition_node->sons[0];
            ast_node * expr2_node = condition_node->sons[1];

            LabelInstruction * check_expr2_label = new LabelInstruction(currentFunc);
            assert(check_expr2_label != nullptr);

            // Evaluate expr1. If true, jump to overall true_target (short-circuit).
            // If false, jump to check_expr2_label to evaluate expr2.
            generate_branch_for_condition(expr1_node, true_target, check_expr2_label, instruction_list);

            // Add the label for evaluating expr2
            instruction_list.addInst(check_expr2_label);
            printf("[DEBUG] generate_branch_for_condition (OR): Added check_expr2_label %s\n",
                   check_expr2_label->getIRName().c_str());
            fflush(stdout);

            // Evaluate expr2. Its truthiness determines the overall result.
            generate_branch_for_condition(expr2_node, true_target, false_target, instruction_list);
            break;
        }

            // TODO: Add case for direct variable/literal boolean values (int to bool conversion)

        default:
            // Fallback for other types of conditions that might evaluate to a value directly
            // This is where int->bool conversion for existing variables/values would also be needed.
            // For now, assume condition_node itself results in a Value*
            // This part will need refinement for int to bool conversion.
            printf("[DEBUG] generate_branch_for_condition: Default case for node type %d. Visiting node.\n",
                   (int) condition_node->node_type);
            fflush(stdout);
            ast_node * cond_val_visited = ir_visit_ast_node(condition_node);
            if (!cond_val_visited || !cond_val_visited->val) {
                printf("[ERROR] generate_branch_for_condition: Condition node evaluation resulted in null value.\n");
                fflush(stdout);
                return;
            }
            instruction_list.addInst(cond_val_visited->blockInsts);
            Value * cond_value = cond_val_visited->val;

            // Here we need to handle the case where cond_value is i32 and needs to be compared against 0 for i1.
            // For now, if it's already i1, use it. If it's i32, this will be problematic without conversion.
            if (cond_value->getType()->isInt1Byte()) { // isInt1Byte() is actually isInt1Bit()
                BranchInstruction * branch_inst =
                    new BranchInstruction(currentFunc, cond_value, true_target, false_target);
                instruction_list.addInst(branch_inst);
            } else if (cond_value->getType()->isInt32Type()) {
                // Perform implicit bool conversion: value != 0
                // %temp_i1 = cmp ne %value_i32, 0
                // bc %temp_i1, true_target, false_target
                printf("[DEBUG] generate_branch_for_condition: Condition is i32 (%s). Performing implicit conversion "
                       "to i1 (val != 0).\n",
                       cond_value->getIRName().c_str());
                fflush(stdout);
                ConstInt * zero_i32 = module->newConstInt(0);
                assert(zero_i32 != nullptr && "Failed to create ConstInt(0) for i32 to i1 conversion");

                Instruction * ne_zero_inst = new BinaryInstruction(currentFunc,
                                                                   IRInstOperator::IRINST_OP_CMP_NE_I,
                                                                   cond_value,
                                                                   zero_i32,
                                                                   IntegerType::getTypeBool());
                instruction_list.addInst(ne_zero_inst);

                BranchInstruction * branch_inst =
                    new BranchInstruction(currentFunc, ne_zero_inst, true_target, false_target);
                instruction_list.addInst(branch_inst);
            } else {
                printf("[ERROR] generate_branch_for_condition: Unsupported type for direct condition: %s\n",
                       cond_value->getType()->toString().c_str());
                fflush(stdout);
                // Or assert(false)
            }
            break;
    }
}

/// @brief 新增：while语句翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_while_statement(ast_node * node)
{
    assert(node != nullptr && node->node_type == ast_operator_type::AST_OP_WHILE);
    assert(node->sons.size() == 2 && "While node should have 2 children: condition and body");

    printf("[DEBUG] ir_while_statement: Entered for node type %d (line %ld approx)\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    Function * currentFunc = module->getCurrentFunction();
    assert(currentFunc != nullptr && "ir_while_statement called outside of a function context");

    ast_node * condition_node = node->sons[0];
    ast_node * body_node = node->sons[1];

    assert(condition_node != nullptr);
    assert(body_node != nullptr);

    // 1. Create labels
    LabelInstruction * condition_check_label = new LabelInstruction(currentFunc); // Loop entry / condition check
    LabelInstruction * body_entry_label = new LabelInstruction(currentFunc);      // Loop body entry
    LabelInstruction * loop_exit_label = new LabelInstruction(currentFunc);       // Loop exit

    assert(condition_check_label != nullptr);
    assert(body_entry_label != nullptr);
    assert(loop_exit_label != nullptr);

    printf("[DEBUG] ir_while_statement: condition_check_label=%s, body_entry_label=%s, loop_exit_label=%s\n",
           condition_check_label->getIRName().c_str(),
           body_entry_label->getIRName().c_str(),
           loop_exit_label->getIRName().c_str());
    fflush(stdout);

    // Push loop labels for break/continue
    loop_label_stack.push({condition_check_label, loop_exit_label});
    printf("[DEBUG] ir_while_statement: Pushed to loop_label_stack: continue_target=%s, break_target=%s. Stack size: "
           "%zu\n",
           condition_check_label->getIRName().c_str(),
           loop_exit_label->getIRName().c_str(),
           loop_label_stack.size());
    fflush(stdout);

    // 2. Add condition_check_label to instruction stream
    node->blockInsts.addInst(condition_check_label);
    printf("[DEBUG] ir_while_statement: Added condition_check_label %s to blockInsts\n",
           condition_check_label->getIRName().c_str());
    fflush(stdout);

    // 3. Generate branch based on condition
    printf("[DEBUG] ir_while_statement: Calling generate_branch_for_condition for condition_node %p\n",
           (void *) condition_node);
    fflush(stdout);
    generate_branch_for_condition(condition_node, body_entry_label, loop_exit_label, node->blockInsts);
    printf("[DEBUG] ir_while_statement: Returned from generate_branch_for_condition\n");
    fflush(stdout);

    // 4. Add body_entry_label
    node->blockInsts.addInst(body_entry_label);
    printf("[DEBUG] ir_while_statement: Added body_entry_label %s to blockInsts\n",
           body_entry_label->getIRName().c_str());
    fflush(stdout);

    // 5. Translate loop body
    printf("[DEBUG] ir_while_statement: Visiting body_node %p\n", (void *) body_node);
    fflush(stdout);
    ast_node * body_visited = ir_visit_ast_node(body_node);
    if (!body_visited) {
        printf("[ERROR] ir_while_statement: Visiting body_node failed.\n");
        fflush(stdout);
        loop_label_stack.pop(); // Pop labels on error exit
        printf("[DEBUG] ir_while_statement: Popped from loop_label_stack on error. Stack size: %zu\n",
               loop_label_stack.size());
        // module->getLoopContextManager().pop(); // Clean up loop context
        return false; // Propagate error
    }
    node->blockInsts.addInst(body_visited->blockInsts); // Add instructions from the body
    printf("[DEBUG] ir_while_statement: Returned from visiting body_node\n");
    fflush(stdout);

    // 6. Add unconditional jump back to condition_check_label
    GotoInstruction * goto_condition_check = new GotoInstruction(currentFunc, condition_check_label);
    node->blockInsts.addInst(goto_condition_check);
    printf("[DEBUG] ir_while_statement: Added goto_condition_check to %s\n",
           condition_check_label->getIRName().c_str());
    fflush(stdout);

    // 7. Add loop_exit_label
    node->blockInsts.addInst(loop_exit_label);
    printf("[DEBUG] ir_while_statement: Added loop_exit_label %s to blockInsts\n",
           loop_exit_label->getIRName().c_str());
    fflush(stdout);

    // TODO: Pop loop labels for break/continue handling
    // module->getLoopContextManager().pop();
    loop_label_stack.pop();
    printf("[DEBUG] ir_while_statement: Popped from loop_label_stack on normal exit. Stack size: %zu\n",
           loop_label_stack.size());
    fflush(stdout);

    node->val = nullptr; // While statement itself doesn't have a value
    printf("[DEBUG] ir_while_statement: Exiting successfully.\n");
    fflush(stdout);
    return true;
}

/// @brief 新增：break语句翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_break_statement(ast_node * node)
{
    assert(node != nullptr && node->node_type == ast_operator_type::AST_OP_BREAK);
    printf("[DEBUG] ir_break_statement: Entered for node type %d (line %ld approx)\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    Function * currentFunc = module->getCurrentFunction();
    assert(currentFunc != nullptr && "ir_break_statement called outside of a function context");

    if (loop_label_stack.empty()) {
        // Semantic error: break statement not within a loop.
        // You might want to use your minic_log for this kind of error.
        fprintf(stderr, "Error line %ld: break statement not within a loop.\n", node->line_no);
        // Alternatively, set an error flag in the module or throw an exception.
        return false; // Indicate failure
    }

    LabelInstruction * break_target = loop_label_stack.top().second;
    assert(break_target != nullptr && "Break target label is null in loop_label_stack");

    printf("[DEBUG] ir_break_statement: Generating goto to break_target label %s\n", break_target->getIRName().c_str());
    fflush(stdout);

    node->blockInsts.addInst(new GotoInstruction(currentFunc, break_target));
    node->val = nullptr; // break statement doesn't have a value

    printf("[DEBUG] ir_break_statement: Exiting successfully.\n");
    fflush(stdout);
    return true;
}

/// @brief 新增：continue语句翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_continue_statement(ast_node * node)
{
    assert(node != nullptr && node->node_type == ast_operator_type::AST_OP_CONTINUE);
    printf("[DEBUG] ir_continue_statement: Entered for node type %d (line %ld approx)\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    Function * currentFunc = module->getCurrentFunction();
    assert(currentFunc != nullptr && "ir_continue_statement called outside of a function context");

    if (loop_label_stack.empty()) {
        // Semantic error: continue statement not within a loop.
        fprintf(stderr, "Error line %ld: continue statement not within a loop.\n", node->line_no);
        return false; // Indicate failure
    }

    LabelInstruction * continue_target = loop_label_stack.top().first;
    assert(continue_target != nullptr && "Continue target label is null in loop_label_stack");

    printf("[DEBUG] ir_continue_statement: Generating goto to continue_target label %s\n",
           continue_target->getIRName().c_str());
    fflush(stdout);

    node->blockInsts.addInst(new GotoInstruction(currentFunc, continue_target));
    node->val = nullptr; // continue statement doesn't have a value

    printf("[DEBUG] ir_continue_statement: Exiting successfully.\n");
    fflush(stdout);
    return true;
}

// Implementation for ir_logical_not
bool IRGenerator::ir_logical_not(ast_node * node)
{
    assert(node->node_type == ast_operator_type::AST_OP_LOGICAL_NOT && "Node is not logical_not"); // Restored AST check
    ast_node * operand_node = node->sons[0];                                                       // Restored AST check

    ast_node * visited_operand_node = ir_visit_ast_node(operand_node); // Restored AST check
    if (!visited_operand_node) {
        return false;
    }
    node->blockInsts.addInst(visited_operand_node->blockInsts); // Assumed addInst(InterCode) exists
    Value * original_operand_val = visited_operand_node->val;

    if (!original_operand_val) {
        printf("[ERROR] ir_logical_not: Operand for logical NOT did not produce a value. Line: %ld\n", node->line_no);
        fflush(stdout);
        return false;
    }

    Type * operand_type = original_operand_val->getType();
    Function * currentFunc = module->getCurrentFunction();
    Value * operand_val_i1 = nullptr;

    printf("[DEBUG] ir_logical_not: Processing operand %s of type %s. Line: %ld\n",
           original_operand_val->getIRName().c_str(),
           operand_type->toString().c_str(),
           node->line_no);
    fflush(stdout);

    if (operand_type->isInt32Type()) {
        printf("[DEBUG] ir_logical_not: Operand is i32. Converting to i1 (val != 0). Line: %ld\n", node->line_no);
        fflush(stdout);
        ConstInt * const_zero_i32 = module->newConstInt(0);
        if (!const_zero_i32) {
            printf("[ERROR] ir_logical_not: Failed to create i32 zero for comparison. Line: %ld\n", node->line_no);
            fflush(stdout);
            return false;
        }
        Instruction * cmp_ne_zero = new BinaryInstruction(currentFunc,
                                                          IRInstOperator::IRINST_OP_CMP_NE_I,
                                                          original_operand_val,
                                                          const_zero_i32,
                                                          IntegerType::getTypeBool());
        node->blockInsts.addInst(cmp_ne_zero);
        operand_val_i1 = cmp_ne_zero;

    } else if (operand_type->isInt1Byte()) {
        printf("[DEBUG] ir_logical_not: Operand is already i1. Line: %ld\n", node->line_no);
        fflush(stdout);
        operand_val_i1 = original_operand_val;
    } else {
        printf("[ERROR] ir_logical_not: Operand for logical NOT is not i32 or i1 (type: %s). Line: %ld\n",
               operand_type->toString().c_str(),
               node->line_no);
        fflush(stdout);
        return false;
    }

    ConstInt * const_false_i1 = new ConstInt(false);
    if (!const_false_i1) {
        printf("[ERROR] ir_logical_not: Failed to create i1 false constant. Line: %ld\n", node->line_no);
        fflush(stdout);
        return false;
    }

    printf("[DEBUG] ir_logical_not: Comparing %s (i1) with const false (i1). Line: %ld\n",
           operand_val_i1->getIRName().c_str(),
           node->line_no);
    fflush(stdout);

    Instruction * final_cmp_eq_false = new BinaryInstruction(currentFunc,
                                                             IRInstOperator::IRINST_OP_CMP_EQ_I,
                                                             operand_val_i1,
                                                             const_false_i1,
                                                             IntegerType::getTypeBool());
    node->blockInsts.addInst(final_cmp_eq_false);
    node->val = final_cmp_eq_false;

    printf("[DEBUG] ir_logical_not: Completed. Result value is %s. Line: %ld\n",
           node->val->getIRName().c_str(),
           node->line_no);
    fflush(stdout);
    return true;
}

// Implementation for ir_logical_and
bool IRGenerator::ir_logical_and(ast_node * node)
{
    assert(node != nullptr && node->node_type == ast_operator_type::AST_OP_LOGICAL_AND);
    printf("[DEBUG] ir_logical_and: Visited directly (node type %d, line %ld approx). This operator primarily "
           "generates branches via generate_branch_for_condition.\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    assert(node->sons.size() == 2 && "Logical AND should have two operands");
    ast_node * lhs_node = node->sons[0];
    ast_node * rhs_node = node->sons[1];

    // Visit operands to collect their instructions. Short-circuiting logic for *value production*
    // would be more complex here and is not implemented as these are primarily for branching.
    ast_node * lhs_visited = ir_visit_ast_node(lhs_node);
    if (!lhs_visited) {
        printf("[ERROR] ir_logical_and: Visiting LHS node failed.\n");
        fflush(stdout);
        return false;
    }
    node->blockInsts.addInst(lhs_visited->blockInsts);

    ast_node * rhs_visited = ir_visit_ast_node(rhs_node);
    if (!rhs_visited) {
        printf("[ERROR] ir_logical_and: Visiting RHS node failed.\n");
        fflush(stdout);
        return false;
    }
    node->blockInsts.addInst(rhs_visited->blockInsts);

    node->val = nullptr;
    printf("[DEBUG] ir_logical_and: Exiting. node->val set to nullptr.\n");
    fflush(stdout);
    return true;
}

// Implementation for ir_logical_or
bool IRGenerator::ir_logical_or(ast_node * node)
{
    assert(node != nullptr && node->node_type == ast_operator_type::AST_OP_LOGICAL_OR);
    printf("[DEBUG] ir_logical_or: Visited directly (node type %d, line %ld approx). This operator primarily generates "
           "branches via generate_branch_for_condition.\n",
           (int) node->node_type,
           node->line_no);
    fflush(stdout);

    assert(node->sons.size() == 2 && "Logical OR should have two operands");
    ast_node * lhs_node = node->sons[0];
    ast_node * rhs_node = node->sons[1];

    ast_node * lhs_visited = ir_visit_ast_node(lhs_node);
    if (!lhs_visited) {
        printf("[ERROR] ir_logical_or: Visiting LHS node failed.\n");
        fflush(stdout);
        return false;
    }
    node->blockInsts.addInst(lhs_visited->blockInsts);

    ast_node * rhs_visited = ir_visit_ast_node(rhs_node);
    if (!rhs_visited) {
        printf("[ERROR] ir_logical_or: Visiting RHS node failed.\n");
        fflush(stdout);
        return false;
    }
    node->blockInsts.addInst(rhs_visited->blockInsts);

    node->val = nullptr;
    printf("[DEBUG] ir_logical_or: Exiting. node->val set to nullptr.\n");
    fflush(stdout);
    return true;
}

/// @brief 变量初始化节点翻译成线性中间IR
/// @param node AST节点，包含两个孩子：变量ID节点和初值表达式节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_variable_initialize(ast_node * node)
{
    // AST_OP_VAR_INIT节点有两个孩子：
    // 第一个孩子：变量ID节点
    // 第二个孩子：初值表达式节点

    if (node->sons.size() != 2) {
        printf("[ERROR] ir_variable_initialize: Expected 2 children but got %zu\n", node->sons.size());
        return false;
    }

    ast_node * id_node = node->sons[0];
    ast_node * init_expr_node = node->sons[1];

    // 首先处理初值表达式
    ast_node * visited_expr = ir_visit_ast_node(init_expr_node);
    if (!visited_expr) {
        printf("[ERROR] ir_variable_initialize: Failed to visit initialization expression\n");
        return false;
    }

    // 检查表达式是否产生了值
    if (!visited_expr->val) {
        printf("[ERROR] ir_variable_initialize: Initialization expression did not produce a value\n");
        return false;
    }

    // 添加表达式的指令
    node->blockInsts.addInst(visited_expr->blockInsts);

    // 创建变量（类型从表达式推断）
    Type * var_type = visited_expr->val->getType();
    if (!var_type) {
        printf("[ERROR] ir_variable_initialize: Failed to get type from initialization expression\n");
        return false;
    }

    Value * varValue = module->newVarValue(var_type, id_node->name);
    if (!varValue) {
        printf("[ERROR] ir_variable_initialize: Failed to create variable %s\n", id_node->name.c_str());
        return false;
    }

    // 创建赋值指令，将初值赋给变量
    Function * currentFunc = module->getCurrentFunction();
    MoveInstruction * assignInst = new MoveInstruction(currentFunc, varValue, visited_expr->val);
    node->blockInsts.addInst(assignInst);

    // 变量初始化节点的值就是变量本身
    node->val = varValue;

    printf("[DEBUG] ir_variable_initialize: Initialized variable %s with type %s\n",
           id_node->name.c_str(),
           var_type->toString().c_str());

    return true;
}

/// @brief 数组声明节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_array_declare(ast_node * node)
{
    // AST_OP_ARRAY_DECL节点有两个孩子：
    // 第一个孩子：变量ID节点
    // 第二个孩子：数组维度节点

    if (node->sons.size() != 2) {
        printf("[ERROR] ir_array_declare: Expected 2 children but got %zu\n", node->sons.size());
        return false;
    }

    ast_node * id_node = node->sons[0];
    ast_node * dim_node = node->sons[1];

    // 从维度节点中提取维度值
    std::vector<int32_t> dimensions;
    if (!extractDimensions(dim_node, dimensions)) {
        printf("[ERROR] ir_array_declare: Failed to extract array dimensions\n");
        return false;
    }

    // 创建数组类型（元素类型默认为int32）
    Type * elementType = IntegerType::getTypeInt();
    ArrayType * arrayType = ArrayType::getType(elementType, dimensions);

    // 判断是否为函数形参
    Function * currentFunc = module->getCurrentFunction();
    if (currentFunc && isInFormalParams(node)) {
        // 函数形参数组，第一维设为0，类型为指针
        std::vector<int32_t> paramDims = dimensions;
        paramDims[0] = 0; // 第一维设为0表示指针
        ArrayType * paramArrayType = ArrayType::getType(elementType, paramDims);

        // 实际分配的是指针类型
        PointerType * ptrType = PointerType::getType(elementType);
        Value * varValue = module->newVarValue(ptrType, id_node->name);

        if (!varValue) {
            printf("[ERROR] ir_array_declare: Failed to create array parameter %s\n", id_node->name.c_str());
            return false;
        }

        node->val = varValue;
        node->type = paramArrayType; // AST节点类型为数组类型

    } else {
        // 局部或全局数组变量
        Value * varValue = module->newVarValue(arrayType, id_node->name);
        if (!varValue) {
            printf("[ERROR] ir_array_declare: Failed to create array variable %s\n", id_node->name.c_str());
            return false;
        }

        node->val = varValue;
        node->type = arrayType;
    }

    printf("[DEBUG] ir_array_declare: Declared array %s with type %s\n",
           id_node->name.c_str(),
           node->type->toString().c_str());

    // 验证变量是否成功添加到符号表
    Value * testFind = module->findVarValue(id_node->name);
    if (!testFind) {
        printf("[ERROR] ir_array_declare: Array %s not found in symbol table after creation\n", id_node->name.c_str());
        return false;
    }

    return true;
}

/// @brief 数组元素访问节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_array_ref(ast_node * node)
{
    // AST_OP_ARRAY_REF节点有两个孩子：
    // 第一个孩子：数组变量ID节点
    // 第二个孩子：下标表达式节点

    if (node->sons.size() != 2) {
        printf("[ERROR] ir_array_ref: Expected 2 children but got %zu\n", node->sons.size());
        return false;
    }

    ast_node * id_node = node->sons[0];
    ast_node * index_node = node->sons[1];

    // 查找数组变量
    Value * arrayVar = module->findVarValue(id_node->name);
    if (!arrayVar) {
        printf("[ERROR] ir_array_ref: Array variable %s not found\n", id_node->name.c_str());
        return false;
    }

    // 处理下标表达式 - 支持多维数组
    printf("[DEBUG] ir_array_ref: Processing index node type: %d\n", (int) index_node->node_type);

    std::vector<Value *> indexValues;

    if (index_node->node_type == ast_operator_type::AST_OP_ARRAY_DIM) {
        // 处理ArrayDimensions节点，包含多个维度的下标表达式
        if (index_node->sons.empty()) {
            printf("[ERROR] ir_array_ref: ArrayDimensions node has no children\n");
            return false;
        }

        // 处理所有维度的下标表达式
        for (auto dim_expr: index_node->sons) {
            ast_node * visited_index = ir_visit_ast_node(dim_expr);
            if (!visited_index) {
                printf("[ERROR] ir_array_ref: Failed to visit array index expression\n");
                return false;
            }

            // 添加下标计算的指令
            node->blockInsts.addInst(visited_index->blockInsts);
            indexValues.push_back(visited_index->val);
        }
    } else {
        // 直接的表达式节点（一维数组访问）
        ast_node * visited_index = ir_visit_ast_node(index_node);
        if (!visited_index) {
            printf("[ERROR] ir_array_ref: Failed to visit array index\n");
            return false;
        }

        // 添加下标计算的指令
        node->blockInsts.addInst(visited_index->blockInsts);
        indexValues.push_back(visited_index->val);
    }

    printf("[DEBUG] ir_array_ref: Index processed successfully, %zu dimensions\n", indexValues.size());

    // 计算数组元素地址
    Function * currentFunc = module->getCurrentFunction();
    Type * arrayType = arrayVar->getType();

    if (arrayType->isArrayType()) {
        // 局部数组变量
        ArrayType * arrType = dynamic_cast<ArrayType *>(arrayType);
        Type * elementType = arrType->getElementType();
        std::vector<int32_t> dimensions = arrType->getDimensions();

        // 计算多维数组的偏移：offset = (...((i0 * d1 + i1) * d2 + i2) * ... + in) * element_size
        Value * offset = nullptr;

        if (indexValues.size() == 1) {
            // 一维数组访问
            offset = indexValues[0];
        } else {
            // 多维数组访问
            offset = indexValues[0]; // 从第一个维度开始

            for (size_t i = 1; i < indexValues.size(); i++) {
                // offset = offset * dimensions[i] + indexValues[i]
                Value * dimSize = module->newConstInt(dimensions[i]);

                BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_MUL_I,
                                                                    offset,
                                                                    dimSize,
                                                                    IntegerType::getTypeInt());
                node->blockInsts.addInst(mulInst);

                BinaryInstruction * addInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_ADD_I,
                                                                    mulInst,
                                                                    indexValues[i],
                                                                    IntegerType::getTypeInt());
                node->blockInsts.addInst(addInst);
                offset = addInst;
            }
        }

        // 最后乘以元素大小
        Value * elementSize = module->newConstInt(elementType->getSize());
        BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                            IRInstOperator::IRINST_OP_MUL_I,
                                                            offset,
                                                            elementSize,
                                                            IntegerType::getTypeInt());
        node->blockInsts.addInst(mulInst);

        // 创建指针类型的临时变量来存储元素地址
        PointerType * ptrType = PointerType::getType(elementType);

        // 生成地址计算指令：elemPtr = arrayVar + offset
        BinaryInstruction * addrInst =
            new BinaryInstruction(currentFunc, IRInstOperator::IRINST_OP_ADD_I, arrayVar, mulInst, ptrType);
        node->blockInsts.addInst(addrInst);
        node->val = addrInst;

    } else if (arrayType->isPointerType()) {
        // 函数形参数组（指针类型）
        const PointerType * ptrType = dynamic_cast<const PointerType *>(arrayType);
        const Type * elementType = ptrType->getPointeeType();

        // 尝试从变量名称查找对应的形参，获取原始数组类型信息
        std::vector<int32_t> dimensions;
        bool foundDimensions = false;
        
        if (currentFunc) {
            // 在当前函数的形参中查找
            for (auto param : currentFunc->getParams()) {
                if (param->getName() == id_node->name && param->getIsArrayParam() && param->getOriginalArrayType()) {
                    ArrayType * origArrayType = dynamic_cast<ArrayType *>(param->getOriginalArrayType());
                    if (origArrayType) {
                        dimensions = origArrayType->getDimensions();
                        foundDimensions = true;
                        break;
                    }
                }
            }
        }

        if (foundDimensions && indexValues.size() > 1) {
            // 支持多维数组形参访问 - 使用与局部数组相同的偏移计算
            Value * offset = indexValues[0]; // 从第一个维度开始

            for (size_t i = 1; i < indexValues.size(); i++) {
                // offset = offset * dimensions[i] + indexValues[i]
                Value * dimSize = module->newConstInt(dimensions[i]);

                BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_MUL_I,
                                                                    offset,
                                                                    dimSize,
                                                                    IntegerType::getTypeInt());
                node->blockInsts.addInst(mulInst);

                BinaryInstruction * addInst = new BinaryInstruction(currentFunc,
                                                                    IRInstOperator::IRINST_OP_ADD_I,
                                                                    mulInst,
                                                                    indexValues[i],
                                                                    IntegerType::getTypeInt());
                node->blockInsts.addInst(addInst);
                offset = addInst;
            }

            // 最后乘以元素大小
            Value * elementSize = module->newConstInt(elementType->getSize());
            BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                                IRInstOperator::IRINST_OP_MUL_I,
                                                                offset,
                                                                elementSize,
                                                                IntegerType::getTypeInt());
            node->blockInsts.addInst(mulInst);

            // 生成地址计算指令：elemPtr = arrayVar + byteOffset
            PointerType * nonConstPtrType = PointerType::getType(const_cast<Type *>(elementType));
            BinaryInstruction * addrInst =
                new BinaryInstruction(currentFunc, IRInstOperator::IRINST_OP_ADD_I, arrayVar, mulInst, nonConstPtrType);
            node->blockInsts.addInst(addrInst);
            node->val = addrInst;
        } else {
            // 单维访问或未找到维度信息时的原有逻辑
            if (indexValues.size() != 1) {
                printf("[ERROR] ir_array_ref: Multi-dimensional access requires dimension information for parameter %s\n", id_node->name.c_str());
                return false;
            }

            // 计算字节偏移：offset = index * elementSize
            Value * elementSize = module->newConstInt(elementType->getSize());
            BinaryInstruction * mulInst = new BinaryInstruction(currentFunc,
                                                                IRInstOperator::IRINST_OP_MUL_I,
                                                                indexValues[0],
                                                                elementSize,
                                                                IntegerType::getTypeInt());
            node->blockInsts.addInst(mulInst);

            // 生成地址计算指令：elemPtr = arrayVar + byteOffset
            PointerType * nonConstPtrType = PointerType::getType(const_cast<Type *>(elementType));
            BinaryInstruction * addrInst =
                new BinaryInstruction(currentFunc, IRInstOperator::IRINST_OP_ADD_I, arrayVar, mulInst, nonConstPtrType);
            node->blockInsts.addInst(addrInst);
            node->val = addrInst;
        }

    } else {
        printf("[ERROR] ir_array_ref: Variable %s is not an array type\n", id_node->name.c_str());
        return false;
    }

    // 检查这个数组访问是否用作左值（赋值的左边）
    bool isLValue = false;
    ast_node * parent = node->parent;
    if (parent && parent->node_type == ast_operator_type::AST_OP_ASSIGN) {
        // 检查这个节点是否是赋值的左操作数
        if (parent->sons.size() >= 1 && parent->sons[0] == node) {
            isLValue = true;
        }
    }

    // 如果这是右值（用于读取），我们需要生成一个加载指令来获取实际的值
    if (!isLValue) {
        // 为加载的值创建一个临时变量
        Type * elementType = nullptr;
        if (arrayType->isArrayType()) {
            ArrayType * arrType = dynamic_cast<ArrayType *>(arrayType);
            elementType = arrType->getElementType();
        } else if (arrayType->isPointerType()) {
            const PointerType * ptrType = dynamic_cast<const PointerType *>(arrayType);
            elementType = const_cast<Type *>(ptrType->getPointeeType());
        }

        if (elementType) {
            // 创建临时变量来保存加载的值
            MemVariable * loadedValue = currentFunc->newMemVariable(elementType);
            // 注意：不在这里设置IR名，让renameIR统一处理

            // 创建一个加载指令：value = *address
            MoveInstruction * loadInst = new MoveInstruction(currentFunc, loadedValue, node->val);

            node->blockInsts.addInst(loadInst);
            node->val = loadedValue; // 更新node的值为加载后的值

            printf("[DEBUG] ir_array_ref: Generated load instruction for array read %s\n", id_node->name.c_str());
        } else {
            printf("[ERROR] ir_array_ref: Could not determine element type for load\n");
            return false;
        }
    } else {
        printf("[DEBUG] ir_array_ref: Generated address for array write %s\n", id_node->name.c_str());
    }

    printf("[DEBUG] ir_array_ref: Generated array reference for %s\n", id_node->name.c_str());
    return true;
}

/// @brief 数组维度节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_array_dim(ast_node * node)
{
    // 数组维度节点包含多个表达式作为孩子
    // 每个孩子都是一个维度的大小表达式

    for (auto son: node->sons) {
        ast_node * visited = ir_visit_ast_node(son);
        if (!visited) {
            printf("[ERROR] ir_array_dim: Failed to visit dimension expression\n");
            return false;
        }

        // 添加维度表达式的指令
        node->blockInsts.addInst(visited->blockInsts);
    }

    // 维度节点本身不需要产生值
    node->val = nullptr;
    return true;
}

/// @brief 从维度节点中提取常量维度值
/// @param dim_node 维度节点
/// @param dimensions 输出的维度数组
/// @return 是否成功
bool IRGenerator::extractDimensions(ast_node * dim_node, std::vector<int32_t> & dimensions)
{
    for (auto son: dim_node->sons) {
        if (son->node_type == ast_operator_type::AST_OP_LEAF_LITERAL_UINT) {
            dimensions.push_back(static_cast<int32_t>(son->integer_val));
        } else if (son->node_type == ast_operator_type::AST_OP_EMPTY_DIM) {
            // 空维度（用于函数形参），添加0表示指针
            dimensions.push_back(0);
        } else {
            // 暂不支持非常量维度
            printf("[ERROR] extractDimensions: Non-constant array dimensions not supported\n");
            return false;
        }
    }
    return true;
}

/// @brief 空数组维度节点翻译成线性中间IR
/// @param node AST节点
/// @return 翻译是否成功，true：成功，false：失败
bool IRGenerator::ir_empty_dim(ast_node * node)
{
    // 空维度节点不需要生成任何指令，只是一个标记
    node->val = nullptr;
    return true;
}

/// @brief 检查节点是否在函数形参中
/// @param node AST节点
/// @return 是否在形参中
bool IRGenerator::isInFormalParams(ast_node * node)
{
    // 简单的实现：检查父节点类型
    ast_node * parent = node->parent;
    while (parent) {
        if (parent->node_type == ast_operator_type::AST_OP_FUNC_FORMAL_PARAM ||
            parent->node_type == ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS) {
            return true;
        }
        parent = parent->parent;
    }
    return false;
}
