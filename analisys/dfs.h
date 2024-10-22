#ifndef ANALISYS_DFS_H
#define ANALISYS_DFS_H

#include "utils/macros.h"
#include "ir/basic_block.h"

#include <vector>

namespace compiler {

class Graph;
class BasicBlock;

class DFS final {
public:
    NO_COPY_SEMANTIC(DFS);
    NO_MOVE_SEMANTIC(DFS);

    DFS(Graph *graph) : graph_(graph) {}

    const std::vector<BasicBlock *> &GetDfsBlocks() const
    {
        return dfsVector_;
    }

    void Run();

private:
    void DFSImpl(BasicBlock *block);

private:
    Graph *graph_ {nullptr};

    Marker marker_ {true};

    std::vector<BasicBlock *> dfsVector_;
};

}  // namespace compiler

#endif  // ANALISYS_DFS_H
