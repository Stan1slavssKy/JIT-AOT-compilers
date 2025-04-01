#include <gtest/gtest.h>

#include "optimizations/peepholes.h"
#include "ir/ir_builder-inl.h"

namespace compiler::tests {

// For debug purposes.
[[maybe_unused]] static void DumpGraph(Graph &graph)
{
    std::stringstream ss;
    graph.Dump(ss);
    std::cout << ss.str() << std::endl;
}

TEST(ConstantFolding, MUL)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.i64 Constant 2
            1.i64 Constant 120
            2.i64 mul v0, v1
            3.i64 add v2, v0
            4.i64 sub v2, v1
            ==>
            0.i64 Constant 2
            1.i64 Constant 120
            5.i64 Constant 240
            3.i64 add v5, v0
            4.i64 sub v5, v1
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(2);
    auto *v1 = builder.CreateInt64ConstantInsn(120);
    auto *v2 = builder.CreateMulInsn(DataType::I64, v0, v1);
    auto *v3 = builder.CreateAddInsn(DataType::I64, v2, v0);
    auto *v4 = builder.CreateSubInsn(DataType::I64, v2, v1);

    DumpGraph(graph);
    peepholes.Run();
    DumpGraph(graph);

    auto *v5 = v1->GetNext();
    ASSERT_TRUE(v5->IsConst());
    ASSERT_EQ(static_cast<ConstantInsn *>(v5)->GetAsUnsignedInt(), 240U);

    ASSERT_TRUE(v4->GetUsers().empty());

    ASSERT_EQ(v3->GetInput(0), v5);
    ASSERT_EQ(v3->GetInput(1), v0);

    ASSERT_EQ(v4->GetInput(0), v5);
    ASSERT_EQ(v4->GetInput(1), v1);

    auto &v5users = v5->GetUsers();
    ASSERT_EQ(v5users.size(), 2);

    std::array<Instruction *, 2U> expectedUsers = {v3, v4};
    size_t idx = 0;
    for (auto *it : v5users) {
        ASSERT_EQ(it, expectedUsers[idx]);
        ++idx;
    }
}

}  // namespace compiler::tests
