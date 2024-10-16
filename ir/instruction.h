#ifndef IR_INSTRUCTION_H
#define IR_INSTRUCTION_H

#include <cstddef>

namespace compiler {

enum Opcode : size_t {
#define _(instr) instr,
#include "ir/instruction_type.def"
#undef _
};

struct InstructionId {
    InstructionId(size_t id) : insnId(id) {}

    static constexpr size_t INVALID_INSN_ID = -1;
    size_t insnId {INVALID_INSN_ID};
    bool isPhi {false};
};

class Instruction {
public:
    NO_COPY_SEMANTIC(Instruction);
    NO_MOVE_SEMANTIC(Instruction);

    Instruction(Opcode opcode) : opcode_(opcode)
    {}

private:
    Instruction *prev_ {nullptr};
    Instruction *next_ {nullptr};

    Opcode opcode_ {Opcode::UNDEFINED};
    InstructionId insnId_;
};

}  // namespace compiler

#endif  // IR_INSTRUCTION_H
