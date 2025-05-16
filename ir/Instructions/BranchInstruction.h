///
/// @file BranchInstruction.h
/// @brief Conditional Branch Instruction (bc)
/// @author Your Name (youremail@example.com) - Modified by AI
/// @version 1.0
/// @date 2024-12-07 // Or current date
///
/// @copyright Copyright (c) 2024
///
#pragma once

#include "Instruction.h"
#include "LabelInstruction.h" // For LabelInstruction
#include "Value.h"            // For Value

class Function;

///
/// @brief Conditional Branch instruction: bc %cond, true_label, false_label
///
class BranchInstruction : public Instruction {
public:
    /// @brief Constructor
    /// @param _func Owning function
    /// @param _cond The condition value (result of a comparison)
    /// @param _trueLabel Label to jump to if condition is true (non-zero)
    /// @param _falseLabel Label to jump to if condition is false (zero)
    BranchInstruction(Function * _func, Value * _cond, LabelInstruction * _trueLabel, LabelInstruction * _falseLabel);

    /// @brief Destructor
    ~BranchInstruction() override = default;

    /// @brief Convert to string representation
    /// @param str Output string
    void toString(std::string & str) override;

    /// @brief Get the condition value
    /// @return const Value* condition
    const Value * getCondition() const;

    /// @brief Get the true branch label
    /// @return const LabelInstruction* true label
    const LabelInstruction * getTrueLabel() const;

    /// @brief Get the false branch label
    /// @return const LabelInstruction* false label
    const LabelInstruction * getFalseLabel() const;

private:
    // Operands for BranchInstruction:
    // Operand 0: Condition (Value*)
    // Operand 1: True Label (LabelInstruction*) - Note: Labels are also Values if needed, but typically just targets.
    // Operand 2: False Label (LabelInstruction*)
    // We store them directly for type safety and clarity, but also add them as operands for generic Value use if
    // necessary.
};