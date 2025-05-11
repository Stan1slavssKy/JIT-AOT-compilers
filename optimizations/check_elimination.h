#ifndef OPTIMIZATIONS_CHECK_ELIMINATION_H
#define OPTIMIZATIONS_CHECK_ELIMINATION_H

#include "utils/macros.h"
#include "ir/graph.h"

namespace compiler {

class CheckElimination final {
public:
    NO_COPY_SEMANTIC(CheckElimination);
    NO_MOVE_SEMANTIC(CheckElimination);

    CheckElimination(Graph *graph) : graph_(graph) {}
    ~CheckElimination() = default;

    void Run();

    void OptimizeDominatedChecks();

private:
    Graph *graph_ {nullptr};
};

}  // namespace compiler

#endif  // OPTIMIZATIONS_CHECK_ELIMINATION_H
