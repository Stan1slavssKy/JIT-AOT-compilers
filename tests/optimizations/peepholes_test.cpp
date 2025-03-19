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

TEST(Peepholes, IDENTICAL_MUL_CONSTANT_ON_RIGHT_SIDE)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v2, v0
            4.u64 add v3, v0
            ==>
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v2, v0
            4.u64 add v2, v0
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(1);
    auto *v1 = builder.CreateInt64ConstantInsn(120);
    auto *v2 = builder.CreateAddInsn(DataType::U64, v0, v1);
    auto *v3 = builder.CreateMulInsn(DataType::U64, v2, v0);
    auto *v4 = builder.CreateAddInsn(DataType::U64, v3, v0);

    peepholes.Run();

    // Ensure that inputs did not change
    ASSERT_EQ(v3->GetInput(0), v2);
    ASSERT_EQ(v3->GetInput(1), v0);

    // Check that there are no users
    ASSERT_TRUE(v3->GetUsers().empty());

    // Ensure that add have no mul result as input
    ASSERT_EQ(v4->GetInput(0), v2);
    ASSERT_EQ(v4->GetInput(1), v0);
}

TEST(Peepholes, IDENTICAL_MUL_CONSTANT_ON_LEFT_SIDE)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v0, v2
            4.u64 add v3, v0
            ==>
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v2, v0
            4.u64 add v2, v0
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(1);
    auto *v1 = builder.CreateInt64ConstantInsn(120);
    auto *v2 = builder.CreateAddInsn(DataType::U64, v0, v1);
    auto *v3 = builder.CreateMulInsn(DataType::U64, v0, v2);
    auto *v4 = builder.CreateAddInsn(DataType::U64, v3, v0);

    peepholes.Run();

    // Ensure that inputs CHANGED
    ASSERT_EQ(v3->GetInput(0), v2);
    ASSERT_EQ(v3->GetInput(1), v0);

    // Check that there are no users
    ASSERT_TRUE(v3->GetUsers().empty());

    // Ensure that add have no mul result as input
    ASSERT_EQ(v4->GetInput(0), v2);
    ASSERT_EQ(v4->GetInput(1), v0);
}

TEST(Peepholes, IDENTICAL_MUL_WITH_MANY_USERS)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v0, v2
            4.u64 add v3, v0
            5.u64 add v3, v3
            6.u64 add v1, v3
            ==>
            0.u64 Constant 1
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v0, v2
            4.u64 add v2, v0
            4.u64 add v2, v0
            5.u64 add v2, v2
            6.u64 add v1, v2
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(1);
    auto *v1 = builder.CreateInt64ConstantInsn(120);
    auto *v2 = builder.CreateAddInsn(DataType::U64, v0, v1);
    auto *v3 = builder.CreateMulInsn(DataType::U64, v0, v2);
    auto *v4 = builder.CreateAddInsn(DataType::U64, v3, v0);
    auto *v5 = builder.CreateAddInsn(DataType::U64, v3, v3);
    auto *v6 = builder.CreateAddInsn(DataType::U64, v1, v3);

    peepholes.Run();

    // Ensure that inputs CHANGED
    ASSERT_EQ(v3->GetInput(0), v2);
    ASSERT_EQ(v3->GetInput(1), v0);

    // Check that there are no users
    ASSERT_TRUE(v3->GetUsers().empty());

    // Ensure that add have no mul result as input
    ASSERT_EQ(v4->GetInput(0), v2);
    ASSERT_EQ(v4->GetInput(1), v0);

    ASSERT_EQ(v5->GetInput(0), v2);
    ASSERT_EQ(v5->GetInput(1), v2);

    ASSERT_EQ(v6->GetInput(0), v1);
    ASSERT_EQ(v6->GetInput(1), v2);
}

TEST(Peepholes, MUL_BY_TWO)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 2
            1.u64 Constant 12
            2.f64 add v0, v1
            3.f64 mul v2, v0
            4.f64 sub v3, v0
            ==>
            0.u64 Constant 2
            1.u64 Constant 12
            2.f64 add v0, v1
            5.f64 add v2, v2
            4.f64 sub v5, v0
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(2);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateAddInsn(DataType::F64, v0, v1);
    auto *v3 = builder.CreateMulInsn(DataType::F64, v2, v0);
    auto *v4 = builder.CreateSubInsn(DataType::F64, v3, v0);

    peepholes.Run();

    ASSERT_EQ(entryBB->GetLastInsn(), v4);

    auto *v5 = entryBB->GetLastInsn()->GetPrev();

    ASSERT_TRUE(v5->GetOpcode() == Opcode::ADD);
    ASSERT_EQ(v5->GetInput(0), v2);
    ASSERT_EQ(v5->GetInput(1), v2);

    auto v5users = v5->GetUsers();
    ASSERT_EQ(v5users.size(), 1);
    ASSERT_EQ(v5users.front(), v4);

    ASSERT_EQ(v4->GetInput(0), v5);
    ASSERT_EQ(v4->GetInput(1), v0);
}

TEST(Peepholes, MUL_BY_TWO_MANY_USERS)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 2
            1.u64 Constant 12
            2.f64 add v0, v1
            3.f64 mul v2, v0
            4.f64 sub v3, v0
            5.f64 mul v3, v3
            6.f64 div v3, v1
            ==>
            0.u64 Constant 2
            1.u64 Constant 12
            2.f64 add v0, v1
            7.f64 add v2, v2
            4.f64 sub v7, v0
            5.f64 mul v7, v7
            6.f64 div v7, v1
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(2);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateAddInsn(DataType::F64, v0, v1);
    auto *v3 = builder.CreateMulInsn(DataType::F64, v2, v0);
    auto *v4 = builder.CreateSubInsn(DataType::F64, v3, v0);
    auto *v5 = builder.CreateMulInsn(DataType::F64, v3, v3);
    auto *v6 = builder.CreateDivInsn(DataType::F64, v3, v1);

    peepholes.Run();

    ASSERT_EQ(entryBB->GetLastInsn(), v6);

    auto *v7 = entryBB->GetLastInsn()->GetPrev()->GetPrev()->GetPrev();

    ASSERT_TRUE(v7->GetOpcode() == Opcode::ADD);
    ASSERT_EQ(v7->GetInput(0), v2);
    ASSERT_EQ(v7->GetInput(1), v2);

    auto v7users = v7->GetUsers();
    ASSERT_EQ(v7users.size(), 3);

    std::array<Instruction *, 3U> expectedUsers = {v4, v5, v6};
    size_t idx = 0;
    for (auto *it : v7users) {
        ASSERT_EQ(it, expectedUsers[idx]);
        ++idx;
    }

    ASSERT_EQ(v4->GetInput(0), v7);
    ASSERT_EQ(v4->GetInput(1), v0);

    ASSERT_EQ(v5->GetInput(0), v7);
    ASSERT_EQ(v5->GetInput(1), v7);

    ASSERT_EQ(v6->GetInput(0), v7);
    ASSERT_EQ(v6->GetInput(1), v1);
}

}  // namespace compiler::tests
