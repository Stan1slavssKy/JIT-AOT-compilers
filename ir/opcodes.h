#ifndef IR_OPCODES_H
#define IR_OPCODES_H

#include <cstddef>

namespace compiler {

enum Opcode : size_t {
#define _(instr) instr,
#include "ir/instruction_type.def"
#undef _
};

}  // namespace compiler

#endif  // IR_OPCODES_H
