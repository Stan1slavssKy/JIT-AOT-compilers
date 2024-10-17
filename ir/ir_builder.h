#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include "utils/macros.h"
#include "ir/instruction.h"
#include "ir/value.h"
#include "ir/function.h"

#include <string>
#include <vector>
#include <memory>

namespace compiler {

class BasicBlock;

class IrBuilder final {
public:
    static IrBuilder *GetInstance()
    {
        if (instance_ == nullptr) {
            instance_ = new IrBuilder();
        }
        return instance_;
    }

    static void DeleteInstance()
    {
        delete instance_;
    }

    template <typename... T>
    Function *CreateFunction(T &&...args)
    {
        auto func = std::make_unique<Function>(std::forward<T>(args)...);
        functions_.push_back(std::move(func));
        return functions_.back().get();
    }

    BasicBlock *CreateBB(Function *func)
    {
        auto bb = std::make_unique<BasicBlock>(func);
        basicBlocks_.push_back(std::move(bb));
        return basicBlocks_.back().get();
    }

    template<typename T>
    static Value CreateValue(ValueType type, T value)
    {
        switch (type) {
            case ValueType::I8:
                return Value::Create<int32_t>(static_cast<int32_t>(value));
            case ValueType::U8:
                return Value::Create<uint32_t>(static_cast<uint32_t>(value));
            case ValueType::I16:
                return Value::Create<int16_t>(static_cast<int16_t>(value));
            case ValueType::U16:
                return Value::Create<uint16_t>(static_cast<uint16_t>(value));
            case ValueType::I32:
                return Value::Create<int32_t>(static_cast<int32_t>(value));
            case ValueType::U32:
                return Value::Create<uint32_t>(static_cast<uint32_t>(value));
            case ValueType::I64:
                return Value::Create<int64_t>(static_cast<int64_t>(value));
            case ValueType::U64:
                return Value::Create<uint64_t>(static_cast<uint64_t>(value));
            case ValueType::F32:
                return Value::Create<float>(static_cast<float>(value));
            case ValueType::F64:
                return Value::Create<double>(static_cast<double>(value));
            case ValueType::REF:
                return Value::Create<uintptr_t>(static_cast<uintptr_t>(value));
            default:
                UNREACHABLE();
        }
    }

    void CreateJmp(BasicBlock *currentBB, BasicBlock *bbToJmp)
    {
        
    }

private:
    NO_COPY_SEMANTIC(IrBuilder);
    NO_MOVE_SEMANTIC(IrBuilder);

    IrBuilder() = default;

private:
    static IrBuilder *instance_;

    std::vector<std::unique_ptr<Function>> functions_;
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks_;
};

}  // namespace compiler

#endif  // IR_BUILDER_H
