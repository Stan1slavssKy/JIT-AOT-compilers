#include <gtest/gtest.h>

#include "optimizations/peepholes.h"
#include "ir/ir_builder-inl.h"

namespace compiler::tests {

TEST(Peepholes, ZERO_MUL)
{
    Graph graph;
    Peepholes peepholes(&graph);
    IrBuilder builder(&graph);

    /*
        entryBB:
            0.u64 Constant 0
            1.u64 Constant 120
            2.u64 add v0, v1
            3.u64 mul v2, v0
    */
    auto *entryBB = builder.CreateBB();
    builder.SetBasicBlockScope(entryBB);

    auto *v0 = builder.CreateInt64ConstantInsn(0);
    auto *v1 = builder.CreateInt64ConstantInsn(120);
    auto *v2 = builder.CreateAddInsn(DataType::U64, v0, v1);
    [[maybe_unused]] auto *v3 = builder.CreateMulInsn(DataType::U64, v2, v0);

    peepholes.Run();
}

}  // namespace compiler::tests
