///
/// @file GlobalVariable.h
/// @brief 全局变量描述类
///
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
#pragma once

#include "GlobalValue.h"
#include "IRConstant.h"
#include "ArrayType.h"

///
/// @brief 全局变量，寻址时通过符号名或变量名来寻址
///
class GlobalVariable : public GlobalValue {

public:
    ///
    /// @brief 构建全局变量，默认对齐为4字节
    /// @param _type 类型
    /// @param _name 名字
    ///
    explicit GlobalVariable(Type * _type, std::string _name) : GlobalValue(_type, _name), initialValue(nullptr)
    {
        // 设置对齐大小
        setAlignment(4);
    }

    ///
    /// @brief 构建带初始值的全局变量
    /// @param _type 类型
    /// @param _name 名字
    /// @param _initialValue 初始值
    ///
    explicit GlobalVariable(Type * _type, std::string _name, Value * _initialValue) 
        : GlobalValue(_type, _name), initialValue(_initialValue)
    {
        // 设置对齐大小
        setAlignment(4);
        // 有初始值，不在BSS段
        inBSSSection = false;
    }

    ///
    /// @brief 设置初始值
    /// @param value 初始值
    ///
    void setInitialValue(Value * value)
    {
        initialValue = value;
        if (value) {
            inBSSSection = false;
        }
    }

    ///
    /// @brief 获取初始值
    /// @return 初始值，nullptr表示无初始值
    ///
    Value * getInitialValue() const
    {
        return initialValue;
    }

    ///
    /// @brief  检查是否是函数
    /// @return true 是函数
    /// @return false 不是函数
    ///
    [[nodiscard]] bool isGlobalVarible() const override
    {
        return true;
    }

    ///
    /// @brief 是否属于BSS段的变量，即未初始化过的变量，或者初值都为0的变量
    /// @return true
    /// @return false
    ///
    [[nodiscard]] bool isInBSSSection() const
    {
        return this->inBSSSection;
    }

    ///
    /// @brief 取得变量所在的作用域层级
    /// @return int32_t 层级
    ///
    int32_t getScopeLevel() override
    {
        return 0;
    }

    ///
    /// @brief 对该Value进行Load用的寄存器编号
    /// @return int32_t 寄存器编号
    ///
    int32_t getLoadRegId() override
    {
        return this->loadRegNo;
    }

    ///
    /// @brief 对该Value进行Load用的寄存器编号
    /// @return int32_t 寄存器编号
    ///
    void setLoadRegId(int32_t regId) override
    {
        this->loadRegNo = regId;
    }

    ///
    /// @brief Declare指令IR显示
    /// @param str
    ///
    void toDeclareString(std::string & str)
    {
        if (initialValue) {
            // 有初始值的全局变量：declare i32 @a = 3
            if (getType()->isArrayType()) {
                // 数组类型暂时不支持初始化，使用declare
                ArrayType * arrayType = dynamic_cast<ArrayType *>(getType());
                Type * elementType = arrayType->getElementType();

                std::string dimensionsStr;
                for (int32_t dim: arrayType->getDimensions()) {
                    dimensionsStr += "[" + std::to_string(dim) + "]";
                }

                str = "declare " + elementType->toString() + " " + getIRName() + dimensionsStr;
            } else {
                // 标量类型支持初始化：DragonIR格式
                str = "declare " + getType()->toString() + " " + getIRName() + " = " + initialValue->getIRName();
            }
        } else {
            // 无初始值的全局变量：declare i32 @a
            if (getType()->isArrayType()) {
                ArrayType * arrayType = dynamic_cast<ArrayType *>(getType());
                Type * elementType = arrayType->getElementType();

                std::string dimensionsStr;
                for (int32_t dim: arrayType->getDimensions()) {
                    dimensionsStr += "[" + std::to_string(dim) + "]";
                }

                str = "declare " + elementType->toString() + " " + getIRName() + dimensionsStr;
            } else {
                str = "declare " + getType()->toString() + " " + getIRName();
            }
        }
    }

private:
    ///
    /// @brief 变量加载到寄存器中时对应的寄存器编号
    ///
    int32_t loadRegNo = -1;

    ///
    /// @brief 默认全局变量在BSS段，没有初始化，或者即使初始化过，但都值都为0
    ///
    bool inBSSSection = true;

    ///
    /// @brief 全局变量的初始值，nullptr表示无初始值
    ///
    Value * initialValue;
};
