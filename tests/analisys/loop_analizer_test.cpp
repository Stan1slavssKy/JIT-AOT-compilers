#include <gtest/gtest.h>

#include "analisys/loop_analizer.h"
#include "ir/ir_builder.h"

namespace compiler::tests {
#if 0
/*
        +-----------+
        V           |
        A-->B-->D-->E
            |
            V
            C
*/
TEST(LoopAnalizer, TEST_1)
{
    Graph graph;
    IrBuilder builder(&graph);

    auto *a = builder.CreateBB();
    auto *b = builder.CreateBB();
    auto *c = builder.CreateBB();
    auto *d = builder.CreateBB();
    auto *e = builder.CreateBB();

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    b->AddSuccessor(d);
    d->AddSuccessor(e);
    e->AddSuccessor(a);

    LoopAnalizer loopAnalalizer(&graph);
    loopAnalalizer.Run();

    auto *roopLoop = graph.GetRootLoop();
    ASSERT_EQ(roopLoop->GetInnerLoops().size(), 1);
}

/*
    Graph:
      A
      |
      B
    /   \
   C     F
   \    / \
    \  E   G
     \ |  /
       D
*/
TEST(LoopAnalizer, TEST_2)
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

    LoopAnalizer loopAnalalizer(&graph);
    loopAnalalizer.Run();
}
#endif
}  // namespace compiler::tests
