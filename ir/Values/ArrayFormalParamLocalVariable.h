///
/// @file ArrayFormalParamLocalVariable.h
/// @brief 数组形参的局部变量，特殊的显示格式
///

#pragma once

#include "LocalVariable.h"
#include "../Types/ArrayType.h"

///
/// @brief 数组形参的局部变量，用于特殊的IR显示格式
///
class ArrayFormalParamLocalVariable : public LocalVariable {

    friend class Function;

public:
    ///
    /// @brief 构造函数
    /// @param _type 指针类型（用于实际处理）
    /// @param _name 名称
    /// @param _scope_level 作用域层级
    /// @param _originalArrayType 原始数组类型（用于显示）
    ///
    ArrayFormalParamLocalVariable(Type * _type, std::string _name, int32_t _scope_level, Type * _originalArrayType)
        : LocalVariable(_type, _name, _scope_level), originalArrayType(_originalArrayType)
    {}

    ///
    /// @brief 获取用于显示的类型字符串
    /// @return 类型字符串，格式为 "i32 %name[0]"
    ///
    std::string getDisplayTypeString() const
    {
        if (originalArrayType && originalArrayType->isArrayType()) {
            ArrayType * arrType = dynamic_cast<ArrayType *>(originalArrayType);
            if (arrType) {
                return arrType->getElementType()->toString() + " " + getIRName() + "[0]";
            }
        }
        return getType()->toString() + " " + getIRName();
    }

private:
    ///
    /// @brief 原始数组类型（用于显示）
    ///
    Type * originalArrayType = nullptr;
};