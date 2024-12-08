#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "analysis/loop_analyzer.h"
#include "ir/ir_builder.h"

#include <unordered_set>

namespace compiler::tests {

static void Link(BasicBlock *lhs, BasicBlock *rhs1, BasicBlock *rhs2 = nullptr)
{
    lhs->AddSuccessor(rhs1);
    rhs1->AddPredecessor(lhs);

    if (rhs2 != nullptr) {
        lhs->AddSuccessor(rhs2);
        rhs2->AddPredecessor(lhs);
    }
}

/*
    Graph:

        +-----------+
        V           |
        A-->B-->D-->E
            |
            V
            C
*/
TEST(LoopAnalyzer, TEST_1)
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

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

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
TEST(LoopAnalyzer, TEST_2)
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

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

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
TEST(LoopAnalyzer, TEST_3)
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

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(h));

    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 1);
    auto *aLoop = a->GetLoop();
    auto *bLoop = b->GetLoop();

    ASSERT_EQ(aLoop->GetOuterLoop(), rootLoop);
    ASSERT_EQ(bLoop->GetOuterLoop(), aLoop);
    ASSERT_EQ(aLoop->GetInnerLoops().size(), 1);
    ASSERT_EQ(aLoop->GetInnerLoops()[0], bLoop);
    ASSERT_EQ(bLoop->GetInnerLoops().size(), 0);

    ASSERT_THAT(aLoop->GetBlocks(), ::testing::UnorderedElementsAre(a, f));
    ASSERT_THAT(bLoop->GetBlocks(), ::testing::UnorderedElementsAre(b, c, d, e, g));
}

/*
    Graph:

    A--->B--->F--->G
         |    |    |
         |    v    |
         |    E    |
         |    |    |
         v    v    |
         C--->D<---+

*/

TEST(LoopAnalyzer, TEST_4)
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

    Link(a, b);
    Link(b, f, c);
    Link(c, d);
    Link(e, d);
    Link(f, g, e);
    Link(g, d);

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(a, b, c, d, e, f, g));
    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 0);
}

/*
    Graph:

          J-----+
          ^     |
          |     v
    A---->B---->C---->D---->E---->F---->G---->I---->K
          ^     ^     |     ^     |     |
          |     |     |     |     |     |
          |     +-----+     +-----+     |
          |                             v
          +-----------------------------H
*/

TEST(LoopAnalyzer, TEST_5)
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
    auto *i = builder.CreateBB();
    auto *j = builder.CreateBB();
    auto *k = builder.CreateBB();

    Link(a, b);
    Link(b, j, c);
    Link(c, d);
    Link(d, c, e);
    Link(e, f);
    Link(f, g, e);
    Link(g, i, h);
    Link(h, b);
    Link(i, k);
    Link(j, c);

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(a, i, k));
    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 1);

    auto *bLoop = b->GetLoop();
    auto *cLoop = c->GetLoop();
    auto *eLoop = e->GetLoop();
    ASSERT_EQ(bLoop->GetHeader(), b);
    ASSERT_EQ(cLoop->GetHeader(), c);
    ASSERT_EQ(eLoop->GetHeader(), e);

    ASSERT_EQ(bLoop->GetLatches().size(), 1);
    ASSERT_EQ(cLoop->GetLatches().size(), 1);
    ASSERT_EQ(eLoop->GetLatches().size(), 1);
    ASSERT_EQ(bLoop->GetLatches()[0], h);
    ASSERT_EQ(cLoop->GetLatches()[0], d);
    ASSERT_EQ(eLoop->GetLatches()[0], f);

    ASSERT_EQ(rootLoop->GetInnerLoops()[0], bLoop);
    ASSERT_EQ(bLoop->GetInnerLoops().size(), 2);
    ASSERT_EQ(cLoop->GetInnerLoops().size(), 0);
    ASSERT_EQ(eLoop->GetInnerLoops().size(), 0);

    ASSERT_EQ(cLoop->GetOuterLoop(), bLoop);
    ASSERT_EQ(eLoop->GetOuterLoop(), bLoop);

    ASSERT_THAT(bLoop->GetInnerLoops(), ::testing::UnorderedElementsAre(cLoop, eLoop));

    ASSERT_THAT(bLoop->GetBlocks(), ::testing::UnorderedElementsAre(b, g, h, j));
    ASSERT_THAT(cLoop->GetBlocks(), ::testing::UnorderedElementsAre(c, d));
    ASSERT_THAT(eLoop->GetBlocks(), ::testing::UnorderedElementsAre(e, f));
}

/*
    Graph:
            A      +--------------------+
            |      |                    |
            v      v                    |
      +---->B----->C----->D------------>G----->I
      |     |             ^             ^      ^
      |     |             |             |      |
      |     +------------>E----->F----->H------+
      |                          |
      +--------------------------+

*/
TEST(LoopAnalyzer, TEST_6)
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
    auto *i = builder.CreateBB();

    Link(a, b);
    Link(b, c, e);
    Link(c, d);
    Link(d, g);
    Link(e, f, d);
    Link(f, h, b);
    Link(g, c, i);
    Link(h, g, i);

    LoopAnalyzer loopAnalyzer(&graph);
    loopAnalyzer.Run();

    auto *rootLoop = graph.GetRootLoop();
    ASSERT_EQ(rootLoop->GetHeader(), nullptr);
    ASSERT_THAT(rootLoop->GetBlocks(), ::testing::UnorderedElementsAre(a, d, h, i));
    ASSERT_EQ(rootLoop->GetInnerLoops().size(), 2);

    auto *bLoop = b->GetLoop();
    auto *cLoop = c->GetLoop();
    ASSERT_EQ(bLoop->GetHeader(), b);
    ASSERT_EQ(cLoop->GetHeader(), c);

    ASSERT_EQ(bLoop->GetLatches().size(), 1);
    ASSERT_EQ(cLoop->GetLatches().size(), 1);
    ASSERT_EQ(bLoop->GetLatches()[0], f);
    ASSERT_EQ(cLoop->GetLatches()[0], g);

    ASSERT_TRUE(bLoop->IsReducible());
    ASSERT_FALSE(cLoop->IsReducible());

    ASSERT_EQ(bLoop->GetInnerLoops().size(), 0);
    ASSERT_EQ(cLoop->GetInnerLoops().size(), 0);

    ASSERT_THAT(rootLoop->GetInnerLoops(), ::testing::UnorderedElementsAre(bLoop, cLoop));
    ASSERT_THAT(bLoop->GetBlocks(), ::testing::UnorderedElementsAre(b, e, f));
    ASSERT_THAT(cLoop->GetBlocks(), ::testing::UnorderedElementsAre(c, g));

    ASSERT_EQ(bLoop->GetOuterLoop(), rootLoop);
    ASSERT_EQ(cLoop->GetOuterLoop(), rootLoop);
}

}  // namespace compiler::tests
