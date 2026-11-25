#include <gtest/gtest.h>

#include "analysis/dfs.h"
#include "ir/graph.h"
#include "ir/ir_builder.h"

namespace compiler::tests {

TEST(RPO, TEST_1)
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

    DFS rpo(&graph);
    rpo.SetMarker(graph.CreateNewMarker());
    auto rpoVec = rpo.RunRPO();

    std::vector<BasicBlock *> expectedVec = {a, b, f, g, e, c, d};

    ASSERT_EQ(rpoVec.size(), expectedVec.size());

    for (size_t idx = 0; idx < rpoVec.size(); ++idx) {
        ASSERT_EQ(rpoVec[idx]->GetId(), expectedVec[idx]->GetId());
    }
}

TEST(RPO, TEST_2)
{
    Graph graph;
    IrBuilder builder(&graph);

    auto *a = builder.CreateBB();
    auto *b = builder.CreateBB();
    auto *c = builder.CreateBB();
    [[maybe_unused]] auto *d = builder.CreateBB();

    a->AddSuccessor(b);
    b->AddSuccessor(c);
    c->AddSuccessor(b);
    // d is unreachable after infinitive loop.

    DFS rpo(&graph);
    rpo.SetMarker(graph.CreateNewMarker());
    auto rpoVec = rpo.RunRPO();

    std::vector<BasicBlock *> expectedVec = {a, b, c};

    ASSERT_EQ(rpoVec.size(), expectedVec.size());

    for (size_t idx = 0; idx < rpoVec.size(); ++idx) {
        ASSERT_EQ(rpoVec[idx]->GetId(), expectedVec[idx]->GetId());
    }
}

}  // namespace compiler::tests
