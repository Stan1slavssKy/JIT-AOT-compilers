#include "ir/graph.h"
#include "analisys/rpo.h"

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
    rpoVector_ = rpo.Run();
}

void Graph::UnmarkRPOVector()
{
    for (auto *block : rpoVector_) {
        block->SetMarker(false);
    }
}

std::vector<BasicBlock *> &Graph::GetRpoVector()
{
    return rpoVector_;
}

const std::vector<BasicBlock *> &Graph::GetRpoVector() const
{
    return rpoVector_;
}

}  // namespace compiler
