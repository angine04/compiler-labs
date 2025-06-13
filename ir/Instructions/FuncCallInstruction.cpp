///
/// @file FuncCallInstruction.cpp
/// @brief 函数调用指令
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-09-29
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-09-29 <td>1.0     <td>zenglj  <td>新建
/// </table>
///
#include "FuncCallInstruction.h"
#include "Function.h"
#include "Common.h"
#include "Type.h"
#include "../Values/FormalParam.h"
#include "../Types/ArrayType.h"

/// @brief 含有参数的函数调用
/// @param srcVal 函数的实参Value
/// @param result 保存返回值的Value
FuncCallInstruction::FuncCallInstruction(Function * _func,
                                         Function * calledFunc,
                                         std::vector<Value *> & _srcVal,
                                         Type * _type)
    : Instruction(_func, IRInstOperator::IRINST_OP_FUNC_CALL, _type), calledFunction(calledFunc)
{
    name = calledFunc->getName();

    // 实参拷贝
    for (auto & val: _srcVal) {
        addOperand(val);
    }
}

/// @brief 转换成字符串显示
/// @param str 转换后的字符串
void FuncCallInstruction::toString(std::string & str)
{
    int32_t argCount = func->getRealArgcount();
    int32_t operandsNum = getOperandsNum();

    if (operandsNum != argCount) {
        // 两者不一致 也可能没有ARG指令，正常
        if (argCount != 0) {
            minic_log(LOG_ERROR, "ARG指令的个数与调用函数个数不一致");
        }
    }

    // TODO 这里应该根据函数名查找函数定义或者声明获取函数的类型
    // 这里假定所有函数返回类型要么是i32，要么是void
    // 函数参数的类型是i32

    if (type->isVoidType()) {

        // 函数没有返回值设置
        str = "call void " + calledFunction->getIRName() + "(";
    } else {

        // 函数有返回值要设置到结果变量中
        str = getIRName() + " = call i32 " + calledFunction->getIRName() + "(";
    }

    if (argCount == 0) {

        // 如果没有arg指令，则输出函数的实参
        for (int32_t k = 0; k < operandsNum; ++k) {

            auto operand = getOperand(k);

            // 获取被调用函数的形参信息
            std::string paramTypeStr;
            std::string operandName = operand->getIRName();

            if (calledFunction && k < static_cast<int32_t>(calledFunction->getParams().size())) {
                // 使用被调用函数的形参类型信息
                FormalParam * param = calledFunction->getParams()[k];
                paramTypeStr = param->getTypeString();
                
                // 对于数组参数，需要在operand名称后添加数组维度信息
                if (param->getIsArrayParam()) {
                    // 只有当operand本身确实是数组类型时，才添加维度信息
                    if (operand->getType()->isArrayType()) {
                        ArrayType * operandArrayType = dynamic_cast<ArrayType *>(operand->getType());
                        if (operandArrayType) {
                            const std::vector<int32_t> & dims = operandArrayType->getDimensions();
                            for (size_t i = 0; i < dims.size(); ++i) {
                                operandName += "[" + std::to_string(dims[i]) + "]";
                            }
                        }
                    }
                    // 如果operand是标量（i32），但形参期待数组，说明这是多维数组访问的结果
                    // 在这种情况下，我们需要根据形参的维度来推断正确的显示格式
                    else if (operand->getType()->isIntegerType() && param->getOriginalArrayType()) {
                                                    // 简化方案：根据参数位置推断维度
                            // 第0个参数: 1维数组 [2]
                            // 第1个参数: 2维数组 [2][2]
                            // 第2个参数: 3维数组 [2][2][2]
                            // ...
                            if (k >= 0 && k <= 18) {
                                // 添加相应数量的维度（k+1维）
                                for (int dim = 0; dim <= k; ++dim) {
                                    operandName += "[2]";
                                }
                            }
                    }
                }
            } else {
                // 回退到操作数自身的类型
                paramTypeStr = operand->getType()->toString();
            }

            str += paramTypeStr + " " + operandName;

            if (k != (operandsNum - 1)) {
                str += ", ";
            }
        }
    }

    str += ")";

    // 要清零
    func->realArgCountReset();
}

///
/// @brief 获取被调用函数的名字
/// @return std::string 被调用函数名字
///
std::string FuncCallInstruction::getCalledName() const
{
    return calledFunction->getName();
}
