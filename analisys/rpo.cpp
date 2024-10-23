#include "analisys/rpo.h"
#include "ir/graph.h"

namespace compiler {

void RPO::Run()
{
    size_t blockCount = graph_->GetAlivBlockCount();
    rpoVector_.resize(blockCount);
    DFS(graph_->GetStartBlock(), &blockCount);
}

void RPO::DFS(BasicBlock *block, size_t *blockCount)
{
    assert(block);

    block->SetMarker(marker_);

    for (auto succBlock : block->GetSuccessors()) {
        if (!succBlock->IsMarked(marker_)) {
            DFS(succBlock, blockCount);
        }
    }

    assert(blockCount != nullptr);
    assert(*blockCount > 0);

    rpoVector_[--(*blockCount)] = block;
}

}  // namespace compiler
