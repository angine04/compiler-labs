///
/// @file ArrayType.h
/// @brief 数组类型类，可描述一维和多维数组类型
///
/// @author AI Assistant
/// @version 1.0
/// @date 2024-01-20
///
/// @copyright Copyright (c) 2024
///

#pragma once

#include <cstdint>
#include <vector>

#include "Type.h"

class ArrayType final : public Type {

public:
    ///
    /// @brief 创建数组类型
    /// @param elementType 数组元素类型
    /// @param dimensions 各维度大小
    /// @return ArrayType*
    ///
    static ArrayType * getType(Type * elementType, const std::vector<int32_t> & dimensions);

    ///
    /// @brief 获取类型的IR标识符
    /// @return std::string IR标识符
    ///
    [[nodiscard]] std::string toString() const override;

    ///
    /// @brief 获取数组元素类型
    /// @return Type*
    ///
    [[nodiscard]] Type * getElementType() const
    {
        return elementType;
    }

    ///
    /// @brief 获取数组维度信息
    /// @return const std::vector<int32_t>&
    ///
    [[nodiscard]] const std::vector<int32_t> & getDimensions() const
    {
        return dimensions;
    }

    ///
    /// @brief 获取数组维度数
    /// @return int32_t
    ///
    [[nodiscard]] int32_t getNumDimensions() const
    {
        return static_cast<int32_t>(dimensions.size());
    }

    ///
    /// @brief 获取数组总元素个数
    /// @return int32_t
    ///
    [[nodiscard]] int32_t getTotalElements() const;

    ///
    /// @brief 获得类型所占内存空间大小
    /// @return int32_t
    ///
    [[nodiscard]] int32_t getSize() const override;

    ///
    /// @brief 检查两个数组类型是否相等
    /// @param other 另一个数组类型
    /// @return bool
    ///
    [[nodiscard]] bool equals(const ArrayType * other) const;

private:
    ///
    /// @brief 构造函数
    /// @param elementType 数组元素类型
    /// @param dimensions 各维度大小
    ///
    ArrayType(Type * elementType, const std::vector<int32_t> & dimensions);

    ///
    /// @brief 数组元素类型
    ///
    Type * elementType;

    ///
    /// @brief 各维度大小
    ///
    std::vector<int32_t> dimensions;

    ///
    /// @brief 数组类型实例缓存
    ///
    static std::vector<ArrayType *> instances;
};