#include "analysis/dfs.h"
#include <sstream>
#include "ir/graph.h"

namespace compiler {

std::vector<BasicBlock *> DFS::Run()
{
    auto *startBlock = graph_->GetStartBlock();
    assert(startBlock != nullptr);

    std::vector<BasicBlock *> dfsVector;
    dfsVector.reserve(graph_->GetBBCount());

    std::vector<std::pair<BasicBlock *, std::size_t>> paths;
    dfsVector.reserve(graph_->GetBBCount());

    auto visit = [&dfsVector, &paths, this](BasicBlock *block) {
        dfsVector.push_back(block);
        paths.emplace_back(block, 0U);
        block->SetMarker(marker_);
    };

    visit(startBlock);

    while (!paths.empty()) {
        auto &[block, neighborIdx] = paths.back();
        const auto &successors = block->GetSuccessors();
        if (neighborIdx == successors.size()) {
            paths.pop_back();
            continue;
        }
        auto *currBlock = successors[neighborIdx++];
        if (!currBlock->IsMarked(marker_)) {
            visit(currBlock);  // may reallocate: block/neighborIdx are invalid after this line
        }
    }

    return dfsVector;
}

}  // namespace compiler
