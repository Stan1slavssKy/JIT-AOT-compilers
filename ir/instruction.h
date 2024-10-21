#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include "ir/data_types.h"
#include "ir/opcodes.h"
#include "utils/macros.h"

#include <vector>
#include <sstream>

namespace compiler {

class Instruction;
class BasicBlock;

struct Input {
    Instruction *input {nullptr};
};

struct User {
    Instruction *user {nullptr};
};

using InstructionId = size_t;

class Instruction {
public:
    NO_COPY_SEMANTIC(Instruction);
    NO_MOVE_SEMANTIC(Instruction);

    Instruction(Opcode opcode, DataType resultType = DataType::UNDEFINED) : opcode_(opcode), resultType_(resultType) {}

    virtual ~Instruction() = default;

    void SetParentBB(BasicBlock *bb)
    {
        parentBB_ = bb;
    }

    BasicBlock *GetParentBB() const
    {
        return parentBB_;
    }

    void SetNext(Instruction *nextInsn)
    {
        next_ = nextInsn;
    }

    Instruction *GetNext() const
    {
        return next_;
    }

    void SetPrev(Instruction *prevInsn)
    {
        prev_ = prevInsn;
    }

    Instruction *GetPrev() const
    {
        return prev_;
    }

    void AddInput(Instruction *input)
    {
        inputs_.push_back(Input {input});
    }

    void AddUser(Instruction *user)
    {
        users_.push_back(User {user});
    }

    void SetInputs(std::vector<Input> inputs)
    {
        inputs_ = std::move(inputs);
    }

    const std::vector<Input> &GetInputs() const
    {
        return inputs_;
    }

    bool IsPhi() const
    {
        return opcode_ == Opcode::PHI;
    }

    bool IsJmp() const
    {
        return opcode_ == Opcode::JMP;
    }

    bool IsBranch() const
    {
        return (opcode_ == Opcode::BEQ) || (opcode_ == Opcode::BNE) || (opcode_ == Opcode::BGT);
    }

    void SetResultType(DataType type)
    {
        resultType_ = type;
    }

    void SetId(InstructionId id)
    {
        insnId_ = id;
    }

    InstructionId GetId() const
    {
        return insnId_;
    }

    void Dump(std::stringstream &ss) const;

private:
    Instruction *prev_ {nullptr};
    Instruction *next_ {nullptr};

    BasicBlock *parentBB_ {nullptr};

    InstructionId insnId_ {0};
    Opcode opcode_ {Opcode::UNDEFINED};
    DataType resultType_;

    std::vector<Input> inputs_;
    std::vector<User> users_;
};

}  // namespace compiler

#endif  // IR_INSTRUCTION_H
