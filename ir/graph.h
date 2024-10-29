#ifndef IR_GRAPH_H
#define IR_GRAPH_H

#include "utils/macros.h"

#include <memory>
#include <vector>

#include "ir/basic_block.h"
#include "ir/instruction.h"

#include "analisys/loop.h"

namespace compiler {

class Graph final {
public:
    NO_COPY_SEMANTIC(Graph);
    NO_MOVE_SEMANTIC(Graph);

    Graph() = default;
    ~Graph() = default;

    void AddBlock(std::unique_ptr<BasicBlock> block);
    void AddInstruction(std::unique_ptr<Instruction> insn);

    BasicBlock *GetStartBlock() const;

    size_t GetAliveBlockCount() const;

    void RunRpo();

    void UnmarkRPOVector();

    std::vector<BasicBlock *> &GetRpoVector();
    const std::vector<BasicBlock *> &GetRpoVector() const;

    void BuildDominatorTree();

    void SetRootLoop(std::unique_ptr<Loop> rootLoop);
    Loop *GetRootLoop() const;

    void Dump(std::stringstream &ss) const;

private:
    // Graph owns all basic blocks and instruction of the current function.
    std::vector<std::unique_ptr<BasicBlock>> basicBlocks_;
    std::vector<std::unique_ptr<Instruction>> instructions_;

    std::vector<BasicBlock *> rpoVector_;

    std::unique_ptr<Loop> rootLoop_;
};

}  // namespace compiler

#endif  // IR_GRAPH_H
