#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <cstddef>

#include "ir/value.h"

namespace compiler {

enum Opcode : size_t {
#define _(instr) instr,
#include "ir/instruction_type.def"
#undef _
};

class Instruction;

struct Inputs {
    std::vector<Value> intputValues_;
};

struct Users {
    std::vector<Instruction *> usersInstructions_;
};

class InstructionId {
public:
    InstructionId()
    {
        static size_t counter = 0;
        insnId_ = counter++;
    }

    size_t GetId() const
    {
        return insnId_;
    }

private:
    static constexpr size_t INVALID_INSN_ID = -1;
    size_t insnId_ {INVALID_INSN_ID};
};

class Instruction {
public:
    NO_COPY_SEMANTIC(Instruction);
    NO_MOVE_SEMANTIC(Instruction);

    Instruction(Opcode opcode, ValueType resultType) : opcode_(opcode), resultType_(resultType)
    {}

    void SetParentBB(BasicBlock *bb)
    {
        parentBB_ = bb;
    }

    void AddInput(Value input)
    {
        inputs_.intputValues_.push_back(input);
    }

    void AddUser(Instruction *user)
    {
        users_.usersInstructions_.push_back(user);
    }

private:
    Instruction *prev_ {nullptr};
    Instruction *next_ {nullptr};

    BasicBlock *parentBB_ {nullptr};

    InstructionId insnId_;
    Opcode opcode_ {Opcode::UNDEFINED};
    ValueType resultType_;

    Inputs inputs_;
    Users users_;
};

}  // namespace compiler

#endif  // IR_INSTRUCTION_H
