# MiniC 编译器开发指南

## 1. 引言

本文档旨在为 MiniC 编译器的开发者提供指导，帮助理解项目结构，并顺利地为前端（C语言特性支持到IR）和后端（IR到ARM32汇编）添加新功能。

## 2. 项目结构分析

MiniC 编译器主要可以划分为前端、中间表示（IR）和后端三个主要部分。

### 2.1. 主要目录结构

```
.
├── backend/arm32/       # ARM32 后端实现
│   ├── ILocArm32.cpp
│   ├── ILocArm32.h
│   ├── InstSelectorArm32.cpp
│   ├── InstSelectorArm32.h
│   ├── PlatformArm32.h         # ARM平台相关定义（如寄存器名）
│   └── SimpleRegisterAllocator.h # 简单的寄存器分配器
├── build/                 # 编译输出目录 (生成的 .ir, .s 文件等)
├── docs/                  # 项目文档 (本文档所在位置)
├── ir/                    # 中间表示 (IR) 相关定义和实现
│   ├── Generator/
│   │   └── IRGenerator.cpp   # IR 生成器实现
│   ├── Instructions/         # IR 指令定义 (e.g., BranchInstruction.cpp)
│   │   ├── Instruction.h     # 指令基类
│   │   ├── BinaryInstruction.h
│   │   └── ...               # 其他指令类
│   ├── Values/               # IR 值定义 (e.g., ConstInt.cpp)
│   │   ├── Value.h           # Value 基类
│   │   ├── ConstInt.h
│   │   └── ...               # 其他值类
│   ├── Function.h            # IR Function 定义
│   ├── Function.cpp
│   ├── Type.h                # IR Type 定义
│   ├── Module.h              # IR Module 定义 (通常包含多个Function)
│   └── ...                   # 其他 IR 核心组件
├── tests/                 # C 语言测试用例 (e.g., test4.c, test5.c)
├── Makefile               # 项目构建脚本
└── ...                    # 其他源文件 (如main.cpp, 前端词法/语法分析器等)
```

### 2.2. 核心模块与职责

*   **前端 (Frontend)**:
    *   **词法分析器 (Lexer)**: 将C源代码分解为词法单元 (tokens)。(未在对话中详述，但为标准组件)
    *   **语法分析器 (Parser)**: 基于词法单元构建抽象语法树 (AST)。(未在对话中详述，但为标准组件)
    *   **语义分析器 (Semantic Analyzer)**: 进行类型检查、作用域分析等。(未在对话中详述，但为标准组件)
    *   **`IRGenerator` (`ir/Generator/IRGenerator.cpp`)**: 遍历AST，将C语言结构翻译成MiniC的中间表示（IR）。这是前端功能扩展的主要工作区域。

*   **中间表示 (IR)**:
    *   **`Value` (`ir/Values/Value.h`)**: IR中所有值的基类，例如常量 (`ConstInt`)、变量 (`LocalVariable`, `GlobalVariable`)、指令本身（如果指令产生结果）。
    *   **`Type` (`ir/Type.h`)**: 表示IR中的数据类型，如 `IntegerType` (i32, i1), `VoidType`, `PointerType`。
    *   **`Instruction` (`ir/Instructions/Instruction.h`)**: IR中所有指令的基类。具体指令如：
        *   `BinaryInstruction`: 用于算术和比较运算 (`icmp`, `add`, `sub` 等)。
        *   `BranchInstruction`: 条件分支 (`bc`)。
        *   `GotoInstruction`: 无条件跳转 (`br`)。
        *   `LabelInstruction`: 标签定义。
        *   `MoveInstruction`: 赋值/移动。
        *   `FuncCallInstruction`: 函数调用。
    *   **`BasicBlock`**: IR指令的基本单元，由一个入口标签和一系列顺序执行的指令组成，以跳转指令结束 (概念上存在，通过`LabelInstruction`和跳转指令界定)。
    *   **`Function` (`ir/Function.h`)**: 表示一个函数，包含指令列表、局部变量表、参数列表、基本块（通过标签和指令流体现）等。提供了创建临时变量、标签的接口。
    *   **`Module` (`ir/Module.h`)**: 顶层IR容器，通常包含一个程序中的所有函数和全局变量。

*   **后端 (Backend - ARM32)**:
    *   **`InstSelectorArm32` (`backend/arm32/InstSelectorArm32.cpp`)**: 指令选择器。核心职责是将IR指令翻译成目标平台（ARM32）的汇编指令序列。
        *   包含一个 `translator_handlers` 映射，将 `IRInstOperator` (IR操作码) 映射到对应的 `translate_...` 处理函数。
    *   **`ILocArm32` (`backend/arm32/ILocArm32.cpp`)**: 底层指令代码生成器。提供API来生成和管理 `ArmInst` 对象序列。
        *   封装了ARM汇编指令的创建、格式化输出等。
        *   提供了如 `load_var`, `store_var`, `inst`, `jump`, `label` 等便捷方法。
    *   **`ArmInst` (在 `ILocArm32.cpp` 或 `ILocArm32.h` 中定义)**: 表示一条具体的ARM汇编指令，包含操作码、条件码、操作数等字段，以及一个 `outPut()` 方法用于生成其字符串表示。
    *   **`PlatformArm32` (`backend/arm32/PlatformArm32.h`)**: 定义了ARM32平台相关的常量，如寄存器名称、栈指针/帧指针的编号等。
    *   **`SimpleRegisterAllocator` (`backend/arm32/SimpleRegisterAllocator.h`)**: 一个简单的寄存器分配器，供指令选择阶段使用。

## 3. 前端功能添加流程 (C语言新特性 -> IR)

当需要为MiniC添加新的C语言特性时（例如新的循环类型、新的操作符等），主要流程如下：

### 3.1. 词法与语法分析
1.  **词法分析**: 如果新特性引入了新的关键字或符号，需要更新词法分析器规则。
2.  **语法分析**: 为新特性定义新的语法规则，并更新语法分析器以能解析它，并构建相应的AST节点。

### 3.2. AST节点定义
*   如果新特性对应新的语法结构，通常需要定义一个新的AST节点类来表示它。

### 3.3. 语义分析
*   对新的AST节点实现语义分析逻辑，包括：
    *   类型检查。
    *   符号表处理（例如，变量声明、作用域）。
    *   其他语义规则校验。

### 3.4. IR生成 (主要修改 `IRGenerator.cpp`)
这是将新C语言特性映射到编译器内部表示的关键步骤。
1.  **设计IR表示**: 考虑如何用现有的IR指令（或在必要时设计新的IR指令）来表达新特性的语义。
    *   例如，控制流特性（如新的循环或选择语句）通常会被分解为 `LabelInstruction`, `icmp` (`BinaryInstruction`), `BranchInstruction`, 和 `GotoInstruction` 的组合。
    *   参考 `test4.ir` 和 `output.ir` 中 `if-else`, `while`, `&&`, `||` 的实现方式。
2.  **实现AST遍历与IR发射**:
    *   在 `IRGenerator` 中为新的AST节点类型添加一个访问方法 (e.g., `visitNewFeatureNode(NewFeatureNode* node)`)。
    *   在该方法中，按设计的IR表示逻辑，逐步生成IR指令序列。
    *   **关键API和概念**:
        *   获取当前 `Function` 对象: `this->builder->getFunction()`。
        *   创建标签: `currentFunction->newLabelName()` 生成标签名, `new LabelInstruction(currentFunction, labelName)` 创建标签指令。
        *   创建临时变量 (SSA形式): `currentFunction->newTempValueName()` (如果需要显式命名) 或直接将指令作为`Value`使用。
        *   创建局部变量: `currentFunction->newLocalVarValue(type, name)`。
        *   添加指令到当前基本块: `currentFunction->addInst(new SomeInstruction(...))`。
        *   管理基本块: 虽然没有显式的 `BasicBlock` 对象传递，但通过 `addInst` 和正确放置 `LabelInstruction` 与跳转指令来构建控制流图。
3.  **注意事项**:
    *   **SSA形式**: 尽量保持生成的IR接近SSA形式，即每个变量只被赋值一次。临时变量是实现这一点的好方法。
    *   **短路求值**: 对于新的逻辑运算符，需要在IR层面通过条件分支来实现短路。
    *   **类型转换**: 如果新特性涉及隐式或显式类型转换，确保在IR层面插入相应的转换指令（如果IR支持的话，或者通过运算模拟）。

### 3.5. 编写测试用例
*   在 `tests/` 目录下为新特性编写C语言测试用例，覆盖各种正常和边界情况。

## 4. 后端功能添加流程 (新IR指令/特性 -> ARM32汇编)

当需要让ARM32后端支持新的IR指令，或者一个已通过IR表达的新C语言特性需要后端翻译时：

### 4.1. (可选) 定义新的IR指令
*   如果现有IR指令不足以表达新特性，且无法用现有指令组合表示，则可能需要定义新的IR指令类型。
    1.  在 `ir/Instructions/` 下创建新的指令类 (e.g., `NewInstruction.h`, `NewInstruction.cpp`)，继承自 `Instruction`。
    2.  定义其操作码 (`IRInstOperator`)、操作数和行为。
    3.  (可能需要) 更新IR的文本表示法和解析器（如果存在）。

### 4.2. 指令选择器修改 (`InstSelectorArm32`)
这是将新IR指令（或现有IR指令的新用法）翻译成ARM汇编的核心。
1.  **声明翻译处理函数 (Handler)**:
    *   在 `InstSelectorArm32.h` 中，为新的IR指令或需要特殊处理的IR指令声明一个新的 `private` 成员函数：
        ```cpp
        void translate_new_ir_feature(Instruction *inst);
        ```
2.  **注册处理函数**:
    *   在 `InstSelectorArm32.cpp` 的构造函数中，将新的 `IRInstOperator` 映射到刚声明的处理函数：
        ```cpp
        translator_handlers[IRInstOperator::IRINST_OP_NEW_FEATURE] = &InstSelectorArm32::translate_new_ir_feature;
        ```
3.  **实现处理函数**:
    *   在 `InstSelectorArm32.cpp` 中实现该函数，例如 `void InstSelectorArm32::translate_new_ir_feature(Instruction *inst)`。
    *   **步骤**:
        a.  **类型转换**: 将通用的 `Instruction* inst` 转换为其具体的类型，以便访问特有的操作数或方法：
            ```cpp
            InstanceOf(specificInst, SpecificInstructionType*, inst); // 使用 Instanceof 宏或 dynamic_cast
            assert(specificInst && "Expected SpecificInstructionType");
            ```
        b.  **获取操作数**: 从 `specificInst` 中获取操作数，例如 `Value* operand1 = specificInst->getOperand(0);`。
        c.  **寄存器分配**:
            *   为源操作数和目标结果（如果指令产生结果）分配寄存器。使用 `simpleRegisterAllocator`:
                ```cpp
                int32_t reg_op1 = -1, reg_op2 = -1, reg_dest = -1;
                // For operand1
                if (operand1->getRegId() != -1) { // Already in a register
                    reg_op1 = operand1->getRegId();
                } else {
                    reg_op1 = simpleRegisterAllocator.Allocate(operand1); // Allocate if a Value, or Allocate() for temp
                    iloc.load_var(reg_op1, operand1); // Load from memory/const if not in reg
                }
                // Similar for operand2
                // For destination (if inst is a Value and has a regId pre-assigned or needs one)
                if (inst->getRegId() != -1) {
                    reg_dest = inst->getRegId();
                } else {
                     // If the instruction itself is a Value that needs a register for its result
                    reg_dest = simpleRegisterAllocator.Allocate(static_cast<Value*>(inst));
                }
                ```
        d.  **生成ARM指令**: 使用 `ILocArm32& iloc` 对象的方法来发射ARM汇编指令：
            *   `iloc.inst("op", "rd", "rn", "rm")`: 通用指令发射。
            *   `iloc.load_var(reg_no, value*)`: 加载变量/常量到寄存器。
            *   `iloc.store_var(reg_no, value*, tmp_reg_no)`: 将寄存器存回变量。
            *   `iloc.jump("labelName")`: 无条件跳转。
            *   `iloc.label("labelName")`: 定义标签。
            *   条件跳转通常由 `translate_branch_conditional` 处理，它会根据 `icmp` 指令的条件生成如 `bne`, `beq` 等，这些指令的opcode是动态构建的 (`"b" + cond_suffix`) 然后通过 `iloc.inst(cond_op, label)` 发射。
        e.  **结果存储**: 如果指令产生结果并且该结果需要写回内存（而不是仅存在于寄存器中供下一条指令使用），则使用 `iloc.store_var()`。
        f.  **寄存器释放**: 如果 `simpleRegisterAllocator` 的策略要求显式释放临时分配的寄存器，确保在适当的时候调用 `simpleRegisterAllocator.free()`。 (当前 `translate_two_operator` 示例在操作完成后释放操作数和结果的寄存器。)

### 4.3. (可选) `ILocArm32` 或 `ArmInst` 扩展
*   如果新特性需要生成一种 `ILocArm32` 当前不直接支持的特殊ARM指令或寻址模式，或 `ArmInst::outPut()` 无法正确格式化：
    1.  可能需要在 `ArmInst` 中添加新的字段或修改 `outPut()` 逻辑 (如我们之前为 `cmp` 指令修复逗号问题所做的)。
    2.  可能需要在 `ILocArm32` 中添加新的辅助方法来封装这个特殊指令的发射。

### 4.4. 编写测试用例
*   创建新的C语言测试用例，这些用例会生成涉及新IR特性/指令的IR。
*   检查生成的IR是否符合预期。
*   检查生成的ARM汇编代码是否正确。
*   如果可能，在ARM模拟器或硬件上运行并验证结果。

## 5. 测试策略

*   **单元测试**: 对编译器各模块（如特定IR指令的翻译、`SimpleRegisterAllocator` 的行为）进行单元测试（如果项目结构支持）。
*   **集成测试**:
    *   编写小程序（`.c` 文件）来测试特定语言特性。
    *   编译这些程序，生成IR (`.ir`) 和汇编 (`.s`)。
    *   **IR审查**: 手动或通过脚本检查生成的IR是否符合预期逻辑。
    *   **汇编审查**: 手动或通过脚本检查生成的汇编代码是否正确、高效。
    *   **执行测试**: 将汇编代码编译链接成可执行文件，并在目标环境（模拟器或硬件）上运行，对比程序输出和预期输出。
*   **回归测试**: 维护一个测试套件，在每次代码更改后运行，以确保没有破坏现有功能。

## 6. 重要注意事项与最佳实践

*   **模块化**: 保持前端、IR和后端之间的清晰分离。IR是它们之间明确的接口。
*   **IR的稳定性与通用性**: 设计IR时，力求其既能充分表达源语言特性，又不过于依赖特定源语言或目标机器。
*   **错误处理与日志**:
    *   在编译器各个阶段添加清晰的错误提示。
    *   使用日志系统 (`minic_log`) 记录编译过程中的重要信息和警告，方便调试。
*   **代码注释与文档**: 及时更新代码注释和相关文档（如本文）。
*   **版本控制**: 使用 Git 等版本控制系统进行有效的代码管理。
*   **逐步实现**: 对于复杂功能，采用小步快跑、逐步迭代的方式实现和测试。
*   **调试技巧**:
    *   打印中间结果：在 `IRGenerator` 中打印AST片段，在 `InstSelectorArm32` 中打印当前处理的IR指令和生成的部分汇编。
    *   使用调试器 (gdb) 来跟踪C++代码的执行。
    *   分析生成的 `.ir` 和 `.s` 文件是定位问题的关键。

---

希望这份指南能为您的后续开发工作提供清晰的指引！ 