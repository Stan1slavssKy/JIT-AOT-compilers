#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include "ir/data_types.h"
#include "ir/opcodes.h"
#include "utils/macros.h"

#include <array>
#include <list>
#include <sstream>
#include <iostream>
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

    void RemoveUser(Instruction *userToRemove)
    {
        auto it = std::find(users_.begin(), users_.end(), userToRemove);
        if (it != users_.end()) {
            users_.erase(it);
        }
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

    bool TryReplaceInput(Instruction *inputToReplace, Instruction *insnToReplaceWith, size_t idx)
    {
        if (inputs_[idx] == inputToReplace) {
            inputs_[idx] = insnToReplaceWith;
            return true;
        }
        return false;
    }

    bool ReplaceInputs(Instruction *inputToReplace, Instruction *insnToReplaceWith)
    {
        bool succ0 = TryReplaceInput(inputToReplace, insnToReplaceWith, 0);
        bool succ1 = TryReplaceInput(inputToReplace, insnToReplaceWith, 1);

        return succ0 || succ1;
    }

    /// For all users of this insn change inputs from this insn to given.
    void ReplaceInputsForUsers(Instruction *insnToReplaceWith)
    {
        for (auto userIt = users_.begin(); userIt != users_.end();) {
            // Need to save iterator, because of removing user from list below.
            auto currUser = *(userIt++);
            if (currUser->ReplaceInputs(this, insnToReplaceWith)) {
                // Now we successfully replace input for current user,
                // it means that `this` insn do not have this user, so we need to remove it.
                this->RemoveUser(currUser);
                // Current user is new user for insn by which we replace `this` insn.
                insnToReplaceWith->AddUser(currUser);
            }
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
