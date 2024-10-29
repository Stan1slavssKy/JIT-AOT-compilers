#include "analisys/loop_analizer.h"
#include "analisys/loop.h"
#include "ir/graph.h"

namespace compiler {

void LoopAnalizer::Run()
{
    graph_->BuildDominatorTree();
    CreateRootLoop();
    CollectBackEdges();
}

void LoopAnalizer::CreateRootLoop()
{
    auto rootLoop = std::make_unique<Loop>();
    rootLoop->MarkAsRoot();
    rootLoop_ = rootLoop.get();
    graph_->SetRootLoop(std::move(rootLoop));
}

void LoopAnalizer::CollectBackEdges()
{
    auto *root = graph_->GetStartBlock();

}

} // namespace compiler
