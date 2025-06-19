///
/// @file FormalParam.h
/// @brief 函数形参描述类
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

#include "Value.h"
#include "IRConstant.h"
#include "../Types/ArrayType.h"
#include "../Types/PointerType.h"

/// @brief 描述函数形参类
class FormalParam : public Value {

public:
    /// @brief 基本类型的参数
    /// @param _name 形参的名字
    /// @param _type 基本类型
    FormalParam(Type * _type, std::string _name) : Value(_type)
    {
        this->name = _name;
        this->isArrayParam = false;
    };

    /// @brief 数组类型的参数
    /// @param _name 形参的名字
    /// @param _type 指针类型（用于实际处理）
    /// @param _arrayType 原始数组类型（用于显示）
    FormalParam(Type * _type, std::string _name, Type * _arrayType) : Value(_type)
    {
        this->name = _name;
        this->isArrayParam = true;
        this->originalArrayType = _arrayType;
    };

    /// @brief 获取参数的类型字符串表示（用于IR输出）
    /// @return 类型字符串
    std::string getTypeString() const
    {
        if (isArrayParam && originalArrayType) {
            // 数组形参显示格式：type name[0] 而不是 type[0] name
            ArrayType * arrType = dynamic_cast<ArrayType *>(originalArrayType);
            if (arrType) {
                return arrType->getElementType()->toString();
            }
        }
        return const_cast<FormalParam *>(this)->getType()->toString();
    }

    /// @brief 获取参数的完整字符串表示（包含名称和数组维度）
    /// @return 完整字符串
    std::string getFullString() const
    {
        if (isArrayParam && originalArrayType) {
            // 数组形参在函数签名中应显示为指针类型
            return const_cast<FormalParam*>(this)->getType()->toString() + " " + const_cast<FormalParam*>(this)->getIRName();
        }
        return getTypeString() + " " + const_cast<FormalParam*>(this)->getIRName();
    }

    /// @brief 判断是否是数组形参
    /// @return true 是数组形参，false 不是
    bool getIsArrayParam() const
    {
        return isArrayParam;
    }

    /// @brief 获取原始数组类型
    /// @return 原始数组类型指针
    Type * getOriginalArrayType() const
    {
        return originalArrayType;
    }

    // /// @brief 输出字符串
    // /// @param str
    // std::string toString() override
    // {
    //     return type->toString() + " " + IRName;
    // }

    ///
    /// @brief 获得分配的寄存器编号或ID
    /// @return int32_t 寄存器编号
    ///
    int32_t getRegId() override
    {
        return regId;
    }

    ///
    /// @brief @brief 如是内存变量型Value，则获取基址寄存器和偏移
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
    /// @brief 设置寄存器编号
    /// @param _regId 寄存器编号
    ///
    void setRegId(int32_t _regId)
    {
        this->regId = _regId;
    }

private:
    ///
    /// @brief 寄存器编号，-1表示没有分配寄存器，大于等于0代表寄存器分配后用寄存器寻址
    ///
    int32_t regId = -1;

    ///
    /// @brief 变量在栈内的偏移量，对于全局变量默认为0，临时变量没有意义
    ///
    int32_t offset = 0;

    ///
    /// @brief 栈内寻找时基址寄存器编号
    ///
    int32_t baseRegNo = -1;

    ///
    /// @brief 栈内寻找时基址寄存器名字
    ///
    std::string baseRegName;

    ///
    /// @brief 变量加载到寄存器中时对应的寄存器编号
    ///
    int32_t loadRegNo = -1;

    ///
    /// @brief 是否是数组形参
    ///
    bool isArrayParam = false;

    ///
    /// @brief 原始数组类型（用于显示）
    ///
    Type * originalArrayType = nullptr;
};
