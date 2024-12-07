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

    graph_->EraseMarker(blackMrk_);
    graph_->EraseMarker(grayMrk_);
}

void LoopAnalizer::SearchLatchDFS(BasicBlock *block)
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

void LoopAnalizer::ProcessNewLatch(BasicBlock *header, BasicBlock *latch)
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

void LoopAnalizer::PopulateLoops()
{
    auto &rpoVector = graph_->GetRpoVector();
    for (auto block : rpoVector) {
        auto *loop = block->GetLoop();
        if (loop == nullptr || !block->IsHeader()) {
            continue;
        }
        // now we in header
        if (loop->IsReducible()) {
            std::cerr << "IsReducible!" << std::endl;
            ProcessReducibleLoopHeader(loop, block);
        } else {
            ProcessIrreducibleLoopHeader(loop);
            std::cerr << "IsNOTReducible!" << std::endl;
        }
    }
}

void LoopAnalizer::BuildLoopTree()
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

void LoopAnalizer::ProcessReducibleLoopHeader(Loop *loop, BasicBlock *header)
{
    blackMrk_ = graph_->CreateNewMarker();

    header->SetMarker(blackMrk_);
    for (auto *latch : loop->GetLatches()) {
        std::cerr << "latch = " << latch << " name = " << static_cast<char>(latch->GetId() + 'A') << std::endl;
        LoopSearchDFS(loop, latch);
    }

    graph_->EraseMarker(blackMrk_);
}

void LoopAnalizer::LoopSearchDFS(Loop *loop, BasicBlock *block)
{
    assert(loop);
    assert(block);

    std::cerr << "LoopSearchDFS" << std::endl;

    if (block->IsMarked(blackMrk_)) {
        std::cerr << "return from LoopSearchDFS" << std::endl;
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
            std::cerr << ">>>Add Inner loop" << std::endl;
            std::cerr << "block is = "
                      << " name = " << static_cast<char>(block->GetId() + 'A') << std::endl;
        }
    }

    for (auto *predecessor : block->GetPredecessors()) {
        std::cerr << "pred = " << predecessor << " name = " << static_cast<char>(predecessor->GetId() + 'A')
                  << std::endl;
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
