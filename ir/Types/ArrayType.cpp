///
/// @file ArrayType.cpp
/// @brief 数组类型类的实现
///

#include "ArrayType.h"
#include <algorithm>
#include <sstream>

// 静态成员变量定义
std::vector<ArrayType *> ArrayType::instances;

ArrayType::ArrayType(Type * elementType, const std::vector<int32_t> & dimensions)
    : Type(Type::ArrayTyID), elementType(elementType), dimensions(dimensions)
{}

ArrayType * ArrayType::getType(Type * elementType, const std::vector<int32_t> & dimensions)
{
    // 查找是否已有相同的数组类型实例
    for (auto * instance: instances) {
        if (instance->elementType == elementType && instance->dimensions == dimensions) {
            return instance;
        }
    }

    // 创建新的数组类型实例
    auto * newType = new ArrayType(elementType, dimensions);
    instances.push_back(newType);
    return newType;
}

std::string ArrayType::toString() const
{
    std::ostringstream oss;
    oss << elementType->toString();

    for (int32_t dim: dimensions) {
        oss << "[" << dim << "]";
    }

    return oss.str();
}

int32_t ArrayType::getTotalElements() const
{
    int32_t total = 1;
    for (int32_t dim: dimensions) {
        total *= dim;
    }
    return total;
}

int32_t ArrayType::getSize() const
{
    return getTotalElements() * elementType->getSize();
}

bool ArrayType::equals(const ArrayType * other) const
{
    if (!other) {
        return false;
    }

    return elementType == other->elementType && dimensions == other->dimensions;
}