#include <gtest/gtest.h>

#include "ir/ir_builder.h"

namespace compiler::tests {

/*
    Test generates a IR that corresponds to the following C++ code:

    int fact(int N) {
        if (N == 0) {
            return 1;
        }
        return N * fact(N - 1);
    }

    .function i64 fact(i64 v0)
        BB_1:
            beq.i64 v0, 0
        
        BB_2:
            ret.i64 1

*/
TEST(IR_BUILDER, RecursiveFactorial)
{
    auto *builder = IrBuilder::GetInstance();

    auto *func = builder->CreateFunction("Fact", ValueType::I64, std::vector{ValueType::I64});
    (void) func;

    IrBuilder::DeleteInstance();
}

}  // namespace compiler::tests
