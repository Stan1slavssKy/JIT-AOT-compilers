#include <sstream>
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

    for (auto dependencyIt = dependencies_.cbegin(); dependencyIt != dependencies_.cend(); ++dependencyIt) {
        auto &[inst, bbs] = *dependencyIt;

        for (auto bbIt = bbs.cbegin(); bbIt != bbs.cend(); ++bbIt) {
            ss << 'v' << inst->GetId() << ":BB_" << (*bbIt)->GetId();
            if (std::next(bbIt) != bbs.cend()) {
                ss << ", ";
            }
        }

        if (dependencies_.cend() != std::next(dependencyIt)) {
            ss << ", ";
        }
    }
}

void ArithmeticInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << GetInputs()->GetInput(0)->GetId() << ", "
       << "v" << GetInputs()->GetInput(1)->GetId();
}

void JmpInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "BB_" << GetBBToJmp()->GetId();
}

void BranchInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << GetInputs()->GetInput(0)->GetId() << ", "
       << "v" << GetInputs()->GetInput(1)->GetId() << ", ";
    ss << "BB_" << GetTrueBranchBB()->GetId() << ", BB_" << GetFalseBranchBB()->GetId();
}

void RetInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);

    auto input = GetInputs()->GetInput(0);
    if (input != nullptr) {
        ss << "v" << input->GetId();
    }
}

void NullCheckInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << objectValueToCheck_->GetId();
}

void StoreArrayInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    auto &inputs = GetInputs()->AsVectorInputs()->GetInputs();
    for (auto it = inputs.cbegin(); it < inputs.cend(); ++it) {
        ss << "v" << (*it)->GetId();

        if (std::next(it) != inputs.cend()) {
            ss << ", ";
        }
    }
}

void LoadArrayInsn::Dump(std::stringstream &ss) const
{
    Instruction::Dump(ss);
    ss << "v" << GetInputs()->GetInput(0)->GetId() << ", "
       << "v" << GetInputs()->GetInput(1)->GetId();
}

}  // namespace compiler
