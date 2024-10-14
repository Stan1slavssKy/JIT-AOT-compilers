#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include <vector>

#include "utils/macros.h"

namespace compiler {

class Instruction;
class Function;

using BasicBlockId = size_t;

class BasicBlock {
public:
    NO_COPY_SEMANTIC(BasicBlock);
    NO_MOVE_SEMANTIC(BasicBlock);

    BasicBlock()
    {
        static BasicBlockId id = 0;
        bbId_ = id++;
    }

    BasicBlock(Function *parentFunction) : BasicBlock()
    {
        parentFunction_ = parentFunction;
    }

    ~BasicBlock() = default;

    void PushBack(Instruction *insn)
    {
        insns_.push_back(insn);
    }

    size_t GetId() const
    {
        return bbId_;
    }

private:
    BasicBlockId bbId_ {-1};
    Function *parentFunction_ {nullptr};

    // TODO: replace it with intrusive list of instructions
    std::vector<Instruction *> insns_;

    std::vector<BasicBlock *> predecessors_;
    std::vector<BasicBlock *> successors_;
};

}  // namespace compiler

#endif  // BASIC_BLOCK_H
