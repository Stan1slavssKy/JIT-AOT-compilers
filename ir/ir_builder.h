#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#include "utils/macros.h"
#include "ir/instruction.h"
#include "ir/basic_block.h"
#include "ir/instructions.h"
#include "ir/graph.h"

#include <string>
#include <vector>
#include <memory>

namespace compiler {

class IrBuilder final {
public:
    NO_COPY_SEMANTIC(IrBuilder);
    NO_MOVE_SEMANTIC(IrBuilder);

    IrBuilder(Graph *graph) : graph_(graph) {}
    ~IrBuilder() = default;

    BasicBlock *CreateBB()
    {
        auto bb = std::make_unique<BasicBlock>();
        auto bbPtr = bb.get();
        graph_->AddBlock(std::move(bb));
        return bbPtr;
    }

    void SetBasicBlockScope(BasicBlock *currentBB)
    {
        currentBB_ = currentBB;
    }

    template <typename InsnT, typename... ArgsT>
    Instruction *CreateInstruction(ArgsT &&...args)
    {
        auto insn = std::make_unique<InsnT>(std::forward<ArgsT>(args)...);
        auto insnPtr = insn.get();
        graph_->AddInstruction(std::move(insn));

        insnPtr->SetParentBB(currentBB_);
        currentBB_->PushInstruction(insnPtr);
        return insnPtr;
    }

    Instruction *CreatePhiInsn(DataType resultType);

    Instruction *CreateParameterInsn(uint32_t parameter);

    template <typename T>
    Instruction *CreateConstantInsn(T constant, DataType resultType);
    Instruction *CreateInt32ConstantInsn(uint32_t constant);
    Instruction *CreateInt64ConstantInsn(uint64_t constant);
    Instruction *CreateFloat32ConstantInsn(float constant);
    Instruction *CreateFloat64ConstantInsn(double constant);

    Instruction *CreateAddInsn(DataType resultType, Instruction *input1, Instruction *input2);
    Instruction *CreateSubInsn(DataType resultType, Instruction *input1, Instruction *input2);
    Instruction *CreateMulInsn(DataType resultType, Instruction *input1, Instruction *input2);
    Instruction *CreateDivInsn(DataType resultType, Instruction *input1, Instruction *input2);
    Instruction *CreateRemInsn(DataType resultType, Instruction *input1, Instruction *input2);

    Instruction *CreateJmpInsn(BasicBlock *bbToJmp);
    Instruction *CreateBneInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2);
    Instruction *CreateBeqInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2);
    Instruction *CreateBgtInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2);

    Instruction *CreateRetInsn(DataType retType, Instruction *input);

private:
    Graph *graph_ {nullptr};

    BasicBlock *currentBB_ {nullptr};
};

}  // namespace compiler

#endif  // IR_BUILDER_H
