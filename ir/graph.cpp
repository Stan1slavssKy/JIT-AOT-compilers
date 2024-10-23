#include "ir/graph.h"

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

size_t Graph::GetAlivBlockCount() const
{
    return basicBlocks_.size();
}

}  // namespace compiler
