#ifndef IR_OPCODES_H
#define IR_OPCODES_H

#include <cstddef>

namespace compiler {

enum Opcode : size_t {
#define OPCODE_MACROS(instr, _) instr,
#include "ir/instruction_type.def"
#undef OPCODE_MACROS
};

}  // namespace compiler

#endif  // IR_OPCODES_H
