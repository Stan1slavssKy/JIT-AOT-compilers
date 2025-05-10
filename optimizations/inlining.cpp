#include "analysis/dominator_tree.h"
#include "optimizations/inlining.h"

namespace compiler {

void Inlining::Run()
{
    // DominatorTree tree(graph_);
    // tree.Build();

    // auto &blocks = graph_->GetRpoVector();
    // for (auto *block: blocks) {
    //     block->EnumerateInsns([](Instruction *currInsn) {
    //         if (currInsn->GetOpcode() == Opcode::CALLSTATIC && static_cast<>(currInsn)->) {

    //         }
    //     });
    // }
}

}  // namespace compiler
