///
/// @file BranchInstruction.cpp
/// @brief Conditional Branch Instruction (bc)
/// @author Your Name (youremail@example.com) - Modified by AI
/// @version 1.0
/// @date 2024-12-07 // Or current date
///
/// @copyright Copyright (c) 2024
///
#include <cstdio>  // For printf
#include <cassert> // For assert
#include "BranchInstruction.h"
#include "Function.h" // Required for Function parameter in constructor

BranchInstruction::BranchInstruction(Function * _func,
                                     Value * _cond,
                                     LabelInstruction * _trueLabel,
                                     LabelInstruction * _falseLabel)
    : Instruction(_func, IRInstOperator::IRINST_OP_BR_COND, nullptr) // Result type is nullptr for branch
{
    // We expect _falseLabel to be non-null because ir_if_statement logic ensures
    // it passes endif_label if there's no actual else block.
    printf("[DEBUG] BranchInstruction CTOR: func=%p, cond=%p (name: %s), trueLabel=%p (name: %s), falseLabel=%p (name: "
           "%s)\n",
           (void *) _func,
           (void *) _cond,
           _cond ? _cond->getIRName().c_str() : "null_cond",
           (void *) _trueLabel,
           _trueLabel ? _trueLabel->getIRName().c_str() : "null_true_label",
           (void *) _falseLabel,
           _falseLabel ? _falseLabel->getIRName().c_str() : "null_false_label");
    fflush(stdout);

    assert(_func != nullptr && "Function pointer cannot be null in BranchInstruction CTOR");
    assert(_cond != nullptr && "Condition value cannot be null in BranchInstruction CTOR");
    assert(_trueLabel != nullptr && "True label cannot be null in BranchInstruction CTOR");
    assert(_falseLabel != nullptr &&
           "False label (or endif_label for if-then) cannot be null in BranchInstruction CTOR");

    addOperand(_cond);       // Operand 0: Condition
    addOperand(_trueLabel);  // Operand 1: True Label
    addOperand(_falseLabel); // Operand 2: False Label
}

void BranchInstruction::toString(std::string & str)
{
    Value * cond = getOperand(0);
    LabelInstruction * trueLabel = static_cast<LabelInstruction *>(getOperand(1));
    LabelInstruction * falseLabel = static_cast<LabelInstruction *>(getOperand(2));

    if (!cond || !trueLabel || !falseLabel) {
        str = "Error: Invalid BranchInstruction operands";
        return;
    }

    str = "bc " + cond->getIRName() + ", label " + trueLabel->getIRName() + ", label " + falseLabel->getIRName();
}

const Value * BranchInstruction::getCondition() const
{
    if (getOperands().size() > 0) {
        return getOperand(0);
    }
    return nullptr;
}

const LabelInstruction * BranchInstruction::getTrueLabel() const
{
    if (getOperands().size() > 1) {
        return static_cast<const LabelInstruction *>(getOperand(1));
    }
    return nullptr;
}

const LabelInstruction * BranchInstruction::getFalseLabel() const
{
    if (getOperands().size() > 2) {
        return static_cast<const LabelInstruction *>(getOperand(2));
    }
    return nullptr;
}