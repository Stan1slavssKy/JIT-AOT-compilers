#ifndef OPTIMIZATIONS_PEEPHOLES_H
#define OPTIMIZATIONS_PEEPHOLES_H

#include "utils/macros.h"
#include "ir/graph.h"

namespace compiler {

class Peepholes final {
public:
    NO_COPY_SEMANTIC(Peepholes);
    NO_MOVE_SEMANTIC(Peepholes);

    Peepholes(Graph *graph) : graph_(graph) {}
    ~Peepholes() = default;

    void Run();

#define OPCODE_MACROS(_, instrType) void Visit##instrType(Instruction *insn);
#include "ir/instruction_type.def"
#undef OPCODE_MACROS

private:
    Graph *graph_ {nullptr};

    using VisitMethodType = void (compiler::Peepholes::*)(Instruction *insn);

#define OPCODE_MACROS(_, instrType) &Peepholes::Visit##instrType,
    std::array<VisitMethodType, 23U> opcodeToVisitTable_ {
#include "ir/instruction_type.def"
    };
#undef OPCODE_MACROS
};

}  // namespace compiler

#endif  // OPTIMIZATIONS_PEEPHOLES_H
