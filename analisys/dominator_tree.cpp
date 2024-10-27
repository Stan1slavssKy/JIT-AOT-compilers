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

    for (auto it = rpoVec.begin() + 1; it < rpoVec.end(); ++it) {
        dominatorsMap_.emplace(*it, std::vector<BasicBlock *> {rootBlock});
    }

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

    for (auto blockIt = rpoVec.begin(); blockIt < rpoVec.end(); ++blockIt) {
        CalculateImmediateDominators(*blockIt);
    }
}

std::vector<BasicBlock *> DominatorTree::CalculateDominatedBlocks(BasicBlock *block,
                                                                  const std::vector<BasicBlock *> &originalVec,
                                                                  const std::vector<BasicBlock *> &reachableBlocks)
{
    assert(block);

    std::vector<BasicBlock *> dominatedBlocks;

    // + 1 to skip root block of the graph.
    for (auto originalVecIt = originalVec.begin(); originalVecIt < originalVec.end(); ++originalVecIt) {
        // std::cout << "Try to find " << static_cast<char>('A' + (*originalVecIt)->GetId()) << std::endl;
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

    return dominatedBlocks;
}

void DominatorTree::CalculateImmediateDominators(BasicBlock *block)
{
    std::vector<BasicBlock *> immediateDominators;

    for (auto dominatedBlockIt : block->GetDominatedBlocks()) {
        auto mapIt = dominatorsMap_.find(dominatedBlockIt);
        if (mapIt != dominatorsMap_.end()) {
            auto blocksDominatesOverCurrent = mapIt->second;
            auto it = std::find_if_not(blocksDominatesOverCurrent.begin(), blocksDominatesOverCurrent.end(),
                                       [block](auto domIt) { return domIt->IsDominatesOver(block); });
            if (it == blocksDominatesOverCurrent.end()) {
                immediateDominators.push_back(dominatedBlockIt);
            }
        } else {
            UNREACHABLE();
        }
    }

    std::cout << "Block = " << static_cast<char>('A' + block->GetId()) << " immediately dominates over: ";
    for (auto it : immediateDominators) {
        std::cout << static_cast<char>('A' + it->GetId()) << " ";
    }
    std::cout << std::endl;

    block->SetImmediateDominatedBlocks(std::move(immediateDominators));
}

}  // namespace compiler
