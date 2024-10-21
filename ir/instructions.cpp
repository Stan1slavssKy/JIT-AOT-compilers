#include "ir/instructions.h"
#include "ir/helpers.h"

namespace compiler {

void Instruction::Dump(std::stringstream &ss) const
{
    ss << insnId_ << "." << DataTypeToStr(resultType_) << " " << OpcodeToString(opcode_) << " ";
}

}  // namespace compiler
