#include "optimizations/peepholes.h"
#include "analysis/dominator_tree.h"
#include "ir/instructions.h"
#include "ir/helpers.h"

namespace compiler {

void Peepholes::Run()
{
    DominatorTree tree(graph_);
    tree.Build();

    auto &blocks = graph_->GetRpoVector();
    for (auto *block : blocks) {
        block->EnumerateInsns([this](Instruction *currInsn) {
            auto visitMethod = opcodeToVisitTable_[static_cast<size_t>(currInsn->GetOpcode())];
            (this->*visitMethod)(currInsn);
        });
    }
}

void Peepholes::VisitMul(Instruction *insn)
{
    if (ConstantFoldingMul(insn)) {
        return;
    }

    if (insn->GetInputs()->GetInput(0)->IsConst()) {
        insn->GetInputs()->SwapInputs();
    }

    auto *input0 = insn->GetInputs()->GetInput(0);
    auto *input1 = insn->GetInputs()->GetInput(1);

    if (input1->IsConst()) {
        auto *constInput1 = input1->AsConst();
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

                if (user->GetInputs()->GetInput(0) == insn) {
                    user->GetInputs()->SetInput(input0, 0);
                    needRemoveUser = true;
                }
                if (user->GetInputs()->GetInput(1) == insn) {
                    user->GetInputs()->SetInput(input0, 1);
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

            graph_->CreateNewInsnInsteadOfInsn<AddInsn>(insn, DataType::F64, input0, input0);
        }
    }
}

void Peepholes::VisitAshr(Instruction *insn)
{
    if (ConstantFoldingAshr(insn)) {
        return;
    }

    auto *input0 = insn->GetInputs()->GetInput(0);
    auto *input1 = insn->GetInputs()->GetInput(1);

    if (input1->IsConst()) {
        auto *input1AsConst = input1->AsConst();
        if (input1AsConst->GetAsU64() == 0U) {
            // 0.u64 Constant 0
            // 1.i64 ...
            // 2.i64 ashr v1, v0
            // 3.i64 sub v2, v0
            // ==>
            // 0.u64 Constant 0
            // 1.i64 ...
            // 3.u64 sub v1, v0
            insn->ReplaceInputsForUsers(input0);
            insn->GetParentBB()->Remove(insn);
            return;
        }
        // 0.u64 Constant xxx
        // 1.u64 Constant yyy
        // 2. ...
        // 3. ashr v2, v0
        // 4. ashr v3, v1
        // ==>
        // 0.u64 Constant xxx
        // 1.u64 Constant yyy
        // 2. ...
        // 3. ashr v2, v0   <-- this insn will be deleted by dead code elimination if it has no more users
        // 5.u64 Constant zzz (xxx + yyy = v0 + v1)
        // 4. ashr v2, v5
        if (input0->GetOpcode() == Opcode::ASHR && input0->GetInputs()->GetInput(1)->IsConst()) {
            auto *input0FromPrevInsn = input0->GetInputs()->GetInput(0);
            auto *input1FromPrevInsnAsConst = input0->GetInputs()->GetInput(1)->AsConst();

            if (input1FromPrevInsnAsConst->GetType() == input1AsConst->GetType()) {
                auto newConstType = input1AsConst->GetType();
                auto newConstValue = input1AsConst->GetAsI64();
                newConstValue += input1FromPrevInsnAsConst->GetAsI64();
                auto *newConstInsn = graph_->CreateInsn<ConstantInsn>(newConstValue, newConstType);
                assert(newConstInsn->GetType() == input1AsConst->GetType());

                insn->GetParentBB()->InsertInstruction(insn, newConstInsn);

                input0->RemoveUser(insn);
                input0FromPrevInsn->AddUser(insn);
                newConstInsn->AddUser(insn);

                insn->GetInputs()->SetInput(input0FromPrevInsn, 0);
                insn->GetInputs()->SetInput(newConstInsn, 1);
                return;
            }
        }
    }
}

void Peepholes::VisitOr(Instruction *insn)
{
    if (ConstantFoldingOr(insn)) {
        return;
    }

    if (insn->GetInputs()->GetInput(0)->IsConst()) {
        insn->GetInputs()->SwapInputs();
    }

    auto *input0 = insn->GetInputs()->GetInput(0);
    auto *input1 = insn->GetInputs()->GetInput(1);

    if (input1->IsConst()) {
        auto *input1AsConst = input1->AsConst();
        if (input1AsConst->GetAsU64() == 0U) {
            // 0.u64 Constant 0
            // 1. ...
            // 2.u64 or v1, v0
            // 3.u64 sub v2, v0
            // ==>
            // 0.u64 Constant 0
            // 1. ...
            // 3.u64 sub v1, v0
            insn->ReplaceInputsForUsers(input0);
            insn->GetParentBB()->Remove(insn);
        }
    } else if (input0 == input1) {
        // 0. ...
        // 1. or v0, v0
        // 2. sub v1, v10
        // ==>
        // 0. ...
        // 2. sub v0, v10
        insn->ReplaceInputsForUsers(input0);
        insn->GetParentBB()->Remove(insn);
    }
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
void Peepholes::VisitCallStatic([[maybe_unused]] Instruction *insn) {}

}  // namespace compiler
