#ifndef ANALISYS_DFS_H
#define ANALISYS_DFS_H

#include "utils/macros.h"
#include "ir/basic_block.h"
#include "ir/marker.h"

#include <vector>

namespace compiler {

class Graph;

class DFS final {
public:
    NO_COPY_SEMANTIC(DFS);
    NO_MOVE_SEMANTIC(DFS);

    DFS(Graph *graph) : graph_(graph) {}

    std::vector<BasicBlock *> Run();

    void SetMarker(Marker marker)
    {
        marker_ = marker;
    }

private:
    void DFSImpl(std::vector<BasicBlock *> &dfsVector, BasicBlock *block);

private:
    Graph *graph_ {nullptr};

    Marker marker_;
};

}  // namespace compiler

#endif  // ANALISYS_DFS_H
