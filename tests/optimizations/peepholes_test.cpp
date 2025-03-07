#include <gtest/gtest.h>

#include "optimizations/peepholes.h"
#include "ir/ir_builder-inl.h"

namespace compiler::tests {

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

}  // namespace compiler::tests
