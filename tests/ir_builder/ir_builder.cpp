#include <gtest/gtest.h>

#include "ir/ir_builder-inl.h"

namespace compiler::tests {

/*
    Test generates a IR that corresponds to the following C++ code:

    uint64_t fact(uint32_t n) {
        uint64_t res = 1U;
        for (uint32_t idx = 2U; idx <= n; ++idx) {
            res *= idx;
        }
        return res;
    }

    IR:

    Entry:
        0.u32 Parameter 0 // n
        1.u64 Constant 1
        2.u32 Constant 2
        3. jmp BB_1
    BB_1:
        4p.u64 Phi v1:Entry, v_7:BB_2 // res
        5p.u32 Phi v2:Entry, v_8:BB_2 // idx
        6. bgt v5, v0, BB_3, BB_2
    BB_2:
        7.u64 mul v4, v5
        8.u32 add v5, v1
        9. jmp BB_1
    BB_3:
        10.u64 ret v4
*/
TEST(IR_BUILDER, LoopFactorial)
{
    Graph graph;
    IrBuilder builder(&graph);

    auto *entryBB = builder.CreateBB();
    auto *bb1 = builder.CreateBB();
    auto *bb2 = builder.CreateBB();
    auto *bb3 = builder.CreateBB();

    builder.SetBasicBlockScope(entryBB);
    auto *v0 = builder.CreateParameterInsn(0);
    auto *v1 = builder.CreateInt64ConstantInsn(1);
    auto *v2 = builder.CreateInt64ConstantInsn(2);
    builder.CreateJmpInsn(bb1);

    builder.SetBasicBlockScope(bb1);
    auto *v4 = builder.CreatePhiInsn(DataType::U64);
    auto *v5 = builder.CreatePhiInsn(DataType::U32);
    builder.CreateBgtInsn(v5, v0, bb3, bb2);

    builder.SetBasicBlockScope(bb2);
    auto *v7 = builder.CreateMulInsn(DataType::U64, v4, v5);
    auto *v8 = builder.CreateAddInsn(DataType::U32, v5, v1);
    builder.CreateJmpInsn(bb1);

    v4->AddInput(v1);
    v4->AddInput(v7);

    v5->AddInput(v2);
    v5->AddInput(v8);

    builder.SetBasicBlockScope(bb3);
    builder.CreateRetInsn(DataType::U64, v4);

    std::stringstream ss;
    graph.Dump(ss);
    std::cout << ss.str() << std::endl;
}

}  // namespace compiler::tests
