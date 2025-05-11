#include "ir/instruction.h"
#include "ir/instructions.h"
#include "ir/basic_block.h"
#include "utils/macros.h"

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
    assert(opcode_ != Opcode::PHI);

    bool succ0 = TryReplaceInput(inputToReplace, insnToReplaceWith, 0);
    bool succ1 = TryReplaceInput(inputToReplace, insnToReplaceWith, 1);

    return succ0 || succ1;
}

bool Instruction::ReplaceVectorInputs(Instruction *inputToReplace, Instruction *insnToReplaceWith)
{
    bool inputsReplaced = false;

    auto &inputs = inputs_->AsVectorInputs()->GetInputs();
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        if (*it == inputToReplace) {
            *it = insnToReplaceWith;
            inputsReplaced = true;
        }
    }

    if (IsPhi() && inputsReplaced) {
        auto *phiInsn = static_cast<PhiInsn *>(this);
        bool depsReplaced = phiInsn->ReplaceDependency(inputToReplace, insnToReplaceWith);
        if (UNLIKELY(!depsReplaced)) {
            std::cerr << "ReplaceInputsPhi: failed to replaced dependency." << std::endl;
            UNREACHABLE();
        }
    }

    return inputsReplaced;
}

/// For all users of this insn change inputs from this insn to given.
void Instruction::ReplaceInputsForUsers(Instruction *insnToReplaceWith)
{
    for (auto userIt = users_.begin(); userIt != users_.end();) {
        // Need to save iterator, because of removing user from list below.
        auto currUser = *(userIt++);

        bool isReplaced = currUser->HasVectorInputs() ? currUser->ReplaceVectorInputs(this, insnToReplaceWith)
                                                      : currUser->ReplaceInputs(this, insnToReplaceWith);

        if (isReplaced) {
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

bool Instruction::DominatedOver(Instruction *insn)
{
    if (insn == nullptr) {
        return true;
    }

    if (parentBB_ != insn->GetParentBB()) {
        return parentBB_->IsDominatesOver(insn->GetParentBB());
    }

    bool result = false;

    parentBB_->EnumerateInsns([&result, &insn, this](Instruction *currInsn) {
        if (currInsn == insn) {
            result = false;
            return true;
        } else if (currInsn == this) {
            result = true;
            return true;
        }

        return false;
    });

    return result;
}

}  // namespace compiler
