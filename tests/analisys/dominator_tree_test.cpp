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

    ASSERT_EQ(a->GetImmediateDominator(), nullptr);
    ASSERT_EQ(b->GetImmediateDominator(), a);
    ASSERT_EQ(c->GetImmediateDominator(), b);
    ASSERT_EQ(d->GetImmediateDominator(), b);
    ASSERT_EQ(e->GetImmediateDominator(), f);
    ASSERT_EQ(f->GetImmediateDominator(), b);
    ASSERT_EQ(g->GetImmediateDominator(), f);
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

    ASSERT_EQ(a->GetImmediateDominator(), nullptr);
    ASSERT_EQ(b->GetImmediateDominator(), a);
    ASSERT_EQ(c->GetImmediateDominator(), b);
    ASSERT_EQ(d->GetImmediateDominator(), c);
    ASSERT_EQ(e->GetImmediateDominator(), d);
    ASSERT_EQ(f->GetImmediateDominator(), e);
    ASSERT_EQ(g->GetImmediateDominator(), f);
    ASSERT_EQ(h->GetImmediateDominator(), g);
    ASSERT_EQ(i->GetImmediateDominator(), g);
    ASSERT_EQ(j->GetImmediateDominator(), b);
    ASSERT_EQ(k->GetImmediateDominator(), i);
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

    ASSERT_EQ(a->GetImmediateDominator(), nullptr);
    ASSERT_EQ(b->GetImmediateDominator(), a);
    ASSERT_EQ(c->GetImmediateDominator(), b);
    ASSERT_EQ(d->GetImmediateDominator(), b);
    ASSERT_EQ(e->GetImmediateDominator(), b);
    ASSERT_EQ(f->GetImmediateDominator(), e);
    ASSERT_EQ(g->GetImmediateDominator(), b);
    ASSERT_EQ(h->GetImmediateDominator(), f);
    ASSERT_EQ(i->GetImmediateDominator(), b);
}

}  // namespace compiler::tests
