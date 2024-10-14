#ifndef IR_VALUE_H
#define IR_VALUE_H

#include "utils/macros.h"

#include <variant>
#include <cstdint>

namespace compiler {

enum class ValueType {
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

class Value {
public:
    using ValueVariant = std::variant<std::monostate, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                                      uint64_t, float, double>;

public:
    DEFAULT_COPY_SEMANTIC(Value);
    DEFAULT_MOVE_SEMANTIC(Value);

    Value(ValueVariant val) : value_(val) {}

private:
    ValueVariant value_;
    ValueType type_ {ValueType::UNDEFINED};
};

}  // namespace compiler

#endif  // IR_VALUE_H
