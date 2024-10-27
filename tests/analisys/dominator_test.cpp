#include <gtest/gtest.h>

#include "analisys/dominator_tree.h"
#include "ir/ir_builder.h"

namespace compiler::tests {

TEST(DominatorTree, TEST_1)
{
    Graph graph;
    IrBuilder builder(&graph);

    auto *a = builder.CreateBB();
    auto *b = builder.CreateBB();
    auto *c = builder.CreateBB();
    auto *d = builder.CreateBB();
    auto *e = builder.CreateBB();
    auto *f = builder.CreateBB();
    auto *g = builder.CreateBB();

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    b->AddSuccessor(f);
    c->AddSuccessor(d);
    f->AddSuccessor(e);
    f->AddSuccessor(g);
    g->AddSuccessor(d);
    e->AddSuccessor(d);

    DominatorTree tree(&graph);
    tree.Build();
}

} // namespace compiler::tests
