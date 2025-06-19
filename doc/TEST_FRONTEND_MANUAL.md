# Frontend Testing Script Manual

## Overview

The `test_frontend.py` script is a comprehensive testing framework for the minic compiler that supports three different frontends:
- **flex_bison** (默认/推荐): Lexer + Parser 前端，最稳定可靠
- **antlr4**: ANTLR4 语法分析前端
- **recursive_descent**: 递归下降解析前端

## Quick Start

```bash
# 测试所有基础测试案例 (1-143) 使用 flex_bison 前端
python3 test_frontend.py --frontends flex_bison
```

## Command Line Arguments

### Frontend Selection
```bash
--frontends {flex_bison,antlr4,recursive_descent}
```
- 可以指定多个前端: `--frontends flex_bison recursive_descent`
- **推荐使用 flex_bison** (最稳定，所有测试通过)

### Test Categories  
```bash
--include-for     # 包含 for-loop 测试 (144-160)
--include-cfg     # 包含 CFG 测试 (161-162)
```
- 默认只测试基础功能 (1-143)
- 可以组合使用: `--include-for --include-cfg`

### Test Filtering
```bash
--pattern PATTERN          # 文件名模式匹配 (例: "*json*", "*001*")
--max-tests N              # 限制测试数量
--max-test-number N        # 限制最大测试编号
--test-files FILE1 FILE2   # 指定具体测试文件
```

### Execution Control
```bash
--compile-only    # 仅编译测试，不运行IR
--timeout N       # 设置超时时间 (默认10秒)
--verbose         # 详细输出
```

## Usage Examples

### 1. 完整回归测试
```bash
# 测试所有前端，包含所有类型的测试
python3 test_frontend.py --frontends flex_bison antlr4 recursive_descent --include-for --include-cfg
```

### 2. 快速验证
```bash
# 仅编译前10个测试，验证语法
python3 test_frontend.py --frontends flex_bison --compile-only --max-tests 10
```

### 3. 调试特定问题
```bash
# 测试特定文件模式
python3 test_frontend.py --frontends flex_bison --pattern "*array*" --verbose

# 测试特定编号范围
python3 test_frontend.py --frontends flex_bison --max-test-number 50
```

### 4. 性能测试
```bash
# 测试复杂案例的编译性能
python3 test_frontend.py --frontends flex_bison --pattern "*132_json.c" --timeout 30
```

## Understanding Output

### 编译阶段
```
[1/5] Testing 132_json...
  Compiling with flex+bison... OK (0.01s)
```
- 显示编译时间
- OK = 编译成功，FAILED = 编译失败

### 运行阶段
```
    Running without input... OK (0.02s)
    Running with input file... OK (0.05s) 
```
- 自动检测是否需要输入文件 (.in)
- 显示运行时间和状态

### 最终统计
```
FLEX_BISON:
  Compilation success: 142/145 (97.9%)
  IR generated:        142/145 (97.9%)
  Runtime success:     113/142 (79.6%)
```

## File Structure

测试运行后会在 `test_results/run_YYYYMMDD_HHMMSS/` 创建以下目录：

```
test_results/run_20250619_001234/
├── logs/           # 编译和运行日志
│   ├── test_flex_bison_compile.log
│   └── test_flex_bison_runtime.log
├── ir/             # 生成的IR文件  
│   └── test_flex_bison.ir
└── output/         # 程序运行输出
    └── test_flex_bison.out
```

## Test Categories

### 基础测试 (1-143) - 默认包含
- 变量声明和初始化
- 函数定义和调用  
- 表达式计算
- 数组操作
- 条件语句和循环

### For-loop 测试 (144-160) - 可选
```bash
--include-for
```
- 各种for循环结构
- 嵌套循环
- 循环控制语句

### CFG 测试 (161-162) - 可选  
```bash
--include-cfg
```
- 控制流图相关测试

## Troubleshooting

### 1. 超时问题
如果测试卡住：
```bash
# 增加超时时间
python3 test_frontend.py --timeout 30

# 或仅测试编译
python3 test_frontend.py --compile-only
```

### 2. 内存问题
对于大型测试：
```bash
# 限制并发测试数量
python3 test_frontend.py --max-tests 10
```

### 3. 调试编译错误
```bash
# 查看编译日志
cat test_results/run_*/logs/*_compile.log

# 使用详细模式
python3 test_frontend.py --verbose
```

### 4. 前端特定问题
- **flex_bison**: 最稳定，推荐使用
- **recursive_descent**: 可能有语法解析错误
- **antlr4**: 需要确保ANTLR4正确安装

## Performance Tips

1. **使用 flex_bison 前端**：最快最稳定
2. **仅编译测试**：使用 `--compile-only` 跳过运行阶段
3. **限制测试数量**：使用 `--max-tests` 进行快速验证
4. **并行测试**：脚本已优化，自动并发处理

## Common Patterns

```bash
# 日常开发验证
python3 test_frontend.py --frontends flex_bison --max-tests 20

# 提交前完整测试
python3 test_frontend.py --frontends flex_bison --include-for

# 调试特定功能
python3 test_frontend.py --frontends flex_bison --pattern "*func*" --verbose

# 性能基准测试  
python3 test_frontend.py --frontends flex_bison --compile-only
```

## Notes

- 默认超时10秒，复杂测试可能需要增加
- 所有输出文件都有时间戳，不会覆盖
- 日志文件包含详细的编译和运行信息
- 测试会自动跳过非测试文件 (std.c, std.h等)
- 支持输入文件自动检测 (.in 文件)

---

**推荐配置**: `python3 test_frontend.py --frontends flex_bison --include-for` 