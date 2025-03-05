#include "optimizations/peepholes.h"
#include "analysis/dominator_tree.h"

namespace compiler {

void Peepholes::Run()
{
    DominatorTree tree(graph_);
    tree.Build();

    auto &blocks = graph_->GetRpoVector();
    for (auto *b : blocks) {
        Instruction *currInsn = b->GetFirstInsn();
        Instruction *nextInsn = currInsn->GetNext();
        Instruction *tempInsn = nullptr;

        while (nextInsn != nullptr) {
            auto visitMethod = opcodeToVisitTable_[static_cast<size_t>(currInsn->GetOpcode())];
            (this->*visitMethod)();

            tempInsn = currInsn;
            currInsn = nextInsn;
            nextInsn = tempInsn->GetNext();
        }
    }
}

void Peepholes::VisitMUL()
{
    std::cout << "Im here VisitMUL" << std::endl;
}

void Peepholes::VisitASHR()
{
    std::cout << "Im here VisitASHR" << std::endl;
}

void Peepholes::VisitOR()
{
    std::cout << "Im here VisitOR" << std::endl;
}

void Peepholes::VisitUNDEFINED() {}
void Peepholes::VisitADD() {}
void Peepholes::VisitSUB() {}
void Peepholes::VisitDIV() {}
void Peepholes::VisitREM() {}
void Peepholes::VisitAND() {}
void Peepholes::VisitXOR() {}
void Peepholes::VisitSHR() {}
void Peepholes::VisitSHL() {}
void Peepholes::VisitLOAD() {}
void Peepholes::VisitSTORE() {}
void Peepholes::VisitJMP() {}
void Peepholes::VisitBEQ() {}
void Peepholes::VisitBNE() {}
void Peepholes::VisitBGT() {}
void Peepholes::VisitCALL() {}
void Peepholes::VisitRET() {}
void Peepholes::VisitPHI() {}
void Peepholes::VisitPARAMETER() {}
void Peepholes::VisitCONSTANT() {}

}  // namespace compiler
