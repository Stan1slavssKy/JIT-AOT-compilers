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
        ss << GetAsI64();
    } else if (IsUnsignedInt()) {
        ss << GetAsU64();
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
    ss << "v" << GetInput(0)->GetId() << ":BB_" << GetInput(0)->GetParentBB()->GetId();
    ss << ", ";
    ss << "v" << GetInput(1)->GetId() << ":BB_" << GetInput(1)->GetParentBB()->GetId();
}

void ArithmeticInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << GetInput(0)->GetId() << ", "
       << "v" << GetInput(1)->GetId();
}

void JmpInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "BB_" << GetBBToJmp()->GetId();
}

void BranchInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << GetInput(0)->GetId() << ", "
       << "v" << GetInput(1)->GetId() << ", ";
    ss << "BB_" << GetTrueBranchBB()->GetId() << ", BB_" << GetFalseBranchBB()->GetId();
}

void RetInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << retValue_->GetId();
}

}  // namespace compiler
