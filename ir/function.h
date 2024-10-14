#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H

#include "utils/macros.h"
#include "ir/value.h"

#include <string>
#include <vector>

namespace compiler {

class Function {
public:
    DEFAULT_COPY_SEMANTIC(Function);
    DEFAULT_MOVE_SEMANTIC(Function);

    Function(const std::string &name, ValueType retType = ValueType::UNDEFINED, const std::vector<ValueType> &args = {})
        : name_(name), retType_(retType), args_(args)
    {
    }

    void SetReturnType(ValueType retType)
    {
        retType_ = retType;
    }

    void SetArgs(const std::vector<ValueType> &args)
    {
        args_ = args;
    }

private:
    std::string name_;
    ValueType retType_;
    std::vector<ValueType> args_;
};

}  // namespace compiler

#endif  // IR_FUNCTION_H
