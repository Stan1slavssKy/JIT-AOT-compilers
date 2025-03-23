#ifndef IR_IR_BUILDER_INL_H
#define IR_IR_BUILDER_INL_H

#include "ir/ir_builder.h"

namespace compiler {

template <typename InsnT, typename... ArgsT>
inline Instruction *IrBuilder::CreateInstruction(ArgsT &&...args)
{
    auto insn = std::make_unique<InsnT>(std::forward<ArgsT>(args)...);
    Instruction *insnPtr = insn.get();
    graph_->AddInstruction(std::move(insn));

    insnPtr->SetParentBB(currentBB_);
    currentBB_->PushInstruction(insnPtr);

    if (insnPtr->IsBranch()) {
        auto *branchInsn = static_cast<BranchInsn *>(insnPtr);
        auto *ifTrueBB = branchInsn->GetTrueBranchBB();
        auto *ifFalseBB = branchInsn->GetFalseBranchBB();

        currentBB_->AddSuccessor(ifTrueBB);
        currentBB_->AddSuccessor(ifFalseBB);

        ifTrueBB->AddPredecessor(currentBB_);
        ifFalseBB->AddPredecessor(currentBB_);
    }

    if (insnPtr->IsJmp()) {
        auto *jmpInsn = static_cast<JmpInsn *>(insnPtr);
        auto *bbToJmp = jmpInsn->GetBBToJmp();

        currentBB_->AddSuccessor(bbToJmp);
        bbToJmp->AddPredecessor(currentBB_);
    }

    return insnPtr;
}

inline Instruction *IrBuilder::CreatePhiInsn(DataType resultType)
{
    return CreateInstruction<PhiInsn>(resultType);
}

inline Instruction *IrBuilder::CreateParameterInsn(uint32_t parameter)
{
    return CreateInstruction<ParameterInsn>(parameter);
}

template <typename T>
inline Instruction *IrBuilder::CreateConstantInsn(T constant, DataType resultType)
{
    auto insn = CreateInstruction<ConstantInsn>(constant);
    insn->SetResultType(resultType);
    return insn;
}

inline Instruction *IrBuilder::CreateInt32ConstantInsn(uint32_t constant)
{
    return CreateConstantInsn(constant, DataType::I32);
}

inline Instruction *IrBuilder::CreateInt64ConstantInsn(uint64_t constant)
{
    return CreateConstantInsn(constant, DataType::I64);
}

inline Instruction *IrBuilder::CreateFloat32ConstantInsn(float constant)
{
    return CreateConstantInsn(constant, DataType::F32);
}

inline Instruction *IrBuilder::CreateFloat64ConstantInsn(double constant)
{
    return CreateConstantInsn(constant, DataType::F64);
}

inline Instruction *IrBuilder::CreateAddInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<AddInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateSubInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<SubInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateMulInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<MulInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateDivInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<DivInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateOrInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<DivInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateXorInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<XorInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateAshrInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<AshrInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateShrInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<ShrInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateShlInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<ShlInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateRemInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<RemInsn>(resultType, input1, input2);
}

inline Instruction *IrBuilder::CreateJmpInsn(BasicBlock *bbToJmp)
{
    return CreateInstruction<JmpInsn>(bbToJmp);
}

inline Instruction *IrBuilder::CreateBneInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BneInsn>(input1, input2, bb1, bb2);
}

inline Instruction *IrBuilder::CreateBeqInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BeqInsn>(input1, input2, bb1, bb2);
}

inline Instruction *IrBuilder::CreateBgtInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BgtInsn>(input1, input2, bb1, bb2);
}

inline Instruction *IrBuilder::CreateRetInsn(DataType retType, Instruction *input)
{
    return CreateInstruction<RetInsn>(retType, input);
}

}  // namespace compiler

#endif  // IR_IR_BUILDER_INL_H
