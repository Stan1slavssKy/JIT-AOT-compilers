#ifndef ANALISYS_LOOP_ANALIZER_H
#define ANALISYS_LOOP_ANALIZER_H

#include "utils/macros.h"
#include "ir/marker.h"

#include <vector>

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
    void PopulateLoops();
    void BuildLoopTree();

    void SearchLatchDFS(BasicBlock *block);
    void ProcessNewLatch(BasicBlock *header, BasicBlock *latch);

    void ProcessReducibleLoopHeader(Loop *loop, BasicBlock *header);
    void ProcessIrreducibleLoopHeader(Loop *loop);

    void LoopSearchDFS(Loop *loop, BasicBlock *block);

private:
    Graph *graph_ {nullptr};

    Loop *rootLoop_ {nullptr};

    Marker blackMrk_;
    Marker grayMrk_;
};

}  // namespace compiler

#endif  // ANALISYS_LOOP_ANALIZER_H
