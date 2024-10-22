#include "ir/instructions.h"
#include "ir/helpers.h"
#include "ir/instruction.h"
#include "utils/macros.h"
#include "ir/basic_block.h"

namespace compiler {

void Instruction::Dump(std::stringstream &ss) const
{
    ss << insnId_ << "." << DataTypeToStr(resultType_) << " " << OpcodeToString(opcode_) << " ";
}

void ParameterInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << argNum_;
}

void ConstantInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    if (IsSignedInt()) {
        ss << GetAsSignedInt();
    } else if (IsUnsignedInt()) {
        ss << GetAsUnsignedInt();
    } else if (IsF32()) {
        ss << GetAsF32();
    } else if (IsF64()) {
        ss << GetAsF64();
    } else {
        UNREACHABLE();
    }
}

void PhiInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    auto &inputs = GetInputs();

    for (size_t idx = 0; idx < inputs.size(); ++idx) {
        ss << "v" << inputs[idx].input->GetId() << ":BB_" << inputs[idx].input->GetParentBB()->GetId();
        if (idx != inputs.size() - 1) {
            ss << ", ";
        }
    }
}

void ArithmeticInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);

    auto &inputs = GetInputs();

    for (size_t idx = 0; idx < inputs.size(); ++idx) {
        ss << "v" << inputs[idx].input->GetId();
        if (idx != inputs.size() - 1) {
            ss << ", ";
        }
    }
}

void JmpInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "BB_" << GetBBToJmp()->GetId();
}

void BranchInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    auto &inputs = GetInputs();
    for (auto &i : inputs) {
        ss << "v" << i.input->GetId() << ", ";
    }

    ss << "BB_" << GetTrueBranchBB()->GetId() << ", BB_" << GetFalseBranchBB()->GetId();
}

void RetInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << retValue_->GetId();
}

}  // namespace compiler
