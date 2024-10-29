#ifndef ANALISYS_LOOP_H
#define ANALISYS_LOOP_H

#include <vector>

namespace compiler {
    
class BasicBlock;

class Loop final {
public:
    Loop() {}

    void MarkAsRoot()
    {
        isRoot_ = true;
    }

private:
    BasicBlock *header_;
    std::vector<BasicBlock *> blocks_;
    std::vector<BasicBlock *> backEdges_;

    Loop *outerLoop_ {nullptr};

    bool isReducible_ {false};
    bool isRoot_ {false};
};

} // namespace compiler

#endif // ANALISYS_LOOP_H
