#ifndef ANALISYS_DOMINATOR_TREE_H
#define ANALISYS_DOMINATOR_TREE_H

#include "utils/macros.h"

#include <vector>
#include <unordered_map>

namespace compiler {

class Graph;
class BasicBlock;

class DominatorTree final {
public:
    NO_COPY_SEMANTIC(DominatorTree);
    NO_MOVE_SEMANTIC(DominatorTree);

    DominatorTree(Graph *graph) : graph_(graph) {}

    void Build();

private:
    std::vector<BasicBlock *> CalculateDominatedBlocks(BasicBlock *block, const std::vector<BasicBlock *> &originalVec,
                                                       const std::vector<BasicBlock *> &reachableBlocks);

    void CalculateImmediateDominators(BasicBlock *block);

private:
    Graph *graph_ {nullptr};

    // <block, blocks that dominated on key block>
    std::unordered_map<BasicBlock *, std::vector<BasicBlock *>> dominatorsMap_;
};

}  // namespace compiler

#endif  // ANALISYS_DOMINATOR_TREE_H
