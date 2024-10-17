#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "ir/instruction.h"

namespace compiler {

class ArithmeticInsn : public Instruction {
public:
    ArithmeticInsn(Opcode opcode, ValueType resultType, Value input1, Value input2, Value output):
        Instruction(opcode, resultType)
    {
        AddInput(input1);
        AddInput(input2);
        // AddUser()
    }
};

class AddInstruction final : public ArithmeticInsn {
public:
    AddInstruction(ValueType resultType, Value input1, Value input2, Value output):
        ArithmeticInsn(Opcode::ADD, resultType, input1, input2, output)
    {}
};

class SubInstruction final : public ArithmeticInsn {
public:
    SubInstruction(ValueType resultType, Value input1, Value input2, Value output):
        ArithmeticInsn(Opcode::SUB, resultType, input1, input2, output)
    {}
};

class MulInstruction final : public ArithmeticInsn {
public:
    MulInstruction(ValueType resultType, Value input1, Value input2, Value output):
        ArithmeticInsn(Opcode::MUL, resultType, input1, input2, output)
    {}
};

class DivInstruction final : public ArithmeticInsn {
public:
    DivInstruction(ValueType resultType, Value input1, Value input2, Value output):
        ArithmeticInsn(Opcode::DIV, resultType, input1, input2, output)
    {}
};


} // namespace compiler

#endif // IR_INSTRUCTIONS_H
