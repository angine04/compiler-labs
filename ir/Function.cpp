﻿///
/// @file Function.cpp
/// @brief 函数实现
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-11-21
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// </table>
///

#include <cstdlib>
#include <string>

#include "IRConstant.h"
#include "Function.h"
#include "ArrayType.h"
#include "Types/PointerType.h"

/// @brief 指定函数名字、函数类型的构造函数
/// @param _name 函数名称
/// @param _type 函数类型
/// @param _builtin 是否是内置函数
Function::Function(std::string _name, FunctionType * _type, bool _builtin)
    : GlobalValue(_type, _name), builtIn(_builtin)
{
    returnType = _type->getReturnType();

    // 设置对齐大小
    setAlignment(1);
}

///
/// @brief 析构函数
/// @brief 释放函数占用的内存和IR指令代码
/// @brief 注意：IR指令代码并未释放，需要手动释放
Function::~Function()
{
    Delete();
}

/// @brief 获取函数返回类型
/// @return 返回类型
Type * Function::getReturnType()
{
    return returnType;
}

/// @brief 获取函数的形参列表
/// @return 形参列表
std::vector<FormalParam *> & Function::getParams()
{
    return params;
}

/// @brief 获取函数内的IR指令代码
/// @return IR指令代码
InterCode & Function::getInterCode()
{
    return code;
}

/// @brief 判断该函数是否是内置函数
/// @return true: 内置函数，false：用户自定义
bool Function::isBuiltin()
{
    return builtIn;
}

/// @brief 函数指令信息输出
/// @param str 函数指令
void Function::toString(std::string & str)
{
    if (builtIn) {
        // 内置函数则什么都不输出
        return;
    }

    // 输出函数头
    str = "define " + getReturnType()->toString() + " " + getIRName() + "(";

    bool firstParam = false;
    for (auto & param: params) {

        if (!firstParam) {
            firstParam = true;
        } else {
            str += ", ";
        }

        std::string param_str = param->getFullString();

        str += param_str;
    }

    str += ")\n";

    str += "{\n";

    // 输出局部变量的名字与IR名字
    for (auto & var: this->varsVector) {
        // 检查是否是数组形参局部变量（通过变量名和类型匹配形参）
        bool isArrayParam = false;
        for (auto & param : this->params) {
            if (param->getName() == var->getName() && param->getIsArrayParam() && 
                var->getType()->isPointerType()) {
                isArrayParam = true;
                break;
            }
        }
        
        if (isArrayParam) {
            // 数组形参格式：需要获取对应形参的完整维度信息
            PointerType * ptrType = dynamic_cast<PointerType *>(var->getType());
            if (ptrType) {
                str += "\tdeclare " + ptrType->getPointeeType()->toString() + " " + var->getIRName();
                
                // 查找对应的形参，获取其完整维度信息
                for (auto & param : this->params) {
                    if (param->getName() == var->getName() && param->getIsArrayParam()) {
                        ArrayType * originalArrayType = dynamic_cast<ArrayType *>(param->getOriginalArrayType());
                        if (originalArrayType) {
                            const std::vector<int32_t> & dims = originalArrayType->getDimensions();
                            // 第一维总是[0]，后续维度使用实际值
                            str += "[0]";
                            for (size_t i = 1; i < dims.size(); ++i) {
                                str += "[" + std::to_string(dims[i]) + "]";
                            }
                        } else {
                            // fallback: 只是1维数组
                            str += "[0]";
                        }
                        break;
                    }
                }
            } else {
                str += "\tdeclare " + var->getType()->toString() + " " + var->getIRName();
            }
        } else if (var->getType()->isArrayType()) {
            // 对于普通数组类型，需要特殊格式：declare i32 %var[10]
            ArrayType * arrayType = dynamic_cast<ArrayType *>(var->getType());
            Type * elementType = arrayType->getElementType();

            std::string dimensionsStr;
            for (int32_t dim: arrayType->getDimensions()) {
                dimensionsStr += "[" + std::to_string(dim) + "]";
            }

            str += "\tdeclare " + elementType->toString() + " " + var->getIRName() + dimensionsStr;
        } else {
            // 普通局部变量格式
            str += "\tdeclare " + var->getType()->toString() + " " + var->getIRName();
        }

        std::string realName = var->getName();
        if (!realName.empty()) {
            str += " ; variable: " + realName;
        }

        str += "\n";
    }

    // 输出内存变量的declare形式
    for (auto & memVar: this->memVector) {
        str += "\tdeclare " + memVar->getType()->toString() + " " + memVar->getIRName() + "\n";
    }

    // 输出临时变量的declare形式
    // 遍历所有的线性IR指令，文本输出
    for (auto & inst: code.getInsts()) {

        if (inst->hasResultValue()) {

            // 局部变量和临时变量需要输出declare语句
            str += "\tdeclare " + inst->getType()->toString() + " " + inst->getIRName() + "\n";
        }
    }

    // 遍历所有的线性IR指令，文本输出
    for (auto & inst: code.getInsts()) {

        std::string instStr;
        inst->toString(instStr);

        if (!instStr.empty()) {

            // Label指令不加Tab键
            if (inst->getOp() == IRInstOperator::IRINST_OP_LABEL) {
                str += instStr + "\n";
            } else {
                str += "\t" + instStr + "\n";
            }
        }
    }

    // 输出函数尾部
    str += "}\n";
}

/// @brief 设置函数出口指令
/// @param inst 出口Label指令
void Function::setExitLabel(Instruction * inst)
{
    exitLabel = inst;
}

/// @brief 获取函数出口指令
/// @return 出口Label指令
Instruction * Function::getExitLabel()
{
    return exitLabel;
}

/// @brief 设置函数返回值变量
/// @param val 返回值变量，要求必须是局部变量，不能是临时变量
void Function::setReturnValue(LocalVariable * val)
{
    returnValue = val;
}

/// @brief 获取函数返回值变量
/// @return 返回值变量
LocalVariable * Function::getReturnValue()
{
    return returnValue;
}

/// @brief 获取最大栈帧深度
/// @return 栈帧深度
int Function::getMaxDep()
{
    return maxDepth;
}

/// @brief 设置最大栈帧深度
/// @param dep 栈帧深度
void Function::setMaxDep(int dep)
{
    maxDepth = dep;

    // 设置函数栈帧被重定位标记，用于生成不同的栈帧保护代码
    relocated = true;
}

/// @brief 获取本函数需要保护的寄存器
/// @return 要保护的寄存器
std::vector<int32_t> & Function::getProtectedReg()
{
    return protectedRegs;
}

/// @brief 获取本函数需要保护的寄存器字符串
/// @return 要保护的寄存器
std::string & Function::getProtectedRegStr()
{
    return protectedRegStr;
}

/// @brief 获取函数调用参数个数的最大值
/// @return 函数调用参数个数的最大值
int Function::getMaxFuncCallArgCnt()
{
    return maxFuncCallArgCnt;
}

/// @brief 设置函数调用参数个数的最大值
/// @param count 函数调用参数个数的最大值
void Function::setMaxFuncCallArgCnt(int count)
{
    maxFuncCallArgCnt = count;
}

/// @brief 函数内是否存在函数调用
/// @return 是否存在函调用
bool Function::getExistFuncCall()
{
    return funcCallExist;
}

/// @brief 设置函数是否存在函数调用
/// @param exist true: 存在 false: 不存在
void Function::setExistFuncCall(bool exist)
{
    funcCallExist = exist;
}

/// @brief 新建变量型Value。先检查是否存在，不存在则创建，否则失败
/// @param name 变量ID
/// @param type 变量类型
/// @param scope_level 局部变量的作用域层级
LocalVariable * Function::newLocalVarValue(Type * type, std::string name, int32_t scope_level)
{
    // 创建变量并加入符号表
    LocalVariable * varValue = new LocalVariable(type, name, scope_level);

    // varsVector表中可能存在变量重名的信息
    varsVector.push_back(varValue);

    return varValue;
}

/// @brief 新建一个内存型的Value，并加入到符号表，用于后续释放空间
/// \param type 变量类型
/// \return 临时变量Value
MemVariable * Function::newMemVariable(Type * type)
{
    // 肯定唯一存在，直接插入即可
    MemVariable * memValue = new MemVariable(type);

    memVector.push_back(memValue);

    return memValue;
}

/// @brief 清理函数内申请的资源
void Function::Delete()
{
    // 清理IR指令
    code.Delete();

    // 清理Value
    for (auto & var: varsVector) {
        delete var;
    }

    varsVector.clear();
}

///
/// @brief 函数内的Value重命名
///
void Function::renameIR()
{
    // 内置函数忽略
    if (isBuiltin()) {
        return;
    }

    int32_t nameIndex = 0;

    // 形式参数重命名
    for (auto & param: this->params) {
        param->setIRName(IR_TEMP_VARNAME_PREFIX + std::to_string(nameIndex++));
    }

    // 局部变量重命名
    for (auto & var: this->varsVector) {

        var->setIRName(IR_LOCAL_VARNAME_PREFIX + std::to_string(nameIndex++));
    }

    // 内存变量重命名（MemVariable）
    for (auto & memVar: this->memVector) {
        memVar->setIRName(IR_TEMP_VARNAME_PREFIX + std::to_string(nameIndex++));
    }

    // 遍历所有的指令进行命名
    for (auto inst: this->getInterCode().getInsts()) {
        if (inst->getOp() == IRInstOperator::IRINST_OP_LABEL) {
            inst->setIRName(IR_LABEL_PREFIX + std::to_string(nameIndex++));
        } else if (inst->hasResultValue()) {
            inst->setIRName(IR_TEMP_VARNAME_PREFIX + std::to_string(nameIndex++));
        }
    }
}

///
/// @brief 获取统计的ARG指令的个数
/// @return int32_t 个数
///
int32_t Function::getRealArgcount()
{
    return this->realArgCount;
}

///
/// @brief 用于统计ARG指令个数的自增函数，个数加1
///
void Function::realArgCountInc()
{
    this->realArgCount++;
}

///
/// @brief 用于统计ARG指令个数的清零
///
void Function::realArgCountReset()
{
    this->realArgCount = 0;
}

// Adding new method implementations here
std::string Function::newTempValueName()
{
    return "%" + std::to_string(tempVarCounter++);
}

std::string Function::newLabelName()
{
    return ".L" + std::to_string(labelCounter++);
}

int Function::getNextInstructionID()
{
    // 注意：此函数现在与 renameIR 中的 instructionCounter 重置逻辑有潜在冲突。
    // renameIR 会重置 instructionCounter。如果 getNextInstructionID() 在此之后独立使用，
    // 可能会产生重复的ID。
    // 如果 renameIR 是唯一设置指令名的地方，那么 getNextInstructionID 应该在 renameIR 内部调用，
    // 或者 renameIR 中的 instructionCounter 管理需要更精细。
    // 暂时保持原设计，如果 renameIR 逻辑确实是主要的ID生成方式，
    // 那么这个独立的 getNextInstructionID 可能需要重新审视其用途或被 renameIR 内部消化。
    return instructionCounter++;
}
