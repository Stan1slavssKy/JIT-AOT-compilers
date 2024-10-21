#include "ir/basic_block.h"

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

}  // namespace compiler
