#include "optimizations/peepholes.h"
#include "analysis/dominator_tree.h"
#include "ir/instructions.h"

namespace compiler {

void Peepholes::Run()
{
    DominatorTree tree(graph_);
    tree.Build();

    auto &blocks = graph_->GetRpoVector();
    for (auto *b : blocks) {
        Instruction *currInsn = b->GetFirstInsn();
        Instruction *nextInsn = nullptr;

        while (currInsn != nullptr) {
            nextInsn = currInsn->GetNext();

            auto visitMethod = opcodeToVisitTable_[static_cast<size_t>(currInsn->GetOpcode())];
            (this->*visitMethod)(currInsn);

            currInsn = nextInsn;
        }
    }
}

void Peepholes::VisitMUL(Instruction *insn)
{
    if (insn->GetInput(0)->IsConst()) {
        insn->SwapInputs();
    }

    auto *input0 = insn->GetInput(0);
    auto *input1 = insn->GetInput(1);

    if (!input1->IsConst()) {
        return;
    }

    auto *constInput1 = static_cast<ConstantInsn *>(input1);
    if (constInput1->IsEqualTo(1)) {
        // 0.u64 Constant 1
        // ...
        // 3.u64 mul v2, v0
        // 4.u64 add v3, v0
        // ==>
        // 0.u64 Constant 1
        // ...
        // 3.u64 mul v2, v0
        // 4.u64 add v2, v0
        auto &currInsnUsers = insn->GetUsers();
        auto userIt = currInsnUsers.begin();
        while (userIt != currInsnUsers.end()) {
            auto *user = *userIt;
            bool needRemoveUser = false;

            if (user->GetInput(0) == insn) {
                user->SetInput(input0, 0);
                needRemoveUser = true;
            }
            if (user->GetInput(1) == insn) {
                user->SetInput(input0, 1);
                needRemoveUser = true;
            }
            if (needRemoveUser) {
                userIt = insn->RemoveUser(userIt);
            } else {
                ++userIt;
            }
        }
    }
}

void Peepholes::VisitASHR([[maybe_unused]] Instruction *insn)
{
    std::cout << "Im here VisitASHR" << std::endl;
}

void Peepholes::VisitOR([[maybe_unused]] Instruction *insn)
{
    std::cout << "Im here VisitOR" << std::endl;
}

void Peepholes::VisitUNDEFINED([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitADD([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitSUB([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitDIV([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitREM([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitAND([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitXOR([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitSHR([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitSHL([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitLOAD([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitSTORE([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitJMP([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBEQ([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBNE([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBGT([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitCALL([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitRET([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitPHI([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitPARAMETER([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitCONSTANT([[maybe_unused]] Instruction *insn) {}

}  // namespace compiler
