#ifndef ANALISYS_DFS_H
#define ANALISYS_DFS_H

#include "utils/macros.h"
#include "ir/basic_block.h"

#include <vector>

namespace compiler {

class Graph;

class RPO final {
public:
    NO_COPY_SEMANTIC(RPO);
    NO_MOVE_SEMANTIC(RPO);

    RPO(Graph *graph) : graph_(graph) {}

    std::vector<BasicBlock *> Run();

    void SetMarker(Marker marker)
    {
        marker_ = marker;
    }

private:
    void DFS(std::vector<BasicBlock *> &rpoVector, BasicBlock *block, size_t *blockCount);

private:
    Graph *graph_ {nullptr};

    Marker marker_ {true};
};

}  // namespace compiler

#endif  // ANALISYS_DFS_H