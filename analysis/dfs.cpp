#include "analysis/dfs.h"
#include "ir/graph.h"

#include <algorithm>

namespace compiler {

template <typename PreCb, typename PostCb>
static void Traverse(Graph *graph, Marker marker, PreCb &&onPre, PostCb &&onPost)
{
    auto *startBlock = graph->GetStartBlock();
    assert(startBlock != nullptr);

    std::vector<std::pair<BasicBlock *, std::size_t>> paths;
    paths.reserve(graph->GetBBCount());

    auto enter = [&](BasicBlock *block) {
        onPre(block);
        paths.emplace_back(block, 0U);
        block->SetMarker(marker);
    };

    enter(startBlock);

    while (!paths.empty()) {
        auto &[block, neighborIdx] = paths.back();
        const auto &successors = block->GetSuccessors();
        if (neighborIdx == successors.size()) {
            onPost(block);
            paths.pop_back();
            continue;
        }
        auto *currBlock = successors[neighborIdx++];
        if (!currBlock->IsMarked(marker)) {
            enter(currBlock);  // may reallocate: block/neighborIdx are invalid after this line
        }
    }
}

DFS::DfsOrders DFS::Run()
{
    std::size_t bbCnt = graph_->GetBBCount();

    std::vector<BasicBlock *> preorder;
    std::vector<BasicBlock *> rpo;

    preorder.reserve(bbCnt);
    rpo.reserve(bbCnt);

    Traverse<>(
        graph_, marker_, [&preorder](BasicBlock *block) { preorder.push_back(block); },
        [&rpo](BasicBlock *block) { rpo.push_back(block); });

    std::reverse(rpo.begin(), rpo.end());

    return {std::move(preorder), std::move(rpo)};
}

std::vector<BasicBlock *> DFS::RunPreorder()
{
    std::vector<BasicBlock *> preorder;
    preorder.reserve(graph_->GetBBCount());

    Traverse<>(
        graph_, marker_, [&preorder](BasicBlock *block) { preorder.push_back(block); }, [](BasicBlock *) {});

    return preorder;
}

std::vector<BasicBlock *> DFS::RunRPO()
{
    std::vector<BasicBlock *> rpo;
    rpo.reserve(graph_->GetBBCount());

    Traverse<>(
        graph_, marker_, [](BasicBlock *) {}, [&rpo](BasicBlock *block) { rpo.push_back(block); });

    std::reverse(rpo.begin(), rpo.end());

    return rpo;
}

}  // namespace compiler
