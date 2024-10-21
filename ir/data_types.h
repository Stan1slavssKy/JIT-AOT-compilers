#ifndef IR_DATA_TYPES_H
#define IR_DATA_TYPES_H

namespace compiler {

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

}  // namespace compiler

#endif  // IR_DATA_TYPES_H
