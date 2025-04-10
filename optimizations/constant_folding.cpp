#include "ir/data_types.h"
#include "ir/instructions.h"
#include "optimizations/peepholes.h"

namespace compiler {

bool Peepholes::ConstantFoldingMul(Instruction *insn)
{
    assert(insn != nullptr);
    assert(insn->GetOpcode() == Opcode::MUL);

    if (!insn->GetInputs()->GetInput(0)->IsConst() || !insn->GetInputs()->GetInput(1)->IsConst()) {
        return false;
    }

    auto *input0 = insn->GetInputs()->GetInput(0)->AsConst();
    auto *input1 = insn->GetInputs()->GetInput(1)->AsConst();

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

bool Peepholes::ConstantFoldingAshr(Instruction *insn)
{
    assert(insn != nullptr);
    assert(insn->GetOpcode() == Opcode::ASHR);
    assert(insn->IsIntResultType());

    if (!insn->GetInputs()->GetInput(0)->IsConst() || !insn->GetInputs()->GetInput(1)->IsConst()) {
        return false;
    }

    int64_t input0Value = insn->GetInputs()->GetInput(0)->AsConst()->GetAsI64();
    uint64_t input1Value = insn->GetInputs()->GetInput(1)->AsConst()->GetAsI64();

    if (insn->GetResultType() == DataType::I32 || insn->GetResultType() == DataType::U32) {
        int32_t value = static_cast<int32_t>(input0Value) >> static_cast<uint32_t>(input1Value);
        graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, insn->GetResultType());
    } else {
        int64_t value = input0Value >> input1Value;
        graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, insn->GetResultType());
    }

    return true;
}

bool Peepholes::ConstantFoldingOr(Instruction *insn)
{
    assert(insn != nullptr);
    assert(insn->GetOpcode() == Opcode::OR);
    assert(insn->IsIntResultType());

    if (!insn->GetInputs()->GetInput(0)->IsConst() || !insn->GetInputs()->GetInput(1)->IsConst()) {
        return false;
    }

    auto *input0 = insn->GetInputs()->GetInput(0)->AsConst();
    auto *input1 = insn->GetInputs()->GetInput(1)->AsConst();

    uint64_t value = input0->GetAsU64() | input1->GetAsU64();
    graph_->CreateNewInsnInsteadOfInsn<ConstantInsn>(insn, value, insn->GetResultType());

    return true;
}

}  // namespace compiler
