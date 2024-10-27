#include "analisys/dfs.h"
#include "ir/graph.h"

namespace compiler {

std::vector<BasicBlock *> DFS::Run()
{
    std::vector<BasicBlock *> dfsVector;

    DFSImpl(dfsVector, graph_->GetStartBlock());

    return dfsVector;
}

void DFS::DFSImpl(std::vector<BasicBlock *> &dfsVector, BasicBlock *block)
{
    assert(block);

    block->SetMarker(marker_);

    dfsVector.push_back(block);

    for (auto succBlock : block->GetSuccessors()) {
        if (!succBlock->IsMarked(marker_)) {
            DFSImpl(dfsVector, succBlock);
        }
    }
}

}  // namespace compiler
