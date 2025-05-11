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
            3.u64 LoadArray v0, v1
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
    [[maybe_unused]] auto *v1 = builder.CreateInt64ConstantInsn(12);
    [[maybe_unused]] auto *v2 = builder.CreateBgtInsn(v1, v1, bb1, bb2);

    builder.SetBasicBlockScope(bb1);
    auto *v3 = builder.CreateNullcheckInsn(v0);
    [[maybe_unused]] auto *v4 = builder.CreateLoadArrayInsn(DataType::U64, v3, v1);
    [[maybe_unused]] auto *v5 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb2);
    auto *v6 = builder.CreateNullcheckInsn(v0);
    [[maybe_unused]] auto *v7 = builder.CreateLoadArrayInsn(DataType::U64, v6, v1);
    [[maybe_unused]] auto *v8 = builder.CreateJmpInsn(bb3);

    builder.SetBasicBlockScope(bb3);
    [[maybe_unused]] auto *v9 = builder.CreateRetInsn(DataType::VOID, nullptr);

    checkElimination.Run();

    CompareInputs<2U>(v4, {v3, v1});
    CompareInputs<2U>(v7, {v6, v1});
}

}  // namespace compiler::tests
