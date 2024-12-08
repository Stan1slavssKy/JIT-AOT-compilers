#ifndef ANALYSIS_LOOP_H
#define ANALYSIS_LOOP_H

#include <vector>

namespace compiler {

class BasicBlock;

class Loop final {
public:
    Loop(BasicBlock *header) : header_(header) {}

    void MarkAsRoot()
    {
        isRoot_ = true;
    }

    void AddLatch(BasicBlock *latch)
    {
        latches_.push_back(latch);
    }

    void PushBlock(BasicBlock *block)
    {
        blocks_.push_back(block);
    }

    const std::vector<BasicBlock *> GetBlocks() const
    {
        return blocks_;
    }

    void SetReducible(bool reducible)
    {
        isReducible_ = reducible;
    }

    bool IsReducible() const
    {
        return isReducible_;
    }

    BasicBlock *GetHeader() const
    {
        return header_;
    }

    const std::vector<BasicBlock *> &GetLatches() const
    {
        return latches_;
    }

    Loop *GetOuterLoop() const
    {
        return outerLoop_;
    }

    void SetOuterLoop(Loop *outerLoop)
    {
        outerLoop_ = outerLoop;
    }

    const std::vector<Loop *> &GetInnerLoops() const
    {
        return innerLoops_;
    }

    void AddInnerLoop(Loop *loop)
    {
        innerLoops_.push_back(loop);
    }

private:
    BasicBlock *header_ {nullptr};
    std::vector<BasicBlock *> latches_;
    std::vector<BasicBlock *> blocks_;

    Loop *outerLoop_ {nullptr};
    std::vector<Loop *> innerLoops_;

    bool isReducible_ {false};
    bool isRoot_ {false};
};

}  // namespace compiler

#endif  // ANALYSIS_LOOP_H
