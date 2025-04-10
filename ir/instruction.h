#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include "ir/data_types.h"
#include "ir/opcodes.h"
#include "utils/macros.h"
#include "ir/inputs.h"

#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <memory>
#include <iostream>
namespace compiler {

class ConstantInsn;
class BasicBlock;

using InstructionId = size_t;

class Instruction {
public:
    NO_COPY_SEMANTIC(Instruction);
    NO_MOVE_SEMANTIC(Instruction);

    Instruction(Opcode opcode, DataType resultType = DataType::UNDEFINED) : opcode_(opcode), resultType_(resultType)
    {
        if (IsCall() || IsPhi()) {
            inputs_ = std::make_unique<VectorInputs>();
        } else {
            inputs_ = std::make_unique<DefaultInputs>();
        }
    }

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

    const InputsContainerIface *GetInputs() const
    {
        return inputs_.get();
    }

    InputsContainerIface *GetInputs()
    {
        return inputs_.get();
    }

    bool TryReplaceInput(Instruction *inputToReplace, Instruction *insnToReplaceWith, size_t idx);

    bool ReplaceInputs(Instruction *inputToReplace, Instruction *insnToReplaceWith);

    /// For all users of this insn change inputs from this insn to given.
    void ReplaceInputsForUsers(Instruction *insnToReplaceWith);

    bool IsPhi() const
    {
        return opcode_ == Opcode::PHI;
    }

    bool IsCall() const
    {
        return opcode_ == Opcode::CALLSTATIC;
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

    ConstantInsn *AsConst();

    const ConstantInsn *AsConst() const;

    Opcode GetOpcode() const
    {
        return opcode_;
    }

    void SetResultType(DataType type)
    {
        resultType_ = type;
    }

    DataType GetResultType() const
    {
        return resultType_;
    }

    bool IsIntResultType() const
    {
        return resultType_ == DataType::I32 || resultType_ == DataType::U32 || resultType_ == DataType::I64 ||
               resultType_ == DataType::U64;
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

    std::unique_ptr<InputsContainerIface> inputs_;
    std::list<Instruction *> users_;
};

}  // namespace compiler

#endif  // IR_INSTRUCTION_H
