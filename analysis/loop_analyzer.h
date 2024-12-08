#ifndef ANALYSIS_LOOP_ANALYZER_H
#define ANALYSIS_LOOP_ANALYZER_H

#include "utils/macros.h"
#include "ir/marker.h"

#include <vector>

namespace compiler {

class Graph;
class Loop;
class BasicBlock;

class LoopAnalyzer final {
public:
    NO_COPY_SEMANTIC(LoopAnalyzer);
    NO_MOVE_SEMANTIC(LoopAnalyzer);

    LoopAnalyzer(Graph *graph) : graph_(graph) {}

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

#endif  // ANALYSIS_LOOP_ANALYZER_H
