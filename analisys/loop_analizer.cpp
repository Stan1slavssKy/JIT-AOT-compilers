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
}

void LoopAnalizer::CreateRootLoop()
{
    auto rootLoop = std::make_unique<Loop>();
    rootLoop->MarkAsRoot();
    rootLoop_ = rootLoop.get();
    graph_->SetRootLoop(std::move(rootLoop));
}

void LoopAnalizer::CollectLatches()
{
    auto *root = graph_->GetStartBlock();
    (void)root;
}

void LoopAnalizer::SearchLatch(BasicBlock *block)
{
    (void)block;
}

}  // namespace compiler
