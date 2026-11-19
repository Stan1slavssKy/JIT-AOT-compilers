#ifndef ANALYSIS_DFS_H
#define ANALYSIS_DFS_H

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

    std::vector<BasicBlock *> RunPreorder();
    std::vector<BasicBlock *> RunRPO();

    struct DfsOrders {
        std::vector<BasicBlock *> preorder;
        std::vector<BasicBlock *> rpo;
    };
    DfsOrders Run();

    void SetMarker(Marker marker)
    {
        marker_ = marker;
    }

private:
    Graph *graph_ {nullptr};

    Marker marker_;
};

}  // namespace compiler

#endif  // ANALYSIS_DFS_H
