#include "analysis/dominator_tree.h"
#include "ir/instruction.h"
#include "ir/instructions.h"
#include "optimizations/check_elimination.h"
#include <unordered_map>

namespace compiler {

void CheckElimination::Run()
{
    DominatorTree tree(graph_);
    tree.Build();

    OptimizeDominatedChecks();
}

void CheckElimination::OptimizeDominatedChecks()
{
    auto couldRemoveBoundCheck = [](Instruction *insn, Instruction *insnToRemove) -> bool {
        auto *check = static_cast<BoundsCheckInsn *>(insn);
        auto *checkToRemove = static_cast<BoundsCheckInsn *>(insnToRemove);

        return (check->GetMaxArrayIdx() == checkToRemove->GetMaxArrayIdx()) &&
               (check->GetIdxToCheck() == checkToRemove->GetIdxToCheck());
    };

    auto &blocks = graph_->GetRpoVector();
    for (auto *block : blocks) {
        block->EnumerateInsns([&couldRemoveBoundCheck](Instruction *currInsn) {
            if (!currInsn->DoesProduceReference()) {
                return true;
            }
            // currInsn produce reference, so this reference possibly has redundant checks.
            // The following algorithm find and leave only unredundant checks for currInsn.
            // We go through all users of produced reference, since users can be in other bb,
            // we will check all existing checks in the graph.
            Instruction *checkToRemain = nullptr;

            for (auto userIt = currInsn->GetUsers().begin(); userIt != currInsn->GetUsers().end();) {
                // Need to save iterator, because of removing user from list below.
                auto currUser = *(userIt++);

                bool isBoundCheck = currUser->GetOpcode() == Opcode::BOUNDSCHECK;
                bool isNullCheck = currUser->GetOpcode() == Opcode::NULLCHECK;

                if (isBoundCheck || isNullCheck) {
                    if (checkToRemain == nullptr) {
                        checkToRemain = currUser;
                        continue;
                    }

                    if (checkToRemain->DominatedOver(currUser)) {
                        if (isBoundCheck && !couldRemoveBoundCheck(checkToRemain, currUser)) {
                            continue;
                        }
                        currUser->ReplaceInputsForUsers(checkToRemain);
                        currUser->GetParentBB()->Remove(currUser);
                    }
                }
            }
            return false;
        });
    }
}

}  // namespace compiler
