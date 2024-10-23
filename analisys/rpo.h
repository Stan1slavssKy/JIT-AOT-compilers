#ifndef ANALISYS_DFS_H
#define ANALISYS_DFS_H

#include "utils/macros.h"
#include "ir/basic_block.h"

#include <vector>

namespace compiler {

class Graph;
class BasicBlock;

class RPO final {
public:
    NO_COPY_SEMANTIC(RPO);
    NO_MOVE_SEMANTIC(RPO);

    RPO(Graph *graph) : graph_(graph) {}

    const std::vector<BasicBlock *> &GetRpoBlocks() const
    {
        return rpoVector_;
    }

    void Run();

private:
    void DFS(BasicBlock *block, size_t *blockCount);

private:
    Graph *graph_ {nullptr};

    Marker marker_ {true};

    std::vector<BasicBlock *> rpoVector_;
};

}  // namespace compiler

#endif  // ANALISYS_DFS_H
