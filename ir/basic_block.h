#ifndef IR_BASIC_BLOCK_H
#define IR_BASIC_BLOCK_H

#include "utils/macros.h"
#include "ir/instruction.h"
#include "ir/marker.h"

#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace compiler {

class Graph;
class Loop;

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

    /// `prevInsn` -- instruction after which `insn` will be inserted
    /// `prevInsn` must be pass as nullpt to insert `insn` in the beginning
    void InsertInstruction(Instruction *prevInsn, Instruction *insn)
    {
        if (prevInsn == lastInsn_) {
            PushInstruction(insn);
            return;
        }

        if (prevInsn == nullptr) {
            firstInsn_->SetPrev(insn);
            insn->SetNext(firstInsn_);
            insn->SetPrev(nullptr);
            firstInsn_ = insn;
            return;
        }

        auto *next = prevInsn->GetNext();
        assert(next != nullptr);

        next->SetPrev(insn);
        insn->SetNext(next);

        insn->SetPrev(prevInsn);
        prevInsn->SetNext(insn);
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
        marker_ |= marker;
    }

    void Unmark(Marker marker)
    {
        marker_ &= (~marker);
    }

    void Unmark();

    bool IsMarked(Marker marker) const
    {
        return marker_ & marker;
    }

    const std::vector<BasicBlock *> &GetDominatedBlocks() const
    {
        return dominatedBlocks_;
    }

    void SetImmediateDominator(BasicBlock *dominator)
    {
        immediateDominator_ = dominator;
    }

    BasicBlock *GetImmediateDominator()
    {
        return immediateDominator_;
    }

    const BasicBlock *GetImmediateDominator() const
    {
        return immediateDominator_;
    }

    void SetDominatedBlocks(std::vector<BasicBlock *> &&dominatedBlocks)
    {
        dominatedBlocks_ = std::move(dominatedBlocks);
    }

    void SetDominatedBlocks(const std::vector<BasicBlock *> &dominatedBlocks)
    {
        dominatedBlocks_ = dominatedBlocks;
    }

    bool IsDominatesOver(BasicBlock *block) const
    {
        if (block == this) {
            return true;
        }
        auto it = std::find_if(dominatedBlocks_.begin(), dominatedBlocks_.end(),
                               [block](auto domBlock) { return domBlock == block; });
        return it != dominatedBlocks_.end();
    }

    void SetLoop(Loop *loop)
    {
        loop_ = loop;
    }

    Loop *GetLoop()
    {
        return loop_;
    }

    const Loop *GetLoop() const
    {
        return loop_;
    }

    bool IsHeader() const;

    void Dump(std::stringstream &ss) const;

private:
    BasicBlockId bbId_ {0};

    std::vector<BasicBlock *> predecessors_;
    std::vector<BasicBlock *> successors_;

    Instruction *firstPhi_ {nullptr};
    Instruction *firstInsn_ {nullptr};
    Instruction *lastInsn_ {nullptr};

    Graph *graph_ {nullptr};

    Marker marker_;

    BasicBlock *immediateDominator_ {nullptr};
    std::vector<BasicBlock *> dominatedBlocks_;

    Loop *loop_ {nullptr};
};

}  // namespace compiler

#endif  // IR_BASIC_BLOCK_H
