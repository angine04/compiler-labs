# MiniC 编译器增强计划：实现分支与循环

## 1. 引言

本文档总结了为 MiniC 编译器添加分支（if-else）和循环（while）控制结构的需求、设计思路和初步检查计划。主要依据是用户提供的 "分支与循环.md" 文档，该文档详细描述了这些结构如何翻译成 "DragonIR" 中间表示。

## 2. 核心目标 (源自 "分支与循环.md")

- 实现关系表达式 (`>`, `<`, `>=`, `<=`, `==`, `!=`) 的翻译。
- 实现布尔表达式 (`&&`, `||`, `!`) 的翻译，采用短路求值策略。
- 实现 `if-else` 分支语句的翻译。
- 实现 `while` 循环语句的翻译。
- 最终目标是能够通过提供的 `IRCompiler` (DragonIR 虚拟机) 来运行生成的 `.ir` 文件，并验证其正确性。

## 3. 关键实现细节 (源自 "分支与循环.md")

### 3.1. 关系表达式翻译

- **AST 结构**: 操作符节点，左右孩子为操作数。
- **IR 生成**:
    1. 计算左右操作数的值 ->临时变量 (如 `%t1`, `%t2`)。
    2. 比较指令 (`cmp <op> %t1, %t2`) -> 布尔结果到新临时变量 (如 `%t3`)。
    3. 条件跳转 (`bc %t3, label .L_true, label .L_false`)。
- **真假出口**: 由父节点传递给关系运算符节点。

### 3.2. 布尔表达式翻译 (短路求值)

- **逻辑与 (`&&`)**: `BoolExpr0 && BoolExpr1`
    - 若 `BoolExpr0` 为假，跳转到整体假出口。
    - 若 `BoolExpr0` 为真，计算 `BoolExpr1`，其结果决定整体结果。
    - `BoolExpr0` 的真出口是 `BoolExpr1` 的入口。
- **逻辑或 (`||`)**: `BoolExpr0 || BoolExpr1`
    - 若 `BoolExpr0` 为真，跳转到整体真出口。
    - 若 `BoolExpr0` 为假，计算 `BoolExpr1`。
- **逻辑非 (`!`)**: `!BoolExpr`
    - 交换传递给 `BoolExpr` 的真假出口。
- **实现方式**: 建议在遍历孩子节点前由参数传入真假出口 Label 指令。

### 3.3. `if-else` 语句翻译

- **AST 结构**: `IF` 节点 (子节点: `CondExpr`, `TrueBlock`, 可选 `FalseBlock`)。
- **IR 生成**:
    1. 创建 Labels: `L_true` (真分支入口), `L_false` (假分支入口), `L_exit` (语句出口)。
    2. 翻译 `CondExpr` (真出口 -> `L_true`, 假出口 -> `L_false`)。
    3. `L_true:` 后翻译 `TrueBlock`。
    4. `TrueBlock` 末尾: `br label L_exit`。
    5. `L_false:` 后 (若有 `FalseBlock`) 翻译 `FalseBlock`。
    6. `L_exit:` 标记语句结束。

### 3.4. `while` 语句翻译

- **AST 结构**: `WHILE` 节点 (子节点: `CondExpr`, `BodyBlock`)。
- **IR 生成**:
    1. 创建 Labels: `L_cond` (条件判断入口), `L_body` (循环体入口), `L_exit` (循环出口)。
    2. `L_cond:`
    3. 翻译 `CondExpr` (真出口 -> `L_body`, 假出口 -> `L_exit`)。
    4. `L_body:` 后翻译 `BodyBlock`。
    5. `BodyBlock` 末尾: `br label L_cond`。
    6. `L_exit:` 标记循环结束。

### 3.5. 相关 DragonIR 指令

- `\%t1 = cmp <op> \%val1, \%val2` (比较: `lt`, `gt`, `le`, `ge`, `eq`, `ne`)
- `bc \%cond, label .L_true, label .L_false` (条件跳转)
- `br label .L_target` (无条件跳转)
- `label .L_name` (定义标签)

## 4. 对现有代码的检查方向和计划

### 4.1. 词法分析器 (Lexer - `.l` 或 `.g4` 文件)

- **检查点**:
    - 是否定义关系运算符 Token: `>`, `<`, `>=`, `<=`, `==`, `!=`。
    - 是否定义逻辑运算符 Token: `&&`, `||`, `!`。
    - 是否定义关键字 Token: `if`, `else`, `while`。
- **行动**: 如果缺失，需要添加。

### 4.2. 语法分析器 (Parser - `.y` 或 `.g4` 文件)

- **检查点**:
    - 是否定义关系表达式、逻辑表达式的语法规则 (处理优先级和结合性)。
    - 是否定义 `if-else` 语句的语法规则 (包括仅 `if` 的情况)。
    - 是否定义 `while` 语句的语法规则。
    - 这些规则是否能构建出文档中描述的 AST 结构。
- **行动**: 如果缺失或不正确，需要添加或修改。

### 4.3. 抽象语法树 (`AST.h`, `AST.cpp`)

- **检查点**:
    - `ast_operator_type` 是否包含新运算符类型:
        - 关系运算: `AST_OP_GT`, `AST_OP_LT`, `AST_OP_GE`, `AST_OP_LE`, `AST_OP_EQ`, `AST_OP_NE`。
        - 逻辑运算: `AST_OP_LOGICAL_AND`, `AST_OP_LOGICAL_OR`, `AST_OP_LOGICAL_NOT`。
        - 控制流: `AST_OP_IF`, `AST_OP_WHILE`。
    - `ast_node` 结构是否能支持新语句 (如 `IF` 节点的条件、真/假分支；`WHILE` 节点的条件、循环体)。
- **行动**: 如果缺失，需要添加。

### 4.4. IR 定义 (`ir/` 目录)

- **检查点**:
    - (`Instruction.h`, `IRCode.h` 等)
    - 是否定义 `CmpInstruction` (比较指令)。
    - 是否定义 `BranchIfConditionInstruction` (`bc`) 和 `BranchInstruction` (`br`)。
    - `LabelInstruction` 是否已存在或需要增强。
- **行动**: 如果缺失，需要添加或修改。

### 4.5. IR 生成器 (`ir/IRGenerator.cpp`)

- **核心修改区域**
- **检查点/行动**:
    - 为新的 AST 节点类型添加 `visit` 方法或处理逻辑。
    - 实现文档中描述的算法生成 DragonIR 指令序列。
    - 正确处理标签的创建、传递（如通过函数参数）和使用。

### 4.6. 符号表/作用域 (`symboltable/`)

- **检查点**:
    - 确认分支和循环语句内部的语句块 (`{...}`) 能正确遵循现有作用域规则。
- **行动**: 一般情况下预计不需要大改，但需确认。

## 5. 下一步具体行动

1.  **检查词法和语法定义**: 从 Flex/Bison (`frontend/flexbison/MiniC.l` 和 `frontend/flexbison/MiniC.y`) 或 ANTLR4 (`frontend/antlr4/MiniC.g4`) 开始。
2.  **检查 AST 定义** (`AST.h`)。
3.  **检查 IR 定义** (`ir/` 目录)。
4.  **规划并实现 IR 生成逻辑** (`IRGenerator.cpp`)。

---

文档已准备就绪。 