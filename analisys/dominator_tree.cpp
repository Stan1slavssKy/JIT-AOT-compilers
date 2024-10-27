#include "analisys/dominator_tree.h"
#include "analisys/dfs.h"
#include "ir/graph.h"

#include <algorithm>
#include <iostream>

namespace compiler {

namespace {

void UmmarkVector(const std::vector<BasicBlock *> &vector)
{
    for (auto *block : vector) {
        block->SetMarker(false);
    }
}

}  // namespace

void DominatorTree::Build()
{
    graph_->RunRpo();
    graph_->UnmarkRPOVector();
    auto &rpoVec = graph_->GetRpoVector();

    auto *rootBlock = graph_->GetStartBlock();
    rootBlock->SetDominatedBlocks(rpoVec);  // todo: remove excessive copying

    DFS dfs(graph_);

    for (auto blockIt = rpoVec.begin() + 1; blockIt < rpoVec.end(); ++blockIt) {
        std::cout << "Run through block " << static_cast<char>('A' + (*blockIt)->GetId()) << std::endl;
        (*blockIt)->SetMarker(true);  // mark block to remove it from RPO bypass.

        auto reachableBlocks = dfs.Run();

        std::cout << "Reachable blocks: ";
        for (auto i : reachableBlocks) {
            std::cout << static_cast<char>('A' + (i)->GetId()) << " ";
        }
        std::cout << std::endl;

        (*blockIt)->SetDominatedBlocks(CalculateDominatedBlocks(*blockIt, rpoVec, reachableBlocks));

        std::cout << "Dominated blocks: ";
        for (auto i : (*blockIt)->GetDominatedBlocks()) {
            std::cout << static_cast<char>('A' + (i)->GetId()) << " ";
        }
        std::cout << std::endl;

        UmmarkVector(reachableBlocks);
        (*blockIt)->SetMarker(false);
        std::cout << "==========================" << std::endl;
    }

    for (auto i : dominatorsMap_) {
        std::cout << "Block = " << static_cast<char>('A' + i.first->GetId()) << " dominated by: ";
        for (auto it : i.second) {
            std::cout << static_cast<char>('A' + it->GetId()) << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<BasicBlock *> DominatorTree::CalculateDominatedBlocks(BasicBlock *block,
                                                                  const std::vector<BasicBlock *> &originalVec,
                                                                  const std::vector<BasicBlock *> &reachableBlocks)
{
    assert(block);

    std::vector<BasicBlock *> dominatedBlocks;

    // + 1 to skip root block of the graph.
    for (auto originalVecIt = originalVec.begin() + 1; originalVecIt < originalVec.end(); ++originalVecIt) {
        // std::cout << "Try to find " << static_cast<char>('A' + (*originalVecIt)->GetId()) << std::endl;
        auto dominatedBlock = std::find_if(
            reachableBlocks.begin() + 1, reachableBlocks.end(),
            [&originalVecIt](auto reachableBlocksIt) -> bool { return reachableBlocksIt == *originalVecIt; });

        if (dominatedBlock == reachableBlocks.end() && *originalVecIt != block) {
            dominatedBlocks.push_back(*originalVecIt);
            auto mapIt = dominatorsMap_.find(*originalVecIt);
            if (mapIt == dominatorsMap_.end()) {
                dominatorsMap_.emplace(*originalVecIt, std::vector<BasicBlock *> {block});
            } else {
                mapIt->second.push_back(block);
            }
        }
    }

    return dominatedBlocks;
}

}  // namespace compiler
