#ifndef IR_IR_BUILDER_INL_H
#define IR_IR_BUILDER_INL_H

#include "ir/ir_builder.h"

namespace compiler {

Instruction *IrBuilder::CreatePhiInsn(DataType resultType)
{
    return CreateInstruction<PhiInsn>(resultType);
}

Instruction *IrBuilder::CreateParameterInsn(uint32_t parameter)
{
    return CreateInstruction<ParameterInsn>(parameter);
}

template <typename T>
Instruction *IrBuilder::CreateConstantInsn(T constant, DataType resultType)
{
    auto insn = CreateInstruction<ConstantInsn>(constant);
    insn->SetResultType(resultType);
    return insn;
}

Instruction *IrBuilder::CreateInt32ConstantInsn(uint32_t constant)
{
    return CreateConstantInsn(constant, DataType::I32);
}

Instruction *IrBuilder::CreateInt64ConstantInsn(uint64_t constant)
{
    return CreateConstantInsn(constant, DataType::I64);
}

Instruction *IrBuilder::CreateFloat32ConstantInsn(float constant)
{
    return CreateConstantInsn(constant, DataType::F32);
}

Instruction *IrBuilder::CreateFloat64ConstantInsn(double constant)
{
    return CreateConstantInsn(constant, DataType::F64);
}

Instruction *IrBuilder::CreateAddInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<AddInsn>(resultType, input1, input2);
}

Instruction *IrBuilder::CreateSubInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<SubInsn>(resultType, input1, input2);
}

Instruction *IrBuilder::CreateMulInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<MulInsn>(resultType, input1, input2);
}

Instruction *IrBuilder::CreateDivInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<DivInsn>(resultType, input1, input2);
}

Instruction *IrBuilder::CreateRemInsn(DataType resultType, Instruction *input1, Instruction *input2)
{
    return CreateInstruction<RemInsn>(resultType, input1, input2);
}

Instruction *IrBuilder::CreateJmpInsn(BasicBlock *bbToJmp)
{
    return CreateInstruction<JmpInsn>(bbToJmp);
}

Instruction *IrBuilder::CreateBneInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BneInsn>(input1, input2, bb1, bb2);
}

Instruction *IrBuilder::CreateBeqInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BeqInsn>(input1, input2, bb1, bb2);
}

Instruction *IrBuilder::CreateBgtInsn(Instruction *input1, Instruction *input2, BasicBlock *bb1, BasicBlock *bb2)
{
    return CreateInstruction<BgtInsn>(input1, input2, bb1, bb2);
}

Instruction *IrBuilder::CreateRetInsn(DataType retType, Instruction *input)
{
    return CreateInstruction<RetInsn>(retType, input);
}

}  // namespace compiler

#endif  // IR_IR_BUILDER_INL_H
