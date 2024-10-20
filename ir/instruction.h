#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <cstddef>
#include <vector>

namespace compiler {

enum Opcode : size_t {
#define _(instr) instr,
#include "ir/instruction_type.def"
#undef _
};

enum class DataType {
    UNDEFINED,
    VOID,
    I8,
    U8,
    I16,
    U16,
    I32,
    U32,
    I64,
    U64,
    F32,
    F64,
    REF,
};

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

    void SetNext(Instruction *nextInsn)
    {
        next_ = nextInsn;
    }

    void SetPrev(Instruction *prevInsn)
    {
        prev_ = prevInsn;
    }

    void AddInput(Instruction *input)
    {
        inputs_.push_back(Input {input});
    }

    void AddUser(Instruction *user)
    {
        (void)user;
        // users_.usersInstructions_.push_back(user);
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
