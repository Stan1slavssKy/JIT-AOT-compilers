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

}  // namespace compiler
