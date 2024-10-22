#include "analisys/dfs.h"
#include "ir/graph.h"

namespace compiler {

void DFS::Run()
{
    DFSImpl(graph_->GetStartBlock());
}

void DFS::DFSImpl(BasicBlock *block)
{
    assert(block);

    block->SetMarker(marker_);
    dfsVector_.push_back(block);

    for (auto succBlock : block->GetSuccessors()) {
        if (!succBlock->IsMarked(marker_)) {
            DFSImpl(succBlock);
        }
    }
}

}  // namespace compiler
