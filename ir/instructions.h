#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "ir/instruction.h"
#include "utils/bit_utils.h"

namespace compiler {

class ParameterInsn final : public Instruction {
public:
    ParameterInsn(uint32_t argNum) : Instruction(Opcode::PARAMETER, DataType::U32), argNum_(argNum) {}

    uint32_t GetArgNum() const
    {
        return argNum_;
    }

private:
    uint32_t argNum_ {0};
};

// Available types: integer types, float32, float64.
class ConstantInsn final : public Instruction {
public:
    template <typename T>
    ConstantInsn(T value) : Instruction(Opcode::CONSTANT)
    {
        if constexpr (std::is_integral_v<T>) {
            value_ = value;
        } else if constexpr (std::is_same_v<T, float>) {
            value_ = utils::bit_cast<uint32_t, float>(value);
        } else if constexpr (std::is_same_v<T, double>) {
            value_ = utils::bit_cast<uint64_t, double>(value);
        } else {
            UNREACHABLE();
        }
    }

private:
    uint64_t value_ {0};
};

class PhiInsn final : public Instruction {
public:
    PhiInsn(DataType resultType) : Instruction(Opcode::PHI, resultType) {}

    BasicBlock *GetPhiInputBB(size_t idx)
    {
        // TODO
        (void)idx;
        return nullptr;
    }

    Instruction *GetPhiInput(BasicBlock *bb)
    {
        // TODO
        (void)bb;
        return nullptr;
    }
};

class ArithmeticInsn : public Instruction {
public:
    ArithmeticInsn(Opcode opcode, DataType resultType, Instruction *input1, Instruction *input2)
        : Instruction(opcode, resultType)
    {
        AddInput(input1);
        AddInput(input2);
        input1->AddUser(this);
        input2->AddUser(this);
    }
};

class AddInsn final : public ArithmeticInsn {
public:
    AddInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::ADD, resultType, input1, input2)
    {
    }
};

class SubInsn final : public ArithmeticInsn {
public:
    SubInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::SUB, resultType, input1, input2)
    {
    }
};

class MulInsn final : public ArithmeticInsn {
public:
    MulInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::MUL, resultType, input1, input2)
    {
    }
};

class DivInsn final : public ArithmeticInsn {
public:
    DivInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::DIV, resultType, input1, input2)
    {
    }
};

class RemInsn final : public ArithmeticInsn {
public:
    RemInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::REM, resultType, input1, input2)
    {
    }
};

class AndInsn final : public ArithmeticInsn {
public:
    AndInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::ADD, resultType, input1, input2)
    {
    }
};

class OrInsn final : public ArithmeticInsn {
public:
    OrInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::OR, resultType, input1, input2)
    {
    }
};

class XorInsn final : public ArithmeticInsn {
public:
    XorInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::XOR, resultType, input1, input2)
    {
    }
};

class JmpInsn final : public Instruction {
public:
    JmpInsn(BasicBlock *bbToJmp) : Instruction(Opcode::JMP, DataType::VOID), bbToJmp_(bbToJmp) {}

    BasicBlock *GetBBToJmp() const
    {
        return bbToJmp_;
    }

private:
    BasicBlock *bbToJmp_ {nullptr};
};

class BranchInsn : public Instruction {
public:
    BranchInsn(Opcode opcode, Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : Instruction(opcode, DataType::VOID), ifTrueBB_(ifTrueBB), ifFalseBB_(ifFalseBB)
    {
        AddInput(input1);
        AddInput(input2);
        input1->AddUser(this);
        input2->AddUser(this);
    }

    BasicBlock *GetTrueBranchBB() const
    {
        return ifTrueBB_;
    }

    BasicBlock *GetFalseBranchBB() const
    {
        return ifFalseBB_;
    }

private:
    BasicBlock *ifTrueBB_ {nullptr};
    BasicBlock *ifFalseBB_ {nullptr};
};

class BgtInsn final : public BranchInsn {
public:
    BgtInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BGT, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class BeqInsn final : public BranchInsn {
public:
    BeqInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BEQ, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class BneInsn final : public BranchInsn {
public:
    BneInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BNE, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class RetInsn final : public Instruction {
public:
    RetInsn(DataType retType, Instruction *input) : Instruction(Opcode::RET, retType), retValue_(input)
    {
        AddInput(input);
        input->AddUser(this);
    }

private:
    Instruction *retValue_ {nullptr};
};

}  // namespace compiler

#endif  // IR_INSTRUCTIONS_H
