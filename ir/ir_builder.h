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

    Value *CreateParameter(ValueType type, size_t num)
    {
        
    }

    Value *CreateConstant()
    {

    }

    CreateValue()

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
