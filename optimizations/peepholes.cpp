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

void Peepholes::VisitMul(Instruction *insn)
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
    } else if (constInput1->IsEqualTo(2)) {
        // 0.u64 Constant 2
        // 1. ...
        // 2.f64 mul v1, v0
        // 3.f64 sub v2, v0
        // ==>
        // 0.u64 Constant 2
        // 1. ...
        // 4.f64 add v1, v1
        // 3.f64 sub v4, v0

        auto *newAddInsn = graph_->CreateInsn<AddInsn>(DataType::F64, input0, input0);
        insn->ReplaceInputsForUsers(newAddInsn);

        auto *prevInsn = insn->GetPrev();
        auto *currBb = insn->GetParentBB();

        currBb->InsertInstruction(prevInsn, newAddInsn);
        currBb->Remove(insn);  // May be it is better to not remove insn and do dead code elimination as the last pass.
    }
}

void Peepholes::VisitAshr([[maybe_unused]] Instruction *insn)
{
    std::cout << "Im here VisitAshr" << std::endl;
}

void Peepholes::VisitOr([[maybe_unused]] Instruction *insn)
{
    std::cout << "Im here VisitOr" << std::endl;
}

void Peepholes::VisitUndefined([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitAdd([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitSub([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitDiv([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitRem([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitAnd([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitXor([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitShr([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitShl([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitJmp([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBeq([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBne([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitBgt([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitRet([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitPhi([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitParameter([[maybe_unused]] Instruction *insn) {}
void Peepholes::VisitConstant([[maybe_unused]] Instruction *insn) {}

}  // namespace compiler
