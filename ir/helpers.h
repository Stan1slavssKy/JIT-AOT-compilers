#ifndef IR_HELPERS_H
#define IR_HELPERS_H

#include "ir/data_types.h"
#include "ir/opcodes.h"
#include "utils/macros.h"

#include <string>

namespace compiler {

inline std::string DataTypeToStr(DataType type)
{
    switch (type) {
        case DataType::UNDEFINED:
            return "undefined";
        case DataType::VOID:
            return "void";
        case DataType::I8:
            return "i8";
        case DataType::U8:
            return "u8";
        case DataType::I16:
            return "i16";
        case DataType::U16:
            return "u16";
        case DataType::I32:
            return "i32";
        case DataType::U32:
            return "u32";
        case DataType::I64:
            return "i64";
        case DataType::U64:
            return "u64";
        case DataType::F32:
            return "f32";
        case DataType::F64:
            return "f64";
        case DataType::REF:
            return "ref";
        default:
            UNREACHABLE();
    }
}

inline std::string OpcodeToString(Opcode opcode)
{
    (void)opcode;
#define _(insn)        \
    case Opcode::insn: \
        return #insn;

    switch (opcode) {
#include "ir/instruction_type.def"

        default:
            UNREACHABLE();
    }

#undef _
}

}  // namespace compiler

#endif  // IR_HELPERS_H
