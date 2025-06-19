# 函数功能实现总结

## 实现的功能

本次为MiniC编译器实现了完整的函数相关功能，满足了所有要求：

### 1. 函数定义支持多个形参

- 修改了语法文件 `frontend/antlr4/MiniC.g4`，添加了形参列表的语法规则
- 支持语法：`funcDef: (T_INT | T_VOID) T_ID T_L_PAREN formalParamList? T_R_PAREN block;`
- 形参列表：`formalParamList: formalParam (T_COMMA formalParam)*;`
- 单个形参：`formalParam: basicType T_ID;`

### 2. 函数调用支持多个实参

- 原有的实参列表语法已经支持多个实参
- 实参列表：`realParamList: expr (T_COMMA expr)*;`
- 在IR生成时正确处理实参传递

### 3. 函数返回类型支持int和void

- 添加了 `T_VOID: 'void';` 词法规则
- 函数定义语法支持 `(T_INT | T_VOID)` 返回类型
- 在IR生成时正确处理void类型函数

### 4. 支持内置函数

在 `symboltable/Module.cpp` 中添加了标准库函数：
- `putint(int k)` - 输出整数
- `getint()` - 输入整数  
- `putch(int c)` - 输出字符
- `getch()` - 输入字符

### 5. 形参的值传递和局部变量处理

在 `ir/Generator/IRGenerator.cpp` 中实现：
- 为每个形参创建 `FormalParam` 对象表示实参值
- 为每个形参创建对应的局部变量用于函数内部使用
- 生成 `MoveInstruction` 将形参值复制到局部变量

### 6. 返回值变量管理

- 为非void函数创建返回值变量
- 确保函数只有一个出口（通过跳转到统一的exit标签）
- return语句将值赋给返回值变量，然后跳转到函数出口

### 7. main函数返回值初始化

- 为main函数的返回值变量初始化为0
- 避免进程退出状态的随机值问题

## 修改的文件

### 前端语法分析
1. `frontend/antlr4/MiniC.g4` - 添加形参语法规则和void类型
2. `frontend/antlr4/Antlr4CSTVisitor.h` - 添加形参访问器声明
3. `frontend/antlr4/Antlr4CSTVisitor.cpp` - 实现形参访问器和修改函数定义处理

### IR代码生成
4. `ir/Generator/IRGenerator.cpp` - 修改函数定义和形参处理的IR生成
5. `symboltable/Module.cpp` - 添加内置函数定义

## 测试验证

创建了测试文件验证功能：

### test_func.c
```c
int add(int a, int b) {
    return a + b;
}

int multiply(int x, int y, int z) {
    return x * y * z;
}

void printResult(int result) {
    putint(result);
    putch(10);
}

int main() {
    int x, y, z;
    int result1, result2;
    
    x = 5;
    y = 3;
    z = 2;
    
    result1 = add(x, y);
    result2 = multiply(x, y, z);
    
    printResult(result1);
    printResult(result2);
    
    return 0;
}
```

### 生成的IR代码特点
- 正确处理多个形参：`define i32 @add(i32%t0, i32%t1)`
- 支持void返回类型：`define void @printResult(i32%t0)`
- 形参到局部变量的转换：`%l2 = %t0`, `%l3 = %t1`
- 函数调用传递多个参数：`call i32 @multiply(i32 %l1, i32 %l2, i32 %l3)`
- main函数返回值初始化：`%l0 = 0`

## 编译和运行

使用antlr4前端编译：
```bash
./minic -A -S test_func.c -o test_func.s
```

生成的汇编代码正确实现了函数调用约定，包括参数传递和返回值处理。

## 总结

本次实现完全满足了所有要求：
1. ✅ 函数定义可支持多个形参
2. ✅ 函数调用可支持多个实参  
3. ✅ 函数可返回int类型或void类型
4. ✅ 支持std.c中定义的内置函数
5. ✅ 形参对应实参值和局部变量两个值
6. ✅ 返回值变量确保函数只有一个出口
7. ✅ main函数返回值初始化为0

编译器现在具备了完整的函数功能，可以处理复杂的函数定义、调用和参数传递。 