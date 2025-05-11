#include <gtest/gtest.h>

#include "ir/data_types.h"
#include "tests/test_helper.h"

#include "ir/ir_builder-inl.h"
#include "optimizations/check_elimination.h"

namespace compiler::tests {

TEST(CheckElimination, Nullcheck)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /*
        entryBB:
            0.ref Parameter 0
            1.u64 Constant 12
            2.ref Nullcheck v0
            3.u64 LoadArray v2, v1
            4.ref Nullcheck v0
            5.ref StoreArray v4, v1, v3
            ===========================>
            0.ref Parameter 0
            1.u64 Constant 12
            2.ref Nullcheck v0
            3.u64 LoadArray v2, v1
            5.ref StoreArray v2, v1, v3
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateNullcheckInsn(v0);
    auto *v3 = builder.CreateLoadArrayInsn(DataType::U64, v2, v1);
    auto *v4 = builder.CreateNullcheckInsn(v0);
    auto *v5 = builder.CreateStoreArrayInsn(DataType::U64, v4, v1, v3);

    checkElimination.Run();

    CompareInputs<2U>(v3, {v2, v1});
    CompareInputs<3U>(v5, {v2, v1, v3});
    ASSERT_TRUE(v4->GetUsers().empty());
}

TEST(CheckElimination, NotAppliedNullcheckDifferentObject)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /* Different objects in nullchecks, graph should not be changed.
        entryBB:
            0.ref Parameter 0
            1.ref Parameter 1
            2.u64 Constant 12
            3.ref Nullcheck v0
            4.u64 LoadArray v3, v2
            5.ref Nullcheck v1
            6.ref LoadArray v5, v2
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v2 = builder.CreateInt64ConstantInsn(12);
    auto *v3 = builder.CreateNullcheckInsn(v0);
    auto *v4 = builder.CreateLoadArrayInsn(DataType::U64, v3, v2);
    auto *v5 = builder.CreateNullcheckInsn(v1);
    auto *v6 = builder.CreateLoadArrayInsn(DataType::U64, v5, v2);

    checkElimination.Run();

    CompareInputs<2U>(v4, {v3, v2});
    CompareInputs<2U>(v6, {v5, v2});
}

TEST(CheckElimination, NotAppliedNullcheckNotDominated)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /* Nullcheck is not dominated, should be no changes.
        BB_0:
            0.ref Parameter 0
            1.u64 Constant 12
            2. bgt v0, v0, BB_1, BB_2
        BB_1:
            3.ref Nullcheck v0
            4.u64 LoadArray v3, v1
            5. jmp BB_3
        BB_2:
            6.ref Nullcheck v0
            7.u64 LoadArray v6, v1
            8. jmp BB_3
        BB_3:
            9. ret
    */

    auto *bb0 = builder.CreateBB();
    auto *bb1 = builder.CreateBB();
    auto *bb2 = builder.CreateBB();
    auto *bb3 = builder.CreateBB();

    builder.SetBasicBlockScope(bb0);
    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    [[maybe_unused]] auto *v2 = builder.CreateBgtInsn(v1, v1, bb1, bb2);

    builder.SetBasicBlockScope(bb1);
    auto *v3 = builder.CreateNullcheckInsn(v0);
    auto *v4 = builder.CreateLoadArrayInsn(DataType::U64, v3, v1);
    [[maybe_unused]] auto *v5 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb2);
    auto *v6 = builder.CreateNullcheckInsn(v0);
    auto *v7 = builder.CreateLoadArrayInsn(DataType::U64, v6, v1);
    [[maybe_unused]] auto *v8 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb3);
    [[maybe_unused]] auto *v9 = builder.CreateRetInsn(DataType::VOID, nullptr);

    checkElimination.Run();

    CompareInputs<2U>(v4, {v3, v1});
    CompareInputs<2U>(v7, {v6, v1});
}

TEST(CheckElimination, BoundsCheck)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /*
        entryBB:
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 13
            3.u64 Constant 14
            4.u32 BoundsCheck v0, v1, v2
            5.u64 LoadArray v0, v4
            6.u32 BoundsCheck v0, v1, v2
            7.ref StoreArray v0, v6, v3
            ===========================>
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 13
            3.u64 Constant 14
            4.u32 BoundsCheck v0, v1, v2
            5.u64 LoadArray v0, v4
            7.ref StoreArray v0, v4, v3
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateInt64ConstantInsn(13);
    auto *v3 = builder.CreateInt64ConstantInsn(14);
    auto *v4 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v5 = builder.CreateLoadArrayInsn(DataType::U64, v0, v4);
    auto *v6 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v7 = builder.CreateStoreArrayInsn(DataType::U64, v0, v6, v3);

    checkElimination.Run();

    CompareInputs<2U>(v5, {v0, v4});
    CompareInputs<3U>(v7, {v0, v4, v3});
    ASSERT_TRUE(v6->GetUsers().empty());
}

TEST(CheckElimination, BoundsCheckNotApplied)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /*
        entryBB:
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 20
            3.u64 Constant 30
            4.ref BoundsCheck v0, v1, v2
            5.u64 LoadArray v4, v1
            6.ref BoundsCheck v0, v1, v3
            7.ref StoreArray v6, v1, v3
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateInt64ConstantInsn(20);
    auto *v3 = builder.CreateInt64ConstantInsn(30);
    auto *v4 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v5 = builder.CreateLoadArrayInsn(DataType::U64, v4, v1);
    auto *v6 = builder.CreateBoundsCheckInsn(v0, v1, v3);
    auto *v7 = builder.CreateStoreArrayInsn(DataType::U64, v6, v1, v3);

    checkElimination.Run();

    CompareInputs<2U>(v5, {v4, v1});
    CompareInputs<3U>(v7, {v6, v1, v3});
}

TEST(CheckElimination, BoundsCheckManyBB)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /*
        BB_0:
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 30
            3.u32 BoundsCheck v0, v1, v2
            4.u64 LoadArray v0, v3
            5. bgt v1, v2, BB_1, BB_2
        BB_1:
            6.u32 BoundsCheck v0, v1, v2
            7.u64 LoadArray v0, v6
            8. jmp BB_3
        BB_2:
            9.u32 BoundsCheck v0, v1, v2
            10.u64 LoadArray v0, v9
            11. jmp BB_3
        BB_3:
            12. ret
        ================================>
        BB_0:
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 30
            3.u32 BoundsCheck v0, v1, v2
            4.u64 LoadArray v0, v3
            5. bgt v1, v2, BB_1, BB_2
        BB_1:
            7.u64 LoadArray v0, v3
            8. jmp BB_3
        BB_2:
            10.u64 LoadArray v0, v3
            11. jmp BB_3
        BB_3:
            12. ret
    */
    auto *bb0 = builder.CreateBB();
    auto *bb1 = builder.CreateBB();
    auto *bb2 = builder.CreateBB();
    auto *bb3 = builder.CreateBB();

    builder.SetBasicBlockScope(bb0);
    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateInt64ConstantInsn(30);
    auto *v3 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v4 = builder.CreateLoadArrayInsn(DataType::U64, v0, v3);
    [[maybe_unused]] auto *v5 = builder.CreateBgtInsn(v1, v2, bb1, bb2);

    builder.SetBasicBlockScope(bb1);
    auto *v6 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v7 = builder.CreateLoadArrayInsn(DataType::U64, v0, v6);
    [[maybe_unused]] auto *v8 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb2);
    auto *v9 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v10 = builder.CreateLoadArrayInsn(DataType::U64, v0, v9);
    [[maybe_unused]] auto *v11 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb3);
    [[maybe_unused]] auto *v12 = builder.CreateRetInsn(DataType::VOID);

    checkElimination.Run();

    CompareInputs<2U>(v4, {v0, v3});
    CompareInputs<2U>(v7, {v0, v3});
    CompareInputs<2U>(v10, {v0, v3});

    ASSERT_TRUE(v6->GetUsers().empty());
    ASSERT_TRUE(v9->GetUsers().empty());
}

TEST(CheckElimination, BoundsCheckManyBBNotApplied)
{
    Graph graph;
    IrBuilder builder(&graph);
    CheckElimination checkElimination(&graph);

    /*
        BB_0:
            0.ref Parameter 0
            1.u64 Constant 12
            2.u64 Constant 30
            4. bgt v1, v2, BB_1, BB_2
        BB_1:
            5.u32 BoundsCheck v0, v1, v2
            6.u64 LoadArray v0, v5
            7. jmp BB_3
        BB_2:
            8.u32 BoundsCheck v0, v1, v2
            9.u64 LoadArray v0, v8
            10. jmp BB_3
        BB_3:
            11. ret
    */
    auto *bb0 = builder.CreateBB();
    auto *bb1 = builder.CreateBB();
    auto *bb2 = builder.CreateBB();
    auto *bb3 = builder.CreateBB();

    builder.SetBasicBlockScope(bb0);
    auto *v0 = builder.CreateParameterInsn(0, DataType::REF);
    auto *v1 = builder.CreateInt64ConstantInsn(12);
    auto *v2 = builder.CreateInt64ConstantInsn(30);
    [[maybe_unused]] auto *v4 = builder.CreateBgtInsn(v1, v2, bb1, bb2);

    builder.SetBasicBlockScope(bb1);
    auto *v5 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v6 = builder.CreateLoadArrayInsn(DataType::U64, v0, v5);
    [[maybe_unused]] auto *v7 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb2);
    auto *v8 = builder.CreateBoundsCheckInsn(v0, v1, v2);
    auto *v9 = builder.CreateLoadArrayInsn(DataType::U64, v0, v8);
    [[maybe_unused]] auto *v10 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb3);
    [[maybe_unused]] auto *v11 = builder.CreateRetInsn(DataType::VOID);

    checkElimination.Run();

    CompareInputs<2U>(v6, {v0, v5});
    CompareInputs<2U>(v9, {v0, v8});
}

}  // namespace compiler::tests
