#include <gtest/gtest.h>

#include "analisys/dominator_tree.h"
#include "ir/ir_builder.h"

namespace compiler::tests {

/*
    Graph:    Dominator Tree:
      A             A
      |             |
      B             B
    /   \         / | \
   C     F       C  F  D
   \    / \        / \
    \  E   G      E   G
     \ |  /
       D
*/
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

    auto &aImmDom = a->GetImmediateDominatedBlocks();
    ASSERT_EQ(aImmDom.size(), 1);
    ASSERT_EQ(aImmDom[0], b);

    auto &bImmDom = b->GetImmediateDominatedBlocks();
    ASSERT_EQ(bImmDom.size(), 3);

    std::set<BasicBlock *> expectedBImmDom = {c, f, d};
    for (auto it : bImmDom) {
        auto findIt = expectedBImmDom.find(it);
        ASSERT_FALSE(findIt == expectedBImmDom.end());
    }

    auto &cImmDom = c->GetImmediateDominatedBlocks();
    ASSERT_EQ(cImmDom.size(), 0);

    auto &dImmDom = d->GetImmediateDominatedBlocks();
    ASSERT_EQ(dImmDom.size(), 0);

    auto &fImmDom = f->GetImmediateDominatedBlocks();
    ASSERT_EQ(fImmDom.size(), 2);

    std::set<BasicBlock *> expectedFImmDom = {e, g};
    for (auto it : fImmDom) {
        auto findIt = expectedFImmDom.find(it);
        ASSERT_FALSE(findIt == expectedFImmDom.end());
    }
}

/*
    Graph:
          J ----\
          ^     |
          |     v
    A --> B --> C --> D --> E --> F --> G --> I --> K
          ^     ^     |     ^     |     |
          |     |     |     |     |     |
          |     \-----/     \-----/     |
          |                             v
          \---------------------------- H

    Dominator tree:

    A --> B --> C --> D --> E --> F --> G --> I --> K
          |                             |
          v                             v
          J                             H
*/
TEST(DominatorTree, TEST_2)
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

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    b->AddSuccessor(j);
    j->AddSuccessor(c);
    c->AddSuccessor(d);
    d->AddSuccessor(c);
    d->AddSuccessor(e);
    e->AddSuccessor(f);
    f->AddSuccessor(e);
    f->AddSuccessor(g);
    g->AddSuccessor(i);
    g->AddSuccessor(h);
    h->AddSuccessor(b);
    i->AddSuccessor(k);

    DominatorTree tree(&graph);
    tree.Build();

    auto &aImmDom = a->GetImmediateDominatedBlocks();
    ASSERT_EQ(aImmDom.size(), 1);
    ASSERT_EQ(aImmDom[0], b);

    auto &bImmDom = b->GetImmediateDominatedBlocks();
    ASSERT_EQ(bImmDom.size(), 2);

    std::set<BasicBlock *> expectedBImmDom = {c, j};
    for (auto it : bImmDom) {
        auto findIt = expectedBImmDom.find(it);
        ASSERT_FALSE(findIt == expectedBImmDom.end());
    }

    auto &jImmDom = j->GetImmediateDominatedBlocks();
    ASSERT_EQ(jImmDom.size(), 0);

    auto &cImmDom = c->GetImmediateDominatedBlocks();
    ASSERT_EQ(cImmDom.size(), 1);
    ASSERT_EQ(cImmDom[0], d);

    auto &dImmDom = d->GetImmediateDominatedBlocks();
    ASSERT_EQ(dImmDom.size(), 1);
    ASSERT_EQ(dImmDom[0], e);

    auto &eImmDom = e->GetImmediateDominatedBlocks();
    ASSERT_EQ(eImmDom.size(), 1);
    ASSERT_EQ(eImmDom[0], f);

    auto &fImmDom = f->GetImmediateDominatedBlocks();
    ASSERT_EQ(fImmDom.size(), 1);
    ASSERT_EQ(fImmDom[0], g);

    auto &gImmDom = g->GetImmediateDominatedBlocks();
    ASSERT_EQ(gImmDom.size(), 2);

    std::set<BasicBlock *> expectedGImmDom = {h, i};
    for (auto it : gImmDom) {
        auto findIt = expectedGImmDom.find(it);
        ASSERT_FALSE(findIt == expectedGImmDom.end());
    }

    auto &iImmDom = i->GetImmediateDominatedBlocks();
    ASSERT_EQ(iImmDom.size(), 1);
    ASSERT_EQ(iImmDom[0], k);

    auto &kImmDom = k->GetImmediateDominatedBlocks();
    ASSERT_EQ(kImmDom.size(), 0);

    auto &hImmDom = h->GetImmediateDominatedBlocks();
    ASSERT_EQ(hImmDom.size(), 0);
}

/*
    Graph:
            A      /--------------------\
            |      |                    |
            v      v                    |
      /---> B ---> C ---> D ----------> G ---> I
      |     |             ^             ^      ^
      |     |             |             |      |
      |     \-----------> E ---> F ---> H -----/
      |                          |
      \--------------------------/

    Dominator tree:

             /---> D
            /---> C
    A ---> B ---> E ---> F ---> H
            \---> G
             \---> I
*/
TEST(DominatorTree, TEST_3)
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

    a->AddSuccessor(b);

    b->AddSuccessor(c);
    b->AddSuccessor(e);

    c->AddSuccessor(d);

    e->AddSuccessor(d);
    e->AddSuccessor(f);

    d->AddSuccessor(g);

    f->AddSuccessor(b);
    f->AddSuccessor(h);

    h->AddSuccessor(i);
    h->AddSuccessor(g);

    g->AddSuccessor(i);
    g->AddSuccessor(c);

    DominatorTree tree(&graph);
    tree.Build();

    auto &aImmDom = a->GetImmediateDominatedBlocks();
    ASSERT_EQ(aImmDom.size(), 1);
    ASSERT_EQ(aImmDom[0], b);

    auto &bImmDom = b->GetImmediateDominatedBlocks();
    ASSERT_EQ(bImmDom.size(), 5);

    std::set<BasicBlock *> expectedBImmDom = {i, g, c, d, e};
    for (auto it : bImmDom) {
        auto findIt = expectedBImmDom.find(it);
        ASSERT_FALSE(findIt == expectedBImmDom.end());
    }

    auto &eImmDom = e->GetImmediateDominatedBlocks();
    ASSERT_EQ(eImmDom.size(), 1);
    ASSERT_EQ(eImmDom[0], f);

    auto &fImmDom = f->GetImmediateDominatedBlocks();
    ASSERT_EQ(fImmDom.size(), 1);
    ASSERT_EQ(fImmDom[0], h);

    auto &hImmDom = h->GetImmediateDominatedBlocks();
    ASSERT_EQ(hImmDom.size(), 0);

    auto &iImmDom = i->GetImmediateDominatedBlocks();
    ASSERT_EQ(iImmDom.size(), 0);

    auto &gImmDom = g->GetImmediateDominatedBlocks();
    ASSERT_EQ(gImmDom.size(), 0);

    auto &cImmDom = c->GetImmediateDominatedBlocks();
    ASSERT_EQ(cImmDom.size(), 0);

    auto &dImmDom = d->GetImmediateDominatedBlocks();
    ASSERT_EQ(dImmDom.size(), 0);
}

}  // namespace compiler::tests
