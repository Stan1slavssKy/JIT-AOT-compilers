#include "ir/graph.h"
#include "analysis/rpo.h"
#include "analysis/dominator_tree.h"
#include "ir/instructions.h"

namespace compiler {

void Graph::AddBlock(std::unique_ptr<BasicBlock> block)
{
    size_t currblockNum = basicBlocks_.size();
    block->SetId(currblockNum);
    block->SetGraph(this);
    basicBlocks_.push_back(std::move(block));
}

void Graph::AddInstruction(std::unique_ptr<Instruction> insn)
{
    insn->SetId(instructions_.size());
    instructions_.push_back(std::move(insn));
}

void Graph::Dump(std::stringstream &ss) const
{
    for (auto &bb : basicBlocks_) {
        bb->Dump(ss);
    }
}

BasicBlock *Graph::GetStartBlock() const
{
    return basicBlocks_.front().get();
}

size_t Graph::GetAliveBlockCount() const
{
    return basicBlocks_.size();
}

void Graph::RunRpo()
{
    RPO rpo(this);
    auto marker = CreateNewMarker();
    rpo.SetMarker(marker);
    rpoVector_ = rpo.Run();

    EraseMarker(marker);
}

std::vector<BasicBlock *> &Graph::GetRpoVector()
{
    return rpoVector_;
}

const std::vector<BasicBlock *> &Graph::GetRpoVector() const
{
    return rpoVector_;
}

void Graph::BuildDominatorTree()
{
    DominatorTree tree(this);
    tree.Build();
}

void Graph::SetRootLoop(std::unique_ptr<Loop> rootLoop)
{
    rootLoop_ = std::move(rootLoop);
}

Loop *Graph::GetRootLoop() const
{
    return rootLoop_.get();
}

Marker Graph::CreateNewMarker()
{
    return markerManager_.CreateNewMarker();
}

void Graph::EraseMarker(Marker marker)
{
    markerManager_.EraseMarker(marker);
}

Loop *Graph::CreateNewLoop(BasicBlock *header)
{
    auto loop = std::make_unique<Loop>(header);
    auto loopPtr = loop.get();
    loop->PushBlock(header);

    loops_.push_back(std::move(loop));
    return loopPtr;
}

}  // namespace compiler
