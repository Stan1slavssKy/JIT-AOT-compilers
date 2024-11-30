#ifndef ANALISYS_LOOP_ANALIZER_H
#define ANALISYS_LOOP_ANALIZER_H

#include "utils/macros.h"

namespace compiler {

class Graph;
class Loop;
class BasicBlock;

class LoopAnalizer final {
public:
    NO_COPY_SEMANTIC(LoopAnalizer);
    NO_MOVE_SEMANTIC(LoopAnalizer);

    LoopAnalizer(Graph *graph) : graph_(graph) {}

    void Run();

private:
    void CreateRootLoop();

    void CollectLatches();

    void SearchLatch(BasicBlock *block);

private:
    Graph *graph_ {nullptr};

    Loop *rootLoop_ {nullptr};
};

}  // namespace compiler

#endif  // ANALISYS_LOOP_ANALIZER_H
