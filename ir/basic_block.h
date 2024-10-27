#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include "utils/macros.h"
#include "ir/instruction.h"

#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace compiler {

class Graph;

using BasicBlockId = size_t;
using Marker = bool;

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

    const std::vector<BasicBlock *> &GetSuccessors() const
    {
        return successors_;
    }

    const std::vector<BasicBlock *> &GetPredecessors() const
    {
        return predecessors_;
    }

    Instruction *GetLastInsn() const
    {
        return lastInsn_;
    }

    void SetMarker(Marker marker)
    {
        marker_ = marker;
    }

    bool IsMarked(Marker marker) const
    {
        return marker_ == marker;
    }

    const std::vector<BasicBlock *> &GetDominatedBlocks() const
    {
        return dominatedBlocks_;
    }

    BasicBlock *GetDominator()
    {
        return dominator_;
    }

    const BasicBlock *GetDominator() const
    {
        return dominator_;
    }

    void SetDominatedBlocks(std::vector<BasicBlock *> &&dominatedBlocks)
    {
        dominatedBlocks_ = std::move(dominatedBlocks);
    }

    void SetDominatedBlocks(const std::vector<BasicBlock *> &dominatedBlocks)
    {
        dominatedBlocks_ = dominatedBlocks;
    }

    void Dump(std::stringstream &ss) const;

private:
    BasicBlockId bbId_ {0};

    std::vector<BasicBlock *> predecessors_;
    std::vector<BasicBlock *> successors_;

    Instruction *firstPhi_ {nullptr};
    Instruction *firstInsn_ {nullptr};
    Instruction *lastInsn_ {nullptr};

    Graph *graph_ {nullptr};

    Marker marker_ {false};

    BasicBlock *dominator_ {nullptr};
    std::vector<BasicBlock *> dominatedBlocks_;
};

}  // namespace compiler

#endif  // IR_BASIC_BLOCK_H
