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
    auto &blocks = graph_->GetRpoVector();
    for (auto *block : blocks) {
        block->EnumerateInsns([](Instruction *currInsn) {
            bool isProvideRefInBBInsn =
                currInsn->GetOpcode() == Opcode::NEWARR ||
                (currInsn->GetOpcode() == Opcode::PARAMETER && static_cast<ParameterInsn *>(currInsn)->IsRefParam());
            if (!isProvideRefInBBInsn) {
                return true;
            }

            std::cout << "Insn id = " << currInsn->GetId() << std::endl;

            Instruction *nullcheckToRemain = nullptr;

            for (auto userIt = currInsn->GetUsers().begin(); userIt != currInsn->GetUsers().end();) {
                // Need to save iterator, because of removing user from list below.
                auto currUser = *(userIt++);
                if (currUser->GetOpcode() == Opcode::NULLCHECK) {
                    std::cout << "User = " << currUser->GetId() << std::endl;

                    if (nullcheckToRemain == nullptr) {
                        nullcheckToRemain = currUser;
                        std::cout << "NullcheckTo remain = " << nullcheckToRemain->GetId() << std::endl;
                        continue;
                    }

                    if (nullcheckToRemain->DominatedOver(currUser)) {
                        std::cout << nullcheckToRemain->GetId() << " dominated over " << currUser->GetId() << std::endl;
                        currUser->ReplaceInputsForUsers(nullcheckToRemain);
                        currUser->GetParentBB()->Remove(currUser);
                    } else {
                        std::cout << nullcheckToRemain->GetId() << " NOT dominated over " << currUser->GetId()
                                  << std::endl;
                    }
                }
            }
            return false;
        });
    }
}

}  // namespace compiler
