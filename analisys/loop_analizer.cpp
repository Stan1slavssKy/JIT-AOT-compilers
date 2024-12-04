#include "analisys/loop_analizer.h"
#include "analisys/loop.h"

#include "ir/graph.h"
#include "ir/basic_block.h"

namespace compiler {

void LoopAnalizer::Run()
{
    graph_->BuildDominatorTree();
    CreateRootLoop();
    CollectLatches();
    PopulateLoops();
    BuildLoopTree();
}

void LoopAnalizer::CreateRootLoop()
{
    auto rootLoop = std::make_unique<Loop>(nullptr);
    rootLoop->MarkAsRoot();
    rootLoop_ = rootLoop.get();
    graph_->SetRootLoop(std::move(rootLoop));
}

void LoopAnalizer::CollectLatches()
{
    blackMrk_ = graph_->CreateNewMarker();
    grayMrk_ = graph_->CreateNewMarker();

    auto *root = graph_->GetStartBlock();
    SearchLatchDFS(root);

    graph_->DeleteMarker(blackMrk_);
    graph_->DeleteMarker(grayMrk_);
    graph_->UnmarkRpo();
}

void LoopAnalizer::SearchLatchDFS(BasicBlock *block)
{
    assert(block);

    block->SetMarker(blackMrk_);
    block->SetMarker(grayMrk_);

    for (auto succBlock : block->GetSuccessors()) {
        if (succBlock->IsMarked(grayMrk_)) {
            ProcessNewLatch(succBlock, block);
        } else if (!succBlock->IsMarked(blackMrk_)) {
            SearchLatchDFS(succBlock);
        }
    }

    block->Unmark(grayMrk_);
}

void LoopAnalizer::ProcessNewLatch(BasicBlock *header, BasicBlock *latch)
{
    auto loop = header->GetLoop();
    if (loop == nullptr) {
        loop = graph_->CreateNewLoop(header);
        header->SetLoop(loop);
    }

    loop->AddLatch(latch);

    bool isReducible = header->IsDominatesOver(latch);
    loop->SetReducible(isReducible);
}

void LoopAnalizer::PopulateLoops()
{
    blackMrk_ = graph_->CreateNewMarker();

    auto &rpoVector = graph_->GetRpoVector();
    for (auto blockIt : rpoVector) {
        auto *loop = blockIt->GetLoop();
        if (loop == nullptr || !blockIt->IsHeader()) {
            continue;
        }
        // now we in header
        if (loop->IsReducible()) {
            ProcessReducibleLoopHeader(loop, blockIt);
        } else {
            ProcessIrreducibleLoopHeader(loop);
        }
    }

    graph_->DeleteMarker(blackMrk_);
}

void LoopAnalizer::BuildLoopTree()
{
    for (auto block : graph_->GetRpoVector()) {
        auto blockLoop = block->GetLoop();
        if (blockLoop == nullptr) {
            rootLoop_->PushBlock(block);
        } else if (blockLoop->GetOuterLoop() == nullptr) {
            rootLoop_->AddInnerLoop(blockLoop);
            blockLoop->SetOuterLoop(rootLoop_);
        }
    }
}

void LoopAnalizer::ProcessReducibleLoopHeader(Loop *loop, BasicBlock *header)
{
    header->SetMarker(blackMrk_);

    for (auto latch : loop->GetLatches()) {
        LoopSearchDFS(loop, latch);
    }

    graph_->UnmarkRpo(blackMrk_);
}

void LoopAnalizer::LoopSearchDFS(Loop *loop, BasicBlock *block)
{
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

    for (auto predecessor : block->GetPredecessors()) {
        LoopSearchDFS(loop, predecessor);
    }
}

void LoopAnalizer::ProcessIrreducibleLoopHeader(Loop *loop)
{
    for (auto latch : loop->GetLatches()) {
        if (latch->GetLoop() != nullptr) {
            loop->PushBlock(latch);
        }
    }
}

}  // namespace compiler
