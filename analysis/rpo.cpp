#include "analysis/rpo.h"
#include "ir/graph.h"

namespace compiler {

std::vector<BasicBlock *> RPO::Run()
{
    size_t blockCount = graph_->GetAliveBlockCount();
    std::vector<BasicBlock *> rpoVector(blockCount);

    DFS(rpoVector, graph_->GetStartBlock(), &blockCount);

    return rpoVector;
}

void RPO::DFS(std::vector<BasicBlock *> &rpoVector, BasicBlock *block, size_t *blockCount)
{
    assert(block);

    block->SetMarker(marker_);

    for (auto succBlock : block->GetSuccessors()) {
        if (!succBlock->IsMarked(marker_)) {
            DFS(rpoVector, succBlock, blockCount);
        }
    }

    assert(blockCount != nullptr);
    assert(*blockCount > 0);

    rpoVector[--(*blockCount)] = block;
}

}  // namespace compiler
