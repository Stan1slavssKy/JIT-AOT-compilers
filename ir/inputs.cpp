#include "ir/inputs.h"
#include "ir/instruction.h"

namespace compiler {

void DefaultInputs::RemoveUsers(Instruction *insnToRemove)
{
    if (inputs_[0] != nullptr) {
        inputs_[0]->RemoveUser(insnToRemove);
    }
    if (inputs_[1] != nullptr) {
        inputs_[1]->RemoveUser(insnToRemove);
    }
}

void VectorInputs::RemoveUsers(Instruction *insnToRemove)
{
    for (auto *input : inputs_) {
        input->RemoveUser(insnToRemove);
    }
}

}  // namespace compiler
