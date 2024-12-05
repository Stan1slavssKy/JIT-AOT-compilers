#include "analisys/dominator_tree.h"
#include "analisys/dfs.h"
#include "ir/graph.h"

#include <algorithm>
#include <iostream>

namespace compiler {

void DominatorTree::Build()
{
    graph_->RunRpo();
    auto &rpoVec = graph_->GetRpoVector();

    auto *rootBlock = graph_->GetStartBlock();
    rootBlock->SetDominatedBlocks(rpoVec);

    for (auto it = rpoVec.begin() + 1; it < rpoVec.end(); ++it) {
        dominatorsMap_.emplace(*it, std::vector<BasicBlock *> {rootBlock});
    }

    DFS dfs(graph_);

    for (auto blockIt = rpoVec.begin() + 1; blockIt < rpoVec.end(); ++blockIt) {
        auto *block = *blockIt;
        auto marker = graph_->CreateNewMarker();
        dfs.SetMarker(marker);
        block->SetMarker(marker);  // mark block to remove it from DFS bypass.

        auto reachableBlocks = dfs.Run();

        CalculateDominatedBlocks(block, rpoVec, reachableBlocks);

        graph_->EraseMarker(marker);
    }

    for (auto blockIt = rpoVec.begin(); blockIt < rpoVec.end(); ++blockIt) {
        CalculateImmediateDominators(*blockIt);
    }
}

void DominatorTree::CalculateDominatedBlocks(BasicBlock *block, const std::vector<BasicBlock *> &originalVec,
                                             const std::vector<BasicBlock *> &reachableBlocks)
{
    assert(block);

    std::vector<BasicBlock *> dominatedBlocks;

    for (auto originalVecIt = originalVec.begin(); originalVecIt < originalVec.end(); ++originalVecIt) {
        auto dominatedBlock = std::find_if(
            reachableBlocks.begin(), reachableBlocks.end(),
            [&originalVecIt](auto reachableBlocksIt) -> bool { return reachableBlocksIt == *originalVecIt; });

        if (dominatedBlock == reachableBlocks.end() && *originalVecIt != block) {
            dominatedBlocks.push_back(*originalVecIt);
            auto mapIt = dominatorsMap_.find(*originalVecIt);
            if (mapIt != dominatorsMap_.end()) {
                mapIt->second.push_back(block);
            } else {
                UNREACHABLE();
            }
        }
    }

    block->SetDominatedBlocks(std::move(dominatedBlocks));
}

void DominatorTree::CalculateImmediateDominators(BasicBlock *block)
{
    assert(block);

    for (auto dominatedBlockIt : block->GetDominatedBlocks()) {
        auto mapIt = dominatorsMap_.find(dominatedBlockIt);
        if (mapIt != dominatorsMap_.end()) {
            auto blocksDominatesOverCurrent = mapIt->second;
            auto it = std::find_if_not(blocksDominatesOverCurrent.begin(), blocksDominatesOverCurrent.end(),
                                       [block](auto domIt) { return domIt->IsDominatesOver(block); });
            if (it == blocksDominatesOverCurrent.end()) {
                dominatedBlockIt->SetImmediateDominator(block);
            }
        } else {
            UNREACHABLE();
        }
    }
}

}  // namespace compiler
