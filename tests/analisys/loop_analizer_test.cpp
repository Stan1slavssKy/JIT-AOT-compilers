#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "analisys/loop_analizer.h"
#include "ir/ir_builder.h"

#include <unordered_set>

namespace compiler::tests {

/*
    Graph:

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

    a->AddPredecessor(e);
    b->AddPredecessor(a);
    d->AddPredecessor(b);
    e->AddPredecessor(d);
    c->AddPredecessor(b);

    LoopAnalizer loopAnalalizer(&graph);
    loopAnalalizer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(c));

    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 1);
    auto *loop = rootLoop->GetInnerLoops()[0];
    ASSERT_EQ(loop->GetHeader(), a);
    ASSERT_EQ(loop->GetLatches().size(), 1);
    ASSERT_EQ(loop->GetLatches()[0], e);
    ASSERT_THAT(loop->GetBlocks(), ::testing::UnorderedElementsAre(a, b, d, e));
}

/*
    Graph:

    A--->B<-----------+
         |            |
         V            |
         C------>D--->E
         |       |
         +-->F<--+

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

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    c->AddSuccessor(d);
    c->AddSuccessor(f);
    d->AddSuccessor(e);
    d->AddSuccessor(f);
    e->AddSuccessor(b);

    b->AddPredecessor(a);
    c->AddPredecessor(b);
    d->AddPredecessor(c);
    f->AddPredecessor(c);
    f->AddPredecessor(d);
    e->AddPredecessor(d);
    b->AddPredecessor(e);

    LoopAnalizer loopAnalalizer(&graph);
    loopAnalalizer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(a, f));

    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 1);
    auto *loop = rootLoop->GetInnerLoops()[0];
    ASSERT_EQ(loop->GetHeader(), b);
    ASSERT_EQ(loop->GetLatches().size(), 1);
    ASSERT_EQ(loop->GetLatches()[0], e);
    ASSERT_THAT(loop->GetBlocks(), ::testing::UnorderedElementsAre(b, c, d, e));
}

/*
    Graph:

    +------------------------+
    |    +--------------+    |
    |    |              |    |
    v    v              |    |
    A--->B--->C--->D--->E--->F
         |    ^
         v    |
         G----+
         |
         v
         H
*/
TEST(LoopAnalizer, TEST_3)
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
    auto *h = builder.CreateBB();

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    c->AddSuccessor(d);
    d->AddSuccessor(e);
    e->AddSuccessor(f);
    f->AddSuccessor(a);
    e->AddSuccessor(b);
    b->AddSuccessor(g);
    g->AddSuccessor(h);
    g->AddSuccessor(c);

    a->AddPredecessor(f);
    b->AddPredecessor(a);
    c->AddPredecessor(b);
    d->AddPredecessor(c);
    e->AddPredecessor(d);
    f->AddPredecessor(e);
    b->AddPredecessor(e);
    g->AddPredecessor(b);
    h->AddPredecessor(g);
    c->AddPredecessor(g);

    LoopAnalizer loopAnalalizer(&graph);
    loopAnalalizer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(h));

    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 1);
    // auto *loop = rootLoop->GetInnerLoops()[0];
    // ASSERT_EQ(loop->GetHeader(), b);
    // ASSERT_EQ(loop->GetLatches().size(), 1);
    // ASSERT_EQ(loop->GetLatches()[0], e);
    // ASSERT_THAT(loop->GetBlocks(), ::testing::UnorderedElementsAre(b, c, d, e));
}

}  // namespace compiler::tests
