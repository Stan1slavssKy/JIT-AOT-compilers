#include "ir/basic_block.h"
#include "ir/graph.h"
#include "analisys/loop.h"

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

void BasicBlock::Unmark()
{
    marker_ = graph_->GetEmptyMarker();
}

bool BasicBlock::IsHeader() const
{
    return loop_->GetHeader() == this;
}

}  // namespace compiler
