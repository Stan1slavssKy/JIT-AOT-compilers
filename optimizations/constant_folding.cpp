#include "optimizations/peepholes.h"

namespace compiler {

bool Peepholes::ConstantFoldingMul(Instruction *insn)
{
    if (!insn->GetInput(0)->IsConst() || !insn->GetInput(1)->IsConst()) {
        return false;
    }

    auto *input0 = static_cast<ConstantInsn *>(insn->GetInput(0));
    auto *input1 = static_cast<ConstantInsn *>(insn->GetInput(1));

    [[maybe_unused]] ConstantInsn *newConstInsn = nullptr;

    switch (insn->GetResultType()) {
        case DataType::U64:
            [[fallthrough]];
        case DataType::I64: {
            int64_t value = input0->GetAsUnsignedInt() * input1->GetAsUnsignedInt();
            newConstInsn = graph_->CreateInsn<ConstantInsn>(value, DataType::I64);
            insn->ReplaceInputsForUsers(newConstInsn);

            auto *prevInsn = insn->GetPrev();
            auto *currBb = insn->GetParentBB();
            currBb->InsertInstruction(prevInsn, newConstInsn);
            currBb->Remove(insn);
            break;
        }
        case DataType::F32: {
            float value = input0->GetAsF32() * input1->GetAsF32();
            newConstInsn = graph_->CreateInsn<ConstantInsn>(value, DataType::F32);
            insn->ReplaceInputsForUsers(newConstInsn);

            auto *prevInsn = insn->GetPrev();
            auto *currBb = insn->GetParentBB();
            currBb->InsertInstruction(prevInsn, newConstInsn);
            currBb->Remove(insn);
            break;
        }
        case DataType::F64: {
            double value = input0->GetAsF64() * input1->GetAsF64();
            newConstInsn = graph_->CreateInsn<ConstantInsn>(value, DataType::F64);
            insn->ReplaceInputsForUsers(newConstInsn);

            auto *prevInsn = insn->GetPrev();
            auto *currBb = insn->GetParentBB();
            currBb->InsertInstruction(prevInsn, newConstInsn);
            currBb->Remove(insn);
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
