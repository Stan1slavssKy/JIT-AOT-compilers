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

}  // namespace compiler
