#include <gtest/gtest.h>

#include "ir/ir_builder.h"

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
        3. jmp BB_2
    BB_1:
        4p.u64 Phi v1:Entry, v_7:BB_3 // res
        5p.u32 Phi v2:Entry, v_8:BB_3 // idx
        6. bgt BB_4, BB_3
    BB_2:
        7.u64 mul v4, v5
        8.u32 add v5, v1
        9. jmp BB_2
    BB_3:
        10.u64 ret v4
*/
TEST(IR_BUILDER, RecursiveFactorial)
{
    auto *builder = IrBuilder::GetInstance();

    auto *fact = builder->CreateFunction("Fact", ValueType::U64, std::vector{ValueType::U64});

    auto *entryBB = builder->CreateBB(fact);
    auto *bb1 = builder->CreateBB(fact);
    auto *bb2 = builder->CreateBB(fact);
    auto *bb3 = builder->CreateBB(fact);

    auto v0 = builder->CreateValue(ValueType::U32, 0);
    auto v1 = builder->CreateValue(ValueType::U64, 1);
    auto v2 = builder->CreateValue(ValueType::U64, 2);
    builder->CreateJmp(entryBB, bb1);

    

    IrBuilder::DeleteInstance();
}

}  // namespace compiler::tests
