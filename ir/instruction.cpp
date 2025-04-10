#include "ir/instruction.h"
#include "ir/instructions.h"

namespace compiler {

bool Instruction::TryReplaceInput(Instruction *inputToReplace, Instruction *insnToReplaceWith, size_t idx)
{
    if (inputs_->GetInput(idx) == inputToReplace) {
        return inputs_->SetInput(insnToReplaceWith, idx);
    }
    return false;
}

bool Instruction::ReplaceInputs(Instruction *inputToReplace, Instruction *insnToReplaceWith)
{
    bool succ0 = TryReplaceInput(inputToReplace, insnToReplaceWith, 0);
    bool succ1 = TryReplaceInput(inputToReplace, insnToReplaceWith, 1);

    return succ0 || succ1;
}

/// For all users of this insn change inputs from this insn to given.
void Instruction::ReplaceInputsForUsers(Instruction *insnToReplaceWith)
{
    for (auto userIt = users_.begin(); userIt != users_.end();) {
        // Need to save iterator, because of removing user from list below.
        auto currUser = *(userIt++);
        if (currUser->ReplaceInputs(this, insnToReplaceWith)) {
            // Now we successfully replace input for current user,
            // it means that `this` insn do not have this user, so we need to remove it.
            this->RemoveUser(currUser);
            // Current user is new user for insn by which we replace `this` insn.
            insnToReplaceWith->AddUser(currUser);
        }
    }
}

ConstantInsn *Instruction::AsConst()
{
    assert(opcode_ == Opcode::CONSTANT);
    return static_cast<ConstantInsn *>(this);
}

const ConstantInsn *Instruction::AsConst() const
{
    assert(opcode_ == Opcode::CONSTANT);
    return static_cast<const ConstantInsn *>(this);
}

}  // namespace compiler