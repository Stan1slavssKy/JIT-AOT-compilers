#include <gtest/gtest.h>

#include "analisys/dfs.h"
#include "ir/graph.h"
#include "ir/ir_builder.h"

namespace compiler::tests {

TEST(DFS, TEST_1)
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

    DFS dfs(&graph);
    dfs.Run();
    auto &dfsVec = dfs.GetDfsBlocks();

    std::vector<BasicBlock *> expectedVec = {a, b, c, d, f, e, g};

    ASSERT_EQ(dfsVec.size(), expectedVec.size());

    for (size_t idx = 0; idx < dfsVec.size(); ++idx) {
        ASSERT_EQ(dfsVec[idx]->GetId(), expectedVec[idx]->GetId());
    }
}

}  // namespace compiler::tests
