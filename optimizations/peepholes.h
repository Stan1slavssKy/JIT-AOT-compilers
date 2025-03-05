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

#define _(instrType) void Visit##instrType();
#include "ir/instruction_type.def"
#undef _

private:
    Graph *graph_ {nullptr};

    using VisitMethodType = void (compiler::Peepholes::*)();

#define _(instrType) &Peepholes::Visit##instrType,
    std::array<VisitMethodType, 23U> opcodeToVisitTable_ {
#include "ir/instruction_type.def"
    };
#undef _
};

}  // namespace compiler

#endif  // OPTIMIZATIONS_PEEPHOLES_H
