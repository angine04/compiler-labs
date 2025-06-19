# MiniC 编译器测试指南

本项目提供了两个测试脚本来验证MiniC编译器的前端和后端功能。

## 前端测试 (`test_frontend.py`)

测试前端（词法分析、语法分析）功能，生成IR代码并使用IRCompiler验证运行结果。

### 基本用法

```bash
# 测试所有前端（flex+bison, antlr4, recursive descent）
python3 test_frontend.py

# 只测试特定前端
python3 test_frontend.py --frontends flex_bison antlr4

# 只编译，不运行
python3 test_frontend.py --compile-only

# 测试前10个案例
python3 test_frontend.py --max-tests 10

# 测试特定文件
python3 test_frontend.py --test-files 000_main.c 001_var_defn.c
```

### 参数说明

- `--frontends`: 选择要测试的前端（flex_bison, antlr4, recursive_descent）
- `--compile-only`: 只测试编译，跳过运行时测试
- `--include-for`: 包含for循环测试（144-160）
- `--include-cfg`: 包含控制流图测试（161-162）
- `--max-tests`: 最大测试数量
- `--test-files`: 测试特定文件
- `--timeout`: 每个操作的超时时间（秒）

## 后端测试 (`test_backend.py`)

测试后端（代码生成）功能，生成ARM汇编代码，使用gcc编译为二进制文件，并在qemu上运行。

### 基本用法

```bash
# 测试所有前端的后端代码生成
python3 test_backend.py

# 只测试特定前端
python3 test_backend.py --frontends flex_bison

# 测试前10个案例
python3 test_backend.py --max-tests 10

# 测试特定文件
python3 test_backend.py --test-files 000_main.c 001_var_defn.c
```

### 环境要求

后端测试需要以下工具：

```bash
# 安装ARM交叉编译器和qemu模拟器
sudo apt-get install gcc-arm-linux-gnueabihf qemu-user qemu-user-static
```

### 测试流程

后端测试包含以下步骤：

1. **IR生成**: 将源代码编译为IR
2. **汇编生成**: 将源代码编译为ARM汇编代码
3. **二进制编译**: 使用gcc将汇编代码编译为二进制文件
4. **运行测试**: 在qemu-arm上运行二进制文件
5. **结果比较**: 与参考IRCompiler的结果进行比较

### 参数说明

- `--frontends`: 选择要测试的前端
- `--max-tests`: 最大测试数量
- `--pattern`: 测试文件模式（默认: *.c）
- `--timeout`: 每个操作的超时时间（默认: 30秒）
- `--test-files`: 测试特定文件

## 输出结果

两个脚本都会在`test_results/`目录下创建时间戳子目录，包含：

- `logs/`: 编译和运行日志
- `ir/`: 生成的IR文件
- `asm/`: 生成的汇编文件（仅后端测试）
- `binary/`: 编译的二进制文件（仅后端测试）
- `output/`: 程序输出文件
- `ref_output/`: 参考输出文件

## 测试结果解读

### 前端测试统计

- **Compilation success**: 编译成功率
- **IR generated**: IR生成成功率
- **Runtime success**: 运行成功率
- **Output matches ref**: 输出与参考一致率
- **Exit code matches**: 退出码与参考一致率

### 后端测试统计

- **IR generation**: IR生成成功率
- **ASM generation**: 汇编生成成功率
- **Binary compilation**: 二进制编译成功率
- **Runtime success**: 运行成功率
- **Output matches**: 输出与参考一致率
- **Exit code matches**: 退出码与参考一致率
- **FULL SUCCESS**: 全部步骤成功率

## 示例输出

### 前端测试示例

```
FLEX_BISON:
  Compilation success: 118/144 (81.9%)
  IR generated:        118/144 (81.9%)
  Runtime success:     118/144 (81.9%)
  Output matches ref:  118/144 (81.9%)
  Exit code matches:   118/144 (81.9%)
```

### 后端测试示例

```
FLEX_BISON:
  IR generation:      143/143 (100.0%)
  ASM generation:     143/143 (100.0%)
  Binary compilation: 143/143 (100.0%)
  Runtime success:    143/143 (100.0%)
  Output matches:     143/143 (100.0%)
  Exit code matches:  143/143 (100.0%)
  FULL SUCCESS:       143/143 (100.0%)
```

## 故障排除

### 常见错误

1. **minic not found**: 运行`cd build && make`编译项目
2. **IRCompiler not found**: 确保IRCompiler位于正确路径
3. **qemu-arm not found**: 安装qemu-user包
4. **arm-linux-gnueabihf-gcc not found**: 安装交叉编译器

### 调试建议

- 查看`logs/`目录中的详细日志
- 使用`--max-tests 1`测试单个案例
- 使用`--test-files`测试特定失败的文件
- 检查生成的IR和汇编文件是否正确
