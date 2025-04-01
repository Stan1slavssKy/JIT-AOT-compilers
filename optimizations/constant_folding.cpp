#include "ir/data_types.h"
#include "ir/instructions.h"
#include "optimizations/peepholes.h"

namespace compiler {

bool Peepholes::ConstantFoldingMul(Instruction *insn)
{
    if (!insn->GetInput(0)->IsConst() || !insn->GetInput(1)->IsConst()) {
        return false;
    }

    auto *input0 = static_cast<ConstantInsn *>(insn->GetInput(0));
    auto *input1 = static_cast<ConstantInsn *>(insn->GetInput(1));

    switch (insn->GetResultType()) {
        case DataType::U64:
            [[fallthrough]];
        case DataType::I64: {
            int64_t value = input0->GetAsU64() * input1->GetAsU64();
            graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, DataType::I64);
            break;
        }
        case DataType::F32: {
            float value = input0->GetAsF32() * input1->GetAsF32();
            graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, DataType::F32);
            break;
        }
        case DataType::F64: {
            double value = input0->GetAsF64() * input1->GetAsF64();
            graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, DataType::F64);
            break;
        }
        default: {
            UNREACHABLE();
            return false;
        }
    }

    return true;
}

}  // namespace compiler
