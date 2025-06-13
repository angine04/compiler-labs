///
/// @file ArrayFormalParamLocalVariable.h
/// @brief 数组形参的局部变量，特殊的显示格式
///

#pragma once

#include "Value.h"
#include "../Types/ArrayType.h"

///
/// @brief 数组形参的局部变量，用于特殊的IR显示格式
///
class ArrayFormalParamLocalVariable : public Value {

public:
    ///
    /// @brief 构造函数
    /// @param _type 指针类型（用于实际处理）
    /// @param _name 名称
    /// @param _scope_level 作用域层级
    /// @param _originalArrayType 原始数组类型（用于显示）
    ///
    ArrayFormalParamLocalVariable(Type * _type, std::string _name, int32_t _scope_level, Type * _originalArrayType)
        : Value(_type), scope_level(_scope_level), originalArrayType(_originalArrayType)
    {
        this->name = _name;
    }

    ///
    /// @brief 获取用于显示的类型字符串（用于declare语句）
    /// @return 类型字符串，格式为 "i32 %name[0]"
    ///
    std::string getDisplayTypeString() const
    {
        if (originalArrayType && originalArrayType->isArrayType()) {
            ArrayType * arrType = dynamic_cast<ArrayType *>(originalArrayType);
            if (arrType) {
                return arrType->getElementType()->toString();
            }
        }
        return type->toString();
    }

    ///
    /// @brief 获取作用域层级
    /// @return int32_t 层级
    ///
    int32_t getScopeLevel() override
    {
        return scope_level;
    }

    ///
    /// @brief 获得分配的寄存器编号或ID
    /// @return int32_t 寄存器编号
    ///
    int32_t getRegId() override
    {
        return regId;
    }

    ///
    /// @brief 设置寄存器编号
    /// @param _regId 寄存器编号
    ///
    void setRegId(int32_t _regId)
    {
        this->regId = _regId;
    }

    ///
    /// @brief 如是内存变量型Value，则获取基址寄存器和偏移
    /// @param regId 寄存器编号
    /// @param offset 相对偏移
    /// @return true 是内存型变量
    /// @return false 不是内存型变量
    ///
    bool getMemoryAddr(int32_t * _regId = nullptr, int64_t * _offset = nullptr) override
    {
        if (this->baseRegNo == -1) {
            return false;
        }

        if (_regId) {
            *_regId = this->baseRegNo;
        }
        if (_offset) {
            *_offset = this->offset;
        }

        return true;
    }

    ///
    /// @brief 设置内存寻址的基址寄存器和偏移
    /// @param _regId 基址寄存器编号
    /// @param _offset 偏移
    ///
    void setMemoryAddr(int32_t _regId, int64_t _offset)
    {
        baseRegNo = _regId;
        offset = _offset;
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
    /// @brief 检查是否是数组形参局部变量
    /// @return true
    ///
    bool isArrayFormalParamLocalVariable() const
    {
        return true;
    }

private:
    ///
    /// @brief 作用域层级
    ///
    int scope_level = -1;

    ///
    /// @brief 原始数组类型（用于显示）
    ///
    Type * originalArrayType = nullptr;

    /// @brief 寄存器编号，-1表示没有分配寄存器，大于等于0代表是寄存器型Value
    int32_t regId = -1;

    /// @brief 变量在栈内的偏移量，对于全局变量默认为0，临时变量没有意义
    int32_t offset = 0;

    /// @brief 栈内寻找时基址寄存器编号
    int32_t baseRegNo = -1;

    ///
    /// @brief 变量加载到寄存器中时对应的寄存器编号
    ///
    int32_t loadRegNo = -1;
};