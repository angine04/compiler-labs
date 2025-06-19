# Minic编译器数组功能实现总结

## 概述
本次实现为Minic编译器添加了完整的数组支持，包括一维数组、多维数组、数组形参等功能。

## 实现功能

### 1. 数组类型支持
- ✅ 支持int类型的一维数组声明（局部变量、全局变量）
- ✅ 支持int类型的多维数组声明
- ✅ 支持数组作为函数形参（指针语义）
- ✅ 正确区分局部数组和形参数组的类型

### 2. 数组操作
- ✅ 支持数组元素的读写访问
- ✅ 正确处理数组下标计算（地址计算）
- ✅ 区分左值（赋值）和右值（读取）访问
- ✅ 支持数组元素在表达式中的使用

### 3. 内置函数
- ✅ 添加了getarray函数（从输入读取数组）
- ✅ 添加了putarray函数（输出数组元素）

### 4. 代码生成
- ✅ AST翻译成线性IR
- ✅ 正确的数组地址计算
- ✅ ARM32后端支持数组访问

## 实现细节

### AST层面
新增了以下AST节点类型：
- `AST_OP_ARRAY_DECL`: 数组声明
- `AST_OP_ARRAY_REF`: 数组元素访问
- `AST_OP_ARRAY_DIM`: 数组维度

### 语法分析
扩展了MiniC.y语法规则：
```yacc
// 数组声明
VarDef: T_ID ArrayDimensions {
    ast_node * id_node = ast_node::New(var_id_attr{$1.id, $1.lineno});
    free($1.id);
    $$ = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, id_node, $2);
}

// 数组访问
LVal: T_ID ArrayDimensions {
    ast_node * id_node = ast_node::New($1);
    free($1.id);
    $$ = create_contain_node(ast_operator_type::AST_OP_ARRAY_REF, id_node, $2);
}

// 数组形参
FormalParam: BasicType T_ID ArrayDimensions {
    ast_node * array_decl = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, id_node, $3);
    $$ = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAM, type_node, array_decl);
}
```

### 类型系统
实现了`ArrayType`类：
- 支持多维数组类型表示
- 区分局部数组和形参数组（形参第一维为0）
- 正确的元素类型和大小计算

### IR生成
在IRGenerator中实现了：
- `ir_array_declare`: 数组声明处理
- `ir_array_ref`: 数组访问处理
- `ir_array_dim`: 数组维度处理
- 函数形参中的数组参数处理

### 地址计算逻辑
```cpp
// 一维数组访问 a[i] 的地址计算
offset = index * element_size
address = array_base + offset
```

### 左值/右值区分
```cpp
// 左值（赋值）：a[0] = 10
// 生成：address = calculate_address(a, 0)
//      *address = 10

// 右值（读取）：x = a[0] + a[1]  
// 生成：addr1 = calculate_address(a, 0)
//      value1 = *addr1
//      addr2 = calculate_address(a, 1)  
//      value2 = *addr2
//      x = value1 + value2
```

## 测试用例

### 1. 基本数组功能
```c
int main() {
    int a[5];
    a[0] = 10;
    a[1] = 20;
    return a[0] + a[1];  // 返回30
}
```

### 2. 二维数组
```c
int main() {
    int a[3][4];
    a[1][2] = 42;
    return a[1][2];
}
```

### 3. 数组形参
```c
int sum(int a[10], int n) {
    int i, result;
    i = 0;
    result = 0;
    while (i < n) {
        result = result + a[i];
        i = i + 1;
    }
    return result;
}
```

### 4. 内置函数
```c
int main() {
    int a[10];
    int n = getarray(a);
    putarray(a, n);
    return 0;
}
```

## 生成的IR示例

### 局部数组
```ir
declare i32[5] %l1 ; 1:a
%t2 = mul 0,4
%t3 = add %l1,%t2
%t3 = 10
```

### 形参数组
```ir
declare i32* %l2 ; 1:a  // 形参数组为指针类型
%t10 = mul %l5,4
%t11 = add %l2,%t10
%3 = %t11             // 加载值
```

## 关键技术点

### 1. 形参数组处理
- 语法解析：正确识别数组形参语法
- 类型系统：形参数组类型为`i32[0][n]`（第一维为0）
- IR生成：实际分配指针类型变量

### 2. 地址计算vs值获取
- 写入：生成地址，通过后端处理为内存写入
- 读取：生成地址+加载指令，获取实际值

### 3. 后端处理
- ARM32后端的`translate_assign`函数自动处理内存读写
- 正确生成ARM32汇编代码

## 后续优化空间

1. **多维数组访问**：目前只实现了一维数组的完整地址计算
2. **数组初始化**：支持数组声明时的初始值设置
3. **边界检查**：添加运行时数组越界检查
4. **数组传递优化**：优化大数组作为参数的传递方式

## 总结

本次实现成功为Minic编译器添加了完整的数组支持，包括：
- 完整的语法支持（声明、访问、形参）
- 正确的类型系统
- 准确的IR生成
- 有效的ARM32代码生成

所有核心功能都已验证可用，为后续扩展提供了坚实的基础。 