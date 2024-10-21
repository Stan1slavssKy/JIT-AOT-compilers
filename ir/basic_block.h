#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include "utils/macros.h"
#include "ir/instruction.h"

#include <vector>
#include <string>
#include <memory>

namespace compiler {

class Graph;

using BasicBlockId = size_t;

class BasicBlock final {
public:
    NO_COPY_SEMANTIC(BasicBlock);
    NO_MOVE_SEMANTIC(BasicBlock);

    BasicBlock() = default;
    ~BasicBlock() = default;

    void PushInstruction(Instruction *insn)
    {
        if (lastInsn_ == nullptr) {
            firstInsn_ = insn;
            lastInsn_ = firstInsn_;
        } else {
            lastInsn_->SetNext(insn);
            insn->SetPrev(lastInsn_);
            lastInsn_ = insn;
        }
    }

    void SetId(BasicBlockId id)
    {
        bbId_ = id;
    }

    BasicBlockId GetId() const
    {
        return bbId_;
    }

    void SetGraph(Graph *graph)
    {
        graph_ = graph;
    }

    void AddSuccessor(BasicBlock *block)
    {
        assert(successors_.size() != 2);
        successors_.push_back(block);
    }

    void AddPredecessor(BasicBlock *block)
    {
        predecessors_.push_back(block);
    }

    Instruction *GetLastInsn() const
    {
        return lastInsn_;
    }

private:
    BasicBlockId bbId_ {0};

    std::vector<BasicBlock *> predecessors_;
    std::vector<BasicBlock *> successors_;

    Instruction *firstPhi_ {nullptr};
    Instruction *firstInsn_ {nullptr};
    Instruction *lastInsn_ {nullptr};

    Graph *graph_ {nullptr};
};

}  // namespace compiler

#endif  // IR_BASIC_BLOCK_H
