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

        (*blockIt)->SetDominatedBlocks(CalculateDominatedBlocks(*blockIt, rpoVec, reachableBlocks));

        for (auto i : (*blockIt)->GetDominatedBlocks()) {
            std::cout << static_cast<char>('A' + (i)->GetId()) << std::endl;
        }

        UmmarkVector(reachableBlocks);
        (*blockIt)->SetMarker(false);
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
        // std::cout << "[CalculateDominatedBlocks]" << std::endl;
        auto dominatedBlock = std::find_if_not(reachableBlocks.begin() + 1, reachableBlocks.end(),
                                               [&originalVecIt](auto reachableBlocksIt) -> bool {
                                                   return reachableBlocksIt->GetId() == (*originalVecIt)->GetId();
                                               });

        if (dominatedBlock != reachableBlocks.end() && *dominatedBlock != block) {
            dominatedBlocks.push_back(*dominatedBlock);
            auto mapIt = dominatorsMap_.find(*dominatedBlock);
            if (mapIt == dominatorsMap_.end()) {
                dominatorsMap_.emplace(*dominatedBlock, std::vector<BasicBlock *>{block});
            } else {
                mapIt->second.push_back(*dominatedBlock);
            }
        }
    }

    return dominatedBlocks;
}

}  // namespace compiler
