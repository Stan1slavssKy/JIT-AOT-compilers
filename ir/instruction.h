#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include "ir/data_types.h"
#include "ir/opcodes.h"
#include "utils/macros.h"

#include <array>
#include <list>
#include <sstream>

namespace compiler {

class Instruction;
class BasicBlock;

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

    void AddUser(Instruction *user)
    {
        users_.push_back(user);
    }

    std::list<Instruction *> &GetUsers()
    {
        return users_;
    }

    const std::list<Instruction *> &GetUsers() const
    {
        return users_;
    }

    std::list<Instruction *>::iterator RemoveUser(std::list<Instruction *>::iterator userIt)
    {
        return users_.erase(userIt);
    }

    void SetInput(Instruction *input, size_t idx)
    {
        assert(input != nullptr);
        assert(idx < inputs_.size());
        inputs_[idx] = input;
    }

    Instruction *GetInput(size_t idx)
    {
        assert(idx < inputs_.size());
        return inputs_[idx];
    }

    const Instruction *GetInput(size_t idx) const
    {
        assert(idx < inputs_.size());
        return inputs_[idx];
    }

    void SwapInputs()
    {
        auto *temp = inputs_[0];
        inputs_[0] = inputs_[1];
        inputs_[1] = temp;
    }

    void ReplaceInput(Instruction *inputToReplace, Instruction *insnToReplaceWith)
    {
        if (inputs_[0] == inputToReplace) {
            inputs_[0] = insnToReplaceWith;
            insnToReplaceWith->AddUser(this);
        }
        if (inputs_[1] == inputToReplace) {
            inputs_[1] = insnToReplaceWith;
            if (inputs_[0] != inputs_[1]) {
                insnToReplaceWith->AddUser(this);
            }
        }
    }

    void ReplaceInputsForUsers(Instruction *insnToReplaceWith)
    {
        for (auto *it : users_) {
            it->ReplaceInput(this, insnToReplaceWith);
        }
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

    bool IsConst() const
    {
        return opcode_ == Opcode::CONSTANT;
    }

    Opcode GetOpcode() const
    {
        return opcode_;
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

    virtual void Dump(std::stringstream &ss) const;

private:
    Instruction *prev_ {nullptr};
    Instruction *next_ {nullptr};

    BasicBlock *parentBB_ {nullptr};

    InstructionId insnId_ {0};
    Opcode opcode_ {Opcode::UNDEFINED};
    DataType resultType_;

    std::array<Instruction *, 2U> inputs_ {};
    std::list<Instruction *> users_;
};

}  // namespace compiler

#endif  // IR_INSTRUCTION_H
