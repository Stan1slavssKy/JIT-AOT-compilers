#ifndef OPTIMIZATIONS_PEEPHOLES_H
#define OPTIMIZATIONS_PEEPHOLES_H

#include "ir/graph.h"

namespace compiler {

class Inlining final {
public:
    NO_COPY_SEMANTIC(Inlining);
    NO_MOVE_SEMANTIC(Inlining);

    Inlining(Graph *graph) : graph_(graph) {}
    ~Inlining() = default;

    void Run();

private:
    Graph *graph_ {nullptr};
};

}  // namespace compiler

#endif  // OPTIMIZATIONS_PEEPHOLES_H
