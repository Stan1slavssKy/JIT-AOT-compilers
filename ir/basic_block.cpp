#include "ir/basic_block.h"
#include "ir/graph.h"
#include "analysis/loop.h"

#include <iomanip>

namespace compiler {

void BasicBlock::Dump(std::stringstream &ss) const
{
    ss << "BB_" << bbId_ << ":\n";

    auto currInsn = firstInsn_;
    while (currInsn != nullptr) {
        ss << std::setw(4);
        currInsn->Dump(ss);
        ss << "\n";
        currInsn = currInsn->GetNext();
    }
}

bool BasicBlock::IsHeader() const
{
    return loop_->GetHeader() == this;
}

void BasicBlock::SetMarker(Marker marker)
{
    uint32_t markerValue = marker >> MarkerManager::COLOR_BITS;
    uint32_t colorIdx = marker & MarkerManager::COLOR_MASK;
    assert(colorIdx < MarkerManager::COLORS_NUM);
    markers_[colorIdx] = markerValue;
}

void BasicBlock::EraseMarker(Marker marker)
{
    uint32_t colorIdx = marker & MarkerManager::COLOR_MASK;
    assert(colorIdx < MarkerManager::COLORS_NUM);
    markers_[colorIdx] = MarkerManager::UNMARKER;
}

bool BasicBlock::IsMarked(Marker marker) const
{
    uint32_t markerValue = marker >> MarkerManager::COLOR_BITS;
    uint32_t colorIdx = marker & MarkerManager::COLOR_MASK;
    assert(colorIdx < MarkerManager::COLORS_NUM);
    return markers_[colorIdx] == markerValue;
}

void BasicBlock::ClearMarkers()
{
    for (auto &marker : markers_) {
        marker = MarkerManager::UNMARKER;
    }
}

void BasicBlock::PushInstruction(Instruction *insn)
{
    if (firstInsn_ == nullptr) {
        firstInsn_ = insn;
        lastInsn_ = firstInsn_;
    } else {
        lastInsn_->SetNext(insn);
        insn->SetPrev(lastInsn_);
        lastInsn_ = insn;
    }
}

/// `prevInsn` -- instruction after which `insn` will be inserted
/// `prevInsn` must be pass as nullpt to insert `insn` in the beginning
void BasicBlock::InsertInstruction(Instruction *prevInsn, Instruction *insn)
{
    if (prevInsn == lastInsn_) {
        PushInstruction(insn);
        return;
    }

    if (prevInsn == nullptr) {
        firstInsn_->SetPrev(insn);
        insn->SetNext(firstInsn_);
        insn->SetPrev(nullptr);
        firstInsn_ = insn;
        return;
    }

    auto *next = prevInsn->GetNext();
    assert(next != nullptr);

    next->SetPrev(insn);
    insn->SetNext(next);

    insn->SetPrev(prevInsn);
    prevInsn->SetNext(insn);
}

void BasicBlock::Remove(Instruction *insnToRemove)
{
    assert(insnToRemove != nullptr);

    if (insnToRemove == firstInsn_) {
        firstInsn_ = insnToRemove->GetNext();
        firstInsn_->SetPrev(nullptr);
    } else if (insnToRemove == lastInsn_) {
        lastInsn_ = insnToRemove->GetPrev();
        lastInsn_->SetNext(nullptr);
    } else {
        auto *prevInsn = insnToRemove->GetPrev();
        auto *nextInsn = insnToRemove->GetNext();

        prevInsn->SetNext(nextInsn);
        nextInsn->SetPrev(prevInsn);
    }

    insnToRemove->GetInputs()->RemoveUsers(insnToRemove);
}

}  // namespace compiler
