///
/// @file InstSelectorArm32.cpp
/// @brief 指令选择器-ARM32的实现
/// @author zenglj (zenglj@live.com)
/// @version 1.0
/// @date 2024-11-21
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// </table>
///
#include <cstdio>
#include <cassert>

#include "Common.h"
#include "ILocArm32.h"
#include "InstSelectorArm32.h"
#include "PlatformArm32.h"

#include "PointerType.h"
#include "RegVariable.h"
#include "Function.h"

#include "LabelInstruction.h"
#include "GotoInstruction.h"
#include "FuncCallInstruction.h"
#include "MoveInstruction.h"
#include "BinaryInstruction.h"
#include "BranchInstruction.h"

/// @brief 构造函数
/// @param _irCode 指令
/// @param _iloc ILoc
/// @param _func 函数
InstSelectorArm32::InstSelectorArm32(vector<Instruction *> & _irCode,
                                     ILocArm32 & _iloc,
                                     Function * _func,
                                     SimpleRegisterAllocator & allocator)
    : ir(_irCode), iloc(_iloc), func(_func), simpleRegisterAllocator(allocator)
{
    translator_handlers[IRInstOperator::IRINST_OP_ENTRY] = &InstSelectorArm32::translate_entry;
    translator_handlers[IRInstOperator::IRINST_OP_EXIT] = &InstSelectorArm32::translate_exit;

    translator_handlers[IRInstOperator::IRINST_OP_LABEL] = &InstSelectorArm32::translate_label;
    translator_handlers[IRInstOperator::IRINST_OP_GOTO] = &InstSelectorArm32::translate_goto;

    translator_handlers[IRInstOperator::IRINST_OP_ASSIGN] = &InstSelectorArm32::translate_assign;

    translator_handlers[IRInstOperator::IRINST_OP_ADD_I] = &InstSelectorArm32::translate_add_int32;
    translator_handlers[IRInstOperator::IRINST_OP_SUB_I] = &InstSelectorArm32::translate_sub_int32;
    translator_handlers[IRInstOperator::IRINST_OP_MUL_I] = &InstSelectorArm32::translate_mul_int32;
    translator_handlers[IRInstOperator::IRINST_OP_DIV_I] = &InstSelectorArm32::translate_div_int32;
    translator_handlers[IRInstOperator::IRINST_OP_REM_I] = &InstSelectorArm32::translate_rem_int32;

    // Register handlers for comparison operators
    translator_handlers[IRInstOperator::IRINST_OP_CMP_EQ_I] = &InstSelectorArm32::translate_comparison;
    translator_handlers[IRInstOperator::IRINST_OP_CMP_NE_I] = &InstSelectorArm32::translate_comparison;
    translator_handlers[IRInstOperator::IRINST_OP_CMP_LT_I] = &InstSelectorArm32::translate_comparison;
    translator_handlers[IRInstOperator::IRINST_OP_CMP_LE_I] = &InstSelectorArm32::translate_comparison;
    translator_handlers[IRInstOperator::IRINST_OP_CMP_GT_I] = &InstSelectorArm32::translate_comparison;
    translator_handlers[IRInstOperator::IRINST_OP_CMP_GE_I] = &InstSelectorArm32::translate_comparison;

    // Register handler for conditional branch operator
    translator_handlers[IRInstOperator::IRINST_OP_BR_COND] = &InstSelectorArm32::translate_branch_conditional;

    translator_handlers[IRInstOperator::IRINST_OP_FUNC_CALL] = &InstSelectorArm32::translate_call;
    translator_handlers[IRInstOperator::IRINST_OP_ARG] = &InstSelectorArm32::translate_arg;
}

///
/// @brief 析构函数
///
InstSelectorArm32::~InstSelectorArm32()
{}

/// @brief 指令选择执行
void InstSelectorArm32::run()
{
    for (auto inst: ir) {

        // 逐个指令进行翻译
        if (!inst->isDead()) {
            translate(inst);
        }
    }
}

/// @brief 指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate(Instruction * inst)
{
    // 操作符
    IRInstOperator op = inst->getOp();

    map<IRInstOperator, translate_handler>::const_iterator pIter;
    pIter = translator_handlers.find(op);
    if (pIter == translator_handlers.end()) {
        // 没有找到，则说明当前不支持
        printf("Translate: Operator(%d) not support", (int) op);
        return;
    }

    // 开启时输出IR指令作为注释
    if (showLinearIR) {
        outputIRInstruction(inst);
    }

    (this->*(pIter->second))(inst);
}

///
/// @brief 输出IR指令
///
void InstSelectorArm32::outputIRInstruction(Instruction * inst)
{
    std::string irStr;
    inst->toString(irStr);
    if (!irStr.empty()) {
        iloc.comment(irStr);
    }
}

/// @brief NOP翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_nop(Instruction * inst)
{
    (void) inst;
    iloc.nop();
}

/// @brief Label指令指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_label(Instruction * inst)
{
    Instanceof(labelInst, LabelInstruction *, inst);

    iloc.label(labelInst->getName());
}

/// @brief goto指令指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_goto(Instruction * inst)
{
    Instanceof(gotoInst, GotoInstruction *, inst);

    // 无条件跳转
    iloc.jump(gotoInst->getTarget()->getName());
}

/// @brief 函数入口指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_entry(Instruction * inst)
{
    // 查看保护的寄存器
    auto & protectedRegNo = func->getProtectedReg();
    auto & protectedRegStr = func->getProtectedRegStr();

    bool first = true;
    for (auto regno: protectedRegNo) {
        if (first) {
            protectedRegStr = PlatformArm32::regName[regno];
            first = false;
        } else {
            protectedRegStr += "," + PlatformArm32::regName[regno];
        }
    }

    if (!protectedRegStr.empty()) {
        iloc.inst("push", "{" + protectedRegStr + "}");
    }

    // 为fun分配栈帧，含局部变量、函数调用值传递的空间等
    iloc.allocStack(func, ARM32_TMP_REG_NO);
}

/// @brief 函数出口指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_exit(Instruction * inst)
{
    if (inst->getOperandsNum()) {
        // 存在返回值
        Value * retVal = inst->getOperand(0);

        // 赋值给寄存器R0
        iloc.load_var(0, retVal);
    }

    // 恢复栈空间
    iloc.inst("mov", "sp", "fp");

    // 保护寄存器的恢复
    auto & protectedRegStr = func->getProtectedRegStr();
    if (!protectedRegStr.empty()) {
        iloc.inst("pop", "{" + protectedRegStr + "}");
    }

    iloc.inst("bx", "lr");
}

/// @brief 赋值指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_assign(Instruction * inst)
{
    Value * result = inst->getOperand(0);
    Value * arg1 = inst->getOperand(1);

    int32_t arg1_regId = arg1->getRegId();
    int32_t result_regId = result->getRegId();

    // 检查是否是指针赋值（数组元素赋值）
    if (result->getType()->isPointerType() && !arg1->getType()->isPointerType()) {
        // 这是 *ptr = value 的情况（数组元素赋值）
        // result是地址，arg1是要存储的值
        
        int32_t addr_regId = result->getRegId();
        int32_t value_regId = arg1->getRegId();
        
        // 获取地址到寄存器
        int32_t addr_reg;
        if (addr_regId != -1) {
            addr_reg = addr_regId;
        } else {
            addr_reg = simpleRegisterAllocator.Allocate(result);
            iloc.load_var(addr_reg, result);
        }
        
        // 获取值到寄存器
        int32_t value_reg;
        if (value_regId != -1) {
            value_reg = value_regId;
        } else {
            value_reg = simpleRegisterAllocator.Allocate(arg1);
            iloc.load_var(value_reg, arg1);
        }
        
        // 存储值到地址：str value_reg, [addr_reg]
        iloc.inst("str", PlatformArm32::regName[value_reg], "[" + PlatformArm32::regName[addr_reg] + "]");
        
        // 释放临时分配的寄存器
        if (addr_regId == -1) {
            simpleRegisterAllocator.free(result);
        }
        if (value_regId == -1) {
            simpleRegisterAllocator.free(arg1);
        }
        
    } else if (!result->getType()->isPointerType() && arg1->getType()->isPointerType() && arg1->getRegId() == -1) {
        // 这是 value = *ptr 的情况（数组元素读取）
        // 但是需要确保arg1不是寄存器变量，否则它是指针变量赋值而不是指针解引用
        // arg1是地址，result是目标变量
        
        int32_t addr_regId = arg1->getRegId();
        int32_t result_regId = result->getRegId();
        
        // 获取地址到寄存器
        int32_t addr_reg;
        if (addr_regId != -1) {
            addr_reg = addr_regId;
        } else {
            addr_reg = simpleRegisterAllocator.Allocate(arg1);
            iloc.load_var(addr_reg, arg1);
        }
        
        // 从地址加载值到寄存器
        int32_t load_reg;
        if (result_regId != -1) {
            load_reg = result_regId;
        } else {
            load_reg = simpleRegisterAllocator.Allocate(result);
        }
        
        // 从内存加载值：ldr load_reg, [addr_reg]
        iloc.inst("ldr", PlatformArm32::regName[load_reg], "[" + PlatformArm32::regName[addr_reg] + "]");
        
        // 如果result不是寄存器变量，需要存储到内存
        if (result_regId == -1) {
            // 如果result不是寄存器变量，需要存储到内存
            // 按照正常流程，所有临时变量都应该在stackAlloc阶段分配了内存地址
            iloc.store_var(load_reg, result, ARM32_TMP_REG_NO);
            simpleRegisterAllocator.free(result);
        }
        
        // 释放临时分配的寄存器
        if (addr_regId == -1) {
            simpleRegisterAllocator.free(arg1);
        }
        
    } else {
        // 普通赋值情况，包括指针类型变量到整型寄存器的赋值（函数参数传递）
        if (arg1_regId != -1) {
            // 寄存器 => 内存
            // 寄存器 => 寄存器

            // r8 -> rs 可能用到r9
            iloc.store_var(arg1_regId, result, ARM32_TMP_REG_NO);
        } else if (result_regId != -1) {
            // 内存变量 => 寄存器

            iloc.load_var(result_regId, arg1);
        } else {
            // 内存变量 => 内存变量

            int32_t temp_regno = simpleRegisterAllocator.Allocate();

            // arg1 -> r8
            iloc.load_var(temp_regno, arg1);

            // r8 -> rs 可能用到r9
            iloc.store_var(temp_regno, result, ARM32_TMP_REG_NO);

            simpleRegisterAllocator.free(temp_regno);
        }
    }
}

/// @brief 二元操作指令翻译成ARM32汇编
/// @param inst IR指令
/// @param operator_name 操作码
/// @param rs_reg_no 结果寄存器号
/// @param op1_reg_no 源操作数1寄存器号
/// @param op2_reg_no 源操作数2寄存器号
void InstSelectorArm32::translate_two_operator(Instruction * inst, string operator_name)
{
    Value * result = inst;
    Value * arg1 = inst->getOperand(0);
    Value * arg2 = inst->getOperand(1);

    int32_t arg1_reg_no = arg1->getRegId();
    int32_t arg2_reg_no = arg2->getRegId();
    int32_t result_reg_no = inst->getRegId();
    int32_t load_result_reg_no, load_arg1_reg_no, load_arg2_reg_no;

    // 看arg1是否是寄存器，若是则寄存器寻址，否则要load变量到寄存器中
    if (arg1_reg_no == -1) {

        // 分配一个寄存器r8
        load_arg1_reg_no = simpleRegisterAllocator.Allocate(arg1);

        // arg1 -> r8，这里可能由于偏移不满足指令的要求，需要额外分配寄存器
        iloc.load_var(load_arg1_reg_no, arg1);
    } else {
        load_arg1_reg_no = arg1_reg_no;
    }

    // 看arg2是否是寄存器，若是则寄存器寻址，否则要load变量到寄存器中
    if (arg2_reg_no == -1) {

        // 分配一个寄存器r9
        load_arg2_reg_no = simpleRegisterAllocator.Allocate(arg2);

        // arg2 -> r9
        iloc.load_var(load_arg2_reg_no, arg2);
    } else {
        load_arg2_reg_no = arg2_reg_no;
    }

    // 看结果变量是否是寄存器，若不是则需要分配一个新的寄存器来保存运算的结果
    if (result_reg_no == -1) {
        // 分配一个寄存器r10，用于暂存结果
        load_result_reg_no = simpleRegisterAllocator.Allocate(result);
    } else {
        load_result_reg_no = result_reg_no;
    }

    // r8 + r9 -> r10
    iloc.inst(operator_name,
              PlatformArm32::regName[load_result_reg_no],
              PlatformArm32::regName[load_arg1_reg_no],
              PlatformArm32::regName[load_arg2_reg_no]);

    // 结果不是寄存器，则需要把rs_reg_name保存到结果变量中
    if (result_reg_no == -1) {

        // 这里使用预留的临时寄存器，因为立即数可能过大，必须借助寄存器才可操作。

        // r10 -> result
        iloc.store_var(load_result_reg_no, result, ARM32_TMP_REG_NO);
    }

    // 释放寄存器
    simpleRegisterAllocator.free(arg1);
    simpleRegisterAllocator.free(arg2);
    simpleRegisterAllocator.free(result);
}

/// @brief 整数加法指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_add_int32(Instruction * inst)
{
    translate_two_operator(inst, "add");
}

/// @brief 整数减法指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_sub_int32(Instruction * inst)
{
    translate_two_operator(inst, "sub");
}

/// @brief 整数乘法指令翻译成ARM32汇编 (新实现)
void InstSelectorArm32::translate_mul_int32(Instruction * inst)
{
    translate_two_operator(inst, "mul");
}

/// @brief 整数有符号除法指令翻译成ARM32汇编 (新实现)
void InstSelectorArm32::translate_div_int32(Instruction * inst)
{
    translate_two_operator(inst, "sdiv");
}

/// @brief 整数有符号求余指令翻译成ARM32汇编 (新实现)
void InstSelectorArm32::translate_rem_int32(Instruction * inst)
{
    Value * result_val = inst;
    Value * arg1_val = inst->getOperand(0); // Dividend (a)
    Value * arg2_val = inst->getOperand(1); // Divisor (b)

    int32_t arg1_reg_no = arg1_val->getRegId();
    int32_t arg2_reg_no = arg2_val->getRegId();
    int32_t result_reg_no = result_val->getRegId();

    int32_t load_arg1_reg_no, load_arg2_reg_no, load_result_reg_no;
    int32_t temp_reg_for_div_result; // 临时寄存器存储 a/b 的结果

    // 加载 arg1 (被除数 a) 到寄存器
    if (arg1_reg_no == -1) {
        load_arg1_reg_no = simpleRegisterAllocator.Allocate(arg1_val);
        iloc.load_var(load_arg1_reg_no, arg1_val);
    } else {
        load_arg1_reg_no = arg1_reg_no;
    }

    // 加载 arg2 (除数 b) 到寄存器
    if (arg2_reg_no == -1) {
        load_arg2_reg_no = simpleRegisterAllocator.Allocate(arg2_val);
        iloc.load_var(load_arg2_reg_no, arg2_val);
    } else {
        load_arg2_reg_no = arg2_reg_no;
    }

    // 为 a/b 的中间结果分配临时寄存器
    temp_reg_for_div_result = simpleRegisterAllocator.Allocate();

    // 计算 a / b (结果在 temp_reg_for_div_result)
    iloc.inst("sdiv",
              PlatformArm32::regName[temp_reg_for_div_result],
              PlatformArm32::regName[load_arg1_reg_no],
              PlatformArm32::regName[load_arg2_reg_no]);

    // 如果结果最终需要存入内存，或者指定的寄存器与临时寄存器不同，准备最终结果寄存器
    if (result_reg_no == -1) {
        load_result_reg_no = simpleRegisterAllocator.Allocate(result_val);
    } else {
        load_result_reg_no = result_reg_no;
    }

    // 计算 (a/b) * b，结果仍然可以放在 temp_reg_for_div_result (如果它不与 load_result_reg_no 冲突)
    // 或者更安全，使用另一个临时寄存器如果担心冲突，但这里我们直接覆盖 temp_reg_for_div_result
    iloc.inst("mul",
              PlatformArm32::regName[temp_reg_for_div_result], // Rd: (a/b)*b
              PlatformArm32::regName[temp_reg_for_div_result], // Rn: a/b
              PlatformArm32::regName[load_arg2_reg_no]);       // Rm: b

    // 计算 a - ((a/b)*b) (结果在 load_result_reg_no)
    iloc.inst("sub",
              PlatformArm32::regName[load_result_reg_no],       // Rd: final result
              PlatformArm32::regName[load_arg1_reg_no],         // Rn: a
              PlatformArm32::regName[temp_reg_for_div_result]); // Rm: (a/b)*b

    // 如果结果最初不在寄存器中 (result_reg_no == -1)，则需要将 load_result_reg_no 中的值存储回内存
    if (result_reg_no == -1) {
        iloc.store_var(load_result_reg_no, result_val, ARM32_TMP_REG_NO);
    }

    // 释放为 a/b 分配的临时寄存器
    simpleRegisterAllocator.free(temp_reg_for_div_result);

    // 释放操作数和结果（如果它们是临时分配的）
    if (arg1_reg_no == -1)
        simpleRegisterAllocator.free(arg1_val);
    if (arg2_reg_no == -1)
        simpleRegisterAllocator.free(arg2_val);
    if (result_reg_no == -1 && load_result_reg_no != result_reg_no) { // 如果为结果分配了新的寄存器
        simpleRegisterAllocator.free(result_val); // 释放与result_val关联的寄存器（现在是load_result_reg_no）
    }
}

/// @brief 函数调用指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_call(Instruction * inst)
{
    FuncCallInstruction * callInst = dynamic_cast<FuncCallInstruction *>(inst);

    int32_t operandNum = callInst->getOperandsNum();

    if (operandNum != realArgCount) {

        // 两者不一致 也可能没有ARG指令，正常
        if (realArgCount != 0) {

            minic_log(LOG_ERROR, "ARG指令的个数与调用函数个数不一致");
        }
    }

    if (operandNum) {

        // 强制占用这几个寄存器参数传递的寄存器
        simpleRegisterAllocator.Allocate(0);
        simpleRegisterAllocator.Allocate(1);
        simpleRegisterAllocator.Allocate(2);
        simpleRegisterAllocator.Allocate(3);

        // 前四个的后面参数采用栈传递
        int esp = 0;
        for (int32_t k = 4; k < operandNum; k++) {

            auto arg = callInst->getOperand(k);

            // 新建一个内存变量，用于栈传值到形参变量中
            // 不要使用指针类型，使用普通类型来避免触发指针赋值逻辑
            MemVariable * newVal = func->newMemVariable(arg->getType());
            newVal->setMemoryAddr(ARM32_SP_REG_NO, esp);
            esp += 4;

            Instruction * assignInst = new MoveInstruction(func, newVal, arg);

            // 翻译赋值指令
            translate_assign(assignInst);

            delete assignInst;
        }

        for (int32_t k = 0; k < operandNum && k < 4; k++) {

            auto arg = callInst->getOperand(k);

            // 检查实参的类型是否是临时变量。
            // 如果是临时变量，该变量可更改为寄存器变量即可，或者设置寄存器号
            // 如果不是，则必须开辟一个寄存器变量，然后赋值即可

            Instruction * assignInst = new MoveInstruction(func, PlatformArm32::intRegVal[k], arg);

            // 翻译赋值指令
            translate_assign(assignInst);

            delete assignInst;
        }
    }

    iloc.call_fun(callInst->getName());

    if (operandNum) {
        simpleRegisterAllocator.free(0);
        simpleRegisterAllocator.free(1);
        simpleRegisterAllocator.free(2);
        simpleRegisterAllocator.free(3);
    }

    // 赋值指令
    if (callInst->hasResultValue()) {

        // 新建一个赋值操作
        Instruction * assignInst = new MoveInstruction(func, callInst, PlatformArm32::intRegVal[0]);

        // 翻译赋值指令
        translate_assign(assignInst);

        delete assignInst;
    }

    // 函数调用后清零，使得下次可正常统计
    realArgCount = 0;
}

///
/// @brief 实参指令翻译成ARM32汇编
/// @param inst
///
void InstSelectorArm32::translate_arg(Instruction * inst)
{
    // 翻译之前必须确保源操作数要么是寄存器，要么是内存，否则出错。
    Value * src = inst->getOperand(0);

    // 当前统计的ARG指令个数
    int32_t regId = src->getRegId();

    if (realArgCount < 4) {
        // 前四个参数
        if (regId != -1) {
            if (regId != realArgCount) {
                // 肯定寄存器分配有误
                minic_log(LOG_ERROR, "第%d个ARG指令对象寄存器分配有误: %d", argCount + 1, regId);
            }
        } else {
            minic_log(LOG_ERROR, "第%d个ARG指令对象不是寄存器", argCount + 1);
        }
    } else {
        // 必须是内存分配，若不是则出错
        int32_t baseRegId;
        bool result = src->getMemoryAddr(&baseRegId);
        if ((!result) || (baseRegId != ARM32_SP_REG_NO)) {

            minic_log(LOG_ERROR, "第%d个ARG指令对象不是SP寄存器寻址", argCount + 1);
        }
    }

    realArgCount++;
}

/// @brief 比较指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_comparison(Instruction * inst)
{
    assert(dynamic_cast<BinaryInstruction *>(inst) && "translate_comparison expects a BinaryInstruction");

    Value * src1 = inst->getOperand(0);
    Value * src2 = inst->getOperand(1);

    // This logic is similar to the beginning of translate_two_operator
    // to ensure operands are in registers.

    int32_t r_s1 = -1, r_s2 = -1;

    // Get register for src1
    if (src1->getRegId() != -1) {
        r_s1 = src1->getRegId();
    } else {
        r_s1 = simpleRegisterAllocator.Allocate(src1); // Allocate register for src1 if not already in one
        iloc.load_var(r_s1, src1);                     // Load src1 into the allocated register
    }

    // Get register for src2
    // Note: ARM 'cmp' can take an immediate as the second operand, but for simplicity
    // and consistency with translate_two_operator, we load both into registers.
    // A future optimization could be to check if src2 is a ConstInt and use an immediate form of CMP if applicable.
    if (src2->getRegId() != -1) {
        r_s2 = src2->getRegId();
    } else {
        r_s2 = simpleRegisterAllocator.Allocate(src2); // Allocate register for src2
        iloc.load_var(r_s2, src2);                     // Load src2 into the allocated register
    }

    // Emit CMP instruction: cmp r_s1, r_s2
    // The result (target register of the IR BinaryInstruction) is not explicitly used here for CMP,
    // as CMP only sets flags. The IR instruction itself (%tX = icmp ...) still exists and has a name,
    // and SimpleRegisterAllocator might have associated a conceptual regId with it, but we don't mov a 0/1 into it
    // here.
    iloc.inst("cmp", "", PlatformArm32::regName[r_s1], PlatformArm32::regName[r_s2]);

    // 获取比较操作的类型，确定条件后缀
    BinaryInstruction * cmp_inst = static_cast<BinaryInstruction *>(inst);
    IRInstOperator comparison_op = cmp_inst->getOp();
    
    std::string cond_suffix;
    switch (comparison_op) {
        case IRInstOperator::IRINST_OP_CMP_EQ_I:
            cond_suffix = "eq";
            break;
        case IRInstOperator::IRINST_OP_CMP_NE_I:
            cond_suffix = "ne";
            break;
        case IRInstOperator::IRINST_OP_CMP_LT_I:
            cond_suffix = "lt";
            break;
        case IRInstOperator::IRINST_OP_CMP_LE_I:
            cond_suffix = "le";
            break;
        case IRInstOperator::IRINST_OP_CMP_GT_I:
            cond_suffix = "gt";
            break;
        case IRInstOperator::IRINST_OP_CMP_GE_I:
            cond_suffix = "ge";
            break;
        default:
            fprintf(stderr, "[FATAL_ERROR] translate_comparison: Unexpected comparison operator %d.\n", (int) comparison_op);
            assert(false);
            return;
    }

    // 为比较结果分配寄存器或获取已分配的寄存器
    int32_t result_reg = -1;
    if (inst->getRegId() != -1) {
        result_reg = inst->getRegId();
    } else {
        result_reg = simpleRegisterAllocator.Allocate(inst);
    }

    // 根据条件标志位设置结果寄存器的值
    // mov<cond> result_reg, #1    ; 如果条件成立，设置为1
    // mov<not_cond> result_reg, #0 ; 如果条件不成立，设置为0
    
    // 先设置为0
    iloc.inst("movw", PlatformArm32::regName[result_reg], "#0");
    
    // 如果条件成立，设置为1
    std::string conditional_mov = "mov" + cond_suffix;
    iloc.inst(conditional_mov, PlatformArm32::regName[result_reg], "#1");

    // 如果结果变量不在寄存器中，需要存储到内存
    if (inst->getRegId() == -1) {
        iloc.store_var(result_reg, inst, ARM32_TMP_REG_NO);
        simpleRegisterAllocator.free(inst);
    }

    // Register deallocation would depend on the strategy of SimpleRegisterAllocator.
    // If Allocate(Value*) reserves it for the Value's lifetime or a broader scope, no free here.
    // If registers were allocated as temporary scratch registers just for this op, they might be freed.
    // For now, assuming SimpleRegisterAllocator handles this or they are freed later.
    if (src1->getRegId() == -1) {
        simpleRegisterAllocator.free(src1);
    }
    if (src2->getRegId() == -1) {
        simpleRegisterAllocator.free(src2);
    }
}

/// @brief 条件分支指令翻译成ARM32汇编
/// @param inst IR指令
void InstSelectorArm32::translate_branch_conditional(Instruction * inst)
{
    assert(dynamic_cast<BranchInstruction *>(inst) && "translate_branch_conditional expects a BranchInstruction");
    BranchInstruction * branch_inst = static_cast<BranchInstruction *>(inst);

    Value * cond_value = branch_inst->getOperand(0); // This is the BinaryInstruction (the CMP_OP)
    LabelInstruction * true_label = static_cast<LabelInstruction *>(branch_inst->getOperand(1));
    LabelInstruction * false_label = static_cast<LabelInstruction *>(branch_inst->getOperand(2));

    // The condition value must be the result of a comparison instruction (a BinaryInstruction with a CMP_OP)
    assert(dynamic_cast<BinaryInstruction *>(cond_value) &&
           "Condition for BranchInstruction should be a BinaryInstruction (comparison)");
    BinaryInstruction * cmp_op_inst = static_cast<BinaryInstruction *>(cond_value);
    IRInstOperator comparison_op = cmp_op_inst->getOp(); // Get the original comparison type, e.g., IRINST_OP_CMP_EQ_I

    std::string cond_suffix;
    switch (comparison_op) {
        case IRInstOperator::IRINST_OP_CMP_EQ_I:
            cond_suffix = "eq";
            break;
        case IRInstOperator::IRINST_OP_CMP_NE_I:
            cond_suffix = "ne";
            break;
        case IRInstOperator::IRINST_OP_CMP_LT_I:
            cond_suffix = "lt";
            break; // Signed less than
        case IRInstOperator::IRINST_OP_CMP_LE_I:
            cond_suffix = "le";
            break; // Signed less than or equal
        case IRInstOperator::IRINST_OP_CMP_GT_I:
            cond_suffix = "gt";
            break; // Signed greater than
        case IRInstOperator::IRINST_OP_CMP_GE_I:
            cond_suffix = "ge";
            break; // Signed greater than or equal
        default:
            fprintf(
                stderr,
                "[FATAL_ERROR] translate_branch_conditional: Unexpected comparison operator %d for branch condition.\n",
                (int) comparison_op);
            fflush(stderr);
            assert(false && "Unknown comparison op for conditional branch");
            return; // Should not happen
    }

    // Emit: B<cond> true_label_name
    // Construct the conditional branch opcode, e.g., "beq", "bne"
    std::string conditional_branch_opcode = "b" + cond_suffix;
    iloc.inst(conditional_branch_opcode, true_label->getName());

    // Emit: B false_label_name (unconditional branch)
    iloc.jump(false_label->getName()); // jump() should emit an unconditional "b label"
}
