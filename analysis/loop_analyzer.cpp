#include "analysis/loop_analyzer.h"
#include "analysis/loop.h"

#include "ir/graph.h"
#include "ir/basic_block.h"

namespace compiler {

void LoopAnalyzer::Run()
{
    graph_->BuildDominatorTree();
    CreateRootLoop();
    CollectLatches();
    PopulateLoops();
    BuildLoopTree();
}

void LoopAnalyzer::CreateRootLoop()
{
    auto rootLoop = std::make_unique<Loop>(nullptr);
    rootLoop->MarkAsRoot();
    rootLoop_ = rootLoop.get();
    graph_->SetRootLoop(std::move(rootLoop));
}

void LoopAnalyzer::CollectLatches()
{
    blackMrk_ = graph_->CreateNewMarker();
    grayMrk_ = graph_->CreateNewMarker();

    auto *root = graph_->GetStartBlock();
    SearchLatchDFS(root);

    graph_->EraseMarker(blackMrk_);
    graph_->EraseMarker(grayMrk_);
}

void LoopAnalyzer::SearchLatchDFS(BasicBlock *block)
{
    assert(block);

    block->SetMarker(blackMrk_);
    block->SetMarker(grayMrk_);

    for (auto *succBlock : block->GetSuccessors()) {
        if (succBlock->IsMarked(grayMrk_)) {
            ProcessNewLatch(succBlock, block);
        } else if (!succBlock->IsMarked(blackMrk_)) {
            SearchLatchDFS(succBlock);
        }
    }

    block->EraseMarker(grayMrk_);
}

void LoopAnalyzer::ProcessNewLatch(BasicBlock *header, BasicBlock *latch)
{
    assert(header);
    assert(latch);

    auto *loop = header->GetLoop();
    if (loop == nullptr) {
        loop = graph_->CreateNewLoop(header);
        header->SetLoop(loop);
    }

    loop->AddLatch(latch);
    bool isReducible = header->IsDominatesOver(latch);
    loop->SetReducible(isReducible);
}

void LoopAnalyzer::PopulateLoops()
{
    auto &rpoVector = graph_->GetRpoVector();
    // Reverse RPO order to have order from inner loop to outer.
    for (auto it = rpoVector.rbegin(); it != rpoVector.rend(); ++it) {
        auto *block = *it;
        auto *loop = block->GetLoop();
        if (loop == nullptr || !block->IsHeader()) {
            continue;
        }
        // now we in header
        if (loop->IsReducible()) {
            ProcessReducibleLoopHeader(loop, block);
        } else {
            ProcessIrreducibleLoopHeader(loop);
        }
    }
}

void LoopAnalyzer::BuildLoopTree()
{
    for (auto *block : graph_->GetRpoVector()) {
        auto *blockLoop = block->GetLoop();
        if (blockLoop == nullptr) {
            rootLoop_->PushBlock(block);
        } else if (blockLoop->GetOuterLoop() == nullptr) {
            rootLoop_->AddInnerLoop(blockLoop);
            blockLoop->SetOuterLoop(rootLoop_);
        }
    }
}

void LoopAnalyzer::ProcessReducibleLoopHeader(Loop *loop, BasicBlock *header)
{
    blackMrk_ = graph_->CreateNewMarker();

    header->SetMarker(blackMrk_);
    for (auto *latch : loop->GetLatches()) {
        LoopSearchDFS(loop, latch);
    }

    graph_->EraseMarker(blackMrk_);
}

void LoopAnalyzer::LoopSearchDFS(Loop *loop, BasicBlock *block)
{
    assert(loop);
    assert(block);

    if (block->IsMarked(blackMrk_)) {
        return;
    }
    block->SetMarker(blackMrk_);

    auto *blockLoop = block->GetLoop();
    if (blockLoop == nullptr) {
        loop->PushBlock(block);
        block->SetLoop(loop);
    } else if (loop->GetHeader() != blockLoop->GetHeader()) {
        // now we in inner loop
        if (blockLoop->GetOuterLoop() == nullptr) {
            blockLoop->SetOuterLoop(loop);
            loop->AddInnerLoop(blockLoop);
        }
    }

    for (auto *predecessor : block->GetPredecessors()) {
        LoopSearchDFS(loop, predecessor);
    }
}

void LoopAnalyzer::ProcessIrreducibleLoopHeader(Loop *loop)
{
    for (auto latch : loop->GetLatches()) {
        if (latch->GetLoop() != loop) {
            loop->PushBlock(latch);
            latch->SetLoop(loop);
        }
    }
}

}  // namespace compiler
