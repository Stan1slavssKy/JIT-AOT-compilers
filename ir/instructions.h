#ifndef IR_INSTRUCTIONS_H
#define IR_INSTRUCTIONS_H

#include "ir/data_types.h"
#include "ir/inputs.h"
#include "ir/instruction.h"
#include "utils/bit_utils.h"

#include <cmath>
#include <unordered_map>

namespace compiler {

class UndefinedInsn final : public Instruction {
public:
    UndefinedInsn() : Instruction(Opcode::UNDEFINED) {}
};

class ParameterInsn final : public Instruction {
public:
    ParameterInsn(uint32_t argNum) : Instruction(Opcode::PARAMETER, DataType::U32), argNum_(argNum) {}

    uint32_t GetArgNum() const
    {
        return argNum_;
    }

    void Dump(std::stringstream &ss) const override;

private:
    uint32_t argNum_ {0};
};

// Available types: integer types, float32, float64.
class ConstantInsn final : public Instruction {
public:
    template <typename T>
    ConstantInsn(T value, DataType resultType) : Instruction(Opcode::CONSTANT)
    {
        if constexpr (std::is_integral_v<T>) {
            value_ = value;
            if constexpr (std::is_signed_v<T>) {
                type_ = DataType::I64;
            } else {
                type_ = DataType::U64;
            }
        } else if constexpr (std::is_same_v<T, float>) {
            value_ = utils::bit_cast<uint32_t, float>(value);
            type_ = DataType::F32;
        } else if constexpr (std::is_same_v<T, double>) {
            value_ = utils::bit_cast<uint64_t, double>(value);
            type_ = DataType::F64;
        } else {
            UNREACHABLE();
        }
        SetResultType(resultType);
    }

    bool IsSignedInt() const
    {
        return type_ == DataType::I64;
    }

    bool IsUnsignedInt() const
    {
        return type_ == DataType::U64;
    }

    bool IsF32() const
    {
        return type_ == DataType::F32;
    }

    bool IsF64() const
    {
        return type_ == DataType::F64;
    }

    float GetAsF32() const
    {
        return utils::bit_cast<float, uint32_t>(static_cast<uint32_t>(value_));
    }

    double GetAsF64() const
    {
        return utils::bit_cast<double, uint64_t>(value_);
    }

    int64_t GetAsI64() const
    {
        return static_cast<int64_t>(value_);
    }

    uint64_t GetAsU64() const
    {
        return value_;
    }

    bool IsEqual(float value) const
    {
        if (type_ != DataType::F32) {
            return false;
        }

        float rhsValue = GetAsF32();

        if (std::isnan(value) && std::isnan(rhsValue)) {
            return true;
        }

        return value == rhsValue;
    }

    bool IsEqual(double value) const
    {
        if (type_ != DataType::F64) {
            return false;
        }

        double rhsValue = GetAsF64();

        if (std::isnan(value) && std::isnan(rhsValue)) {
            return true;
        }

        return value == rhsValue;
    }

    bool IsEqual(int64_t value) const
    {
        if (type_ != DataType::I64 && type_ != DataType::U64) {
            return false;
        }

        int64_t rhsValue = GetAsI64();
        return value == rhsValue;
    }

    bool IsEqualTo(int64_t value) const
    {
        return IsEqual(value) || IsEqual(static_cast<float>(value)) || IsEqual(static_cast<double>(value));
    }

    DataType GetType() const
    {
        return type_;
    }

    void Dump(std::stringstream &ss) const override;

private:
    DataType type_ {DataType::UNDEFINED};

    uint64_t value_ {0};
};

class PhiInsn final : public Instruction {
public:
    using ValueToBBMap = std::unordered_map<Instruction *, std::list<BasicBlock *>>;

    PhiInsn(DataType resultType) : Instruction(Opcode::PHI, resultType) {}

    ValueToBBMap &GetDependenciesMap()
    {
        return dependencies_;
    }

    const ValueToBBMap &GetDependenciesMap() const
    {
        return dependencies_;
    }

    void ResolveDependency(Instruction *value, BasicBlock *bb)
    {
        auto it = dependencies_.find(value);
        if (it == dependencies_.end()) {
            GetInputs()->AppendInput(value);
            value->AddUser(this);
        }
        dependencies_[value].push_back(bb);
    }

    bool ReplaceDependency(Instruction *oldValue, Instruction *newValue)
    {
        auto node = dependencies_.extract(oldValue);
        if (node.empty()) {
            return false;
        }

        auto nodeWithNewValue = dependencies_.find(newValue);
        if (nodeWithNewValue == dependencies_.end()) {
            node.key() = newValue;
            dependencies_.insert(std::move(node));
        } else {
            auto &oldValueBBVec = node.mapped();
            dependencies_[newValue].merge(std::move(oldValueBBVec));
        }

        return true;
    }

    VectorInputs *GetInputs()
    {
        return static_cast<VectorInputs *>(Instruction::GetInputs());
    }

    const VectorInputs *GetInputs() const
    {
        return static_cast<const VectorInputs *>(Instruction::GetInputs());
    }

    void Dump(std::stringstream &ss) const override;

private:
    ValueToBBMap dependencies_;
};

class ArithmeticInsn : public Instruction {
public:
    ArithmeticInsn(Opcode opcode, DataType resultType, Instruction *input1, Instruction *input2)
        : Instruction(opcode, resultType)
    {
        GetInputs()->SetInput(input1, 0);
        GetInputs()->SetInput(input2, 1);
        input1->AddUser(this);
        if (input1 == input2) {
            return;
        }
        input2->AddUser(this);
    }

    void Dump(std::stringstream &ss) const override;
};

class AddInsn final : public ArithmeticInsn {
public:
    AddInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::ADD, resultType, input1, input2)
    {
    }
};

class SubInsn final : public ArithmeticInsn {
public:
    SubInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::SUB, resultType, input1, input2)
    {
    }
};

class MulInsn final : public ArithmeticInsn {
public:
    MulInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::MUL, resultType, input1, input2)
    {
    }
};

class DivInsn final : public ArithmeticInsn {
public:
    DivInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::DIV, resultType, input1, input2)
    {
    }
};

class RemInsn final : public ArithmeticInsn {
public:
    RemInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::REM, resultType, input1, input2)
    {
    }
};

class AndInsn final : public ArithmeticInsn {
public:
    AndInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::ADD, resultType, input1, input2)
    {
    }
};

class OrInsn final : public ArithmeticInsn {
public:
    OrInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::OR, resultType, input1, input2)
    {
    }
};

class XorInsn final : public ArithmeticInsn {
public:
    XorInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::XOR, resultType, input1, input2)
    {
    }
};

class AshrInsn final : public ArithmeticInsn {
public:
    AshrInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::ASHR, resultType, input1, input2)
    {
    }
};

class ShrInsn final : public ArithmeticInsn {
public:
    ShrInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::SHR, resultType, input1, input2)
    {
    }
};

class ShlInsn final : public ArithmeticInsn {
public:
    ShlInsn(DataType resultType, Instruction *input1, Instruction *input2)
        : ArithmeticInsn(Opcode::SHL, resultType, input1, input2)
    {
    }
};

class JmpInsn final : public Instruction {
public:
    JmpInsn(BasicBlock *bbToJmp) : Instruction(Opcode::JMP, DataType::VOID), bbToJmp_(bbToJmp) {}

    BasicBlock *GetBBToJmp() const
    {
        return bbToJmp_;
    }

    void Dump(std::stringstream &ss) const override;

private:
    BasicBlock *bbToJmp_ {nullptr};
};

class BranchInsn : public Instruction {
public:
    BranchInsn(Opcode opcode, Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : Instruction(opcode, DataType::VOID), ifTrueBB_(ifTrueBB), ifFalseBB_(ifFalseBB)
    {
        GetInputs()->SetInput(input1, 0);
        GetInputs()->SetInput(input2, 1);
        input1->AddUser(this);
        if (input1 == input2) {
            return;
        }
        input2->AddUser(this);
    }

    BasicBlock *GetTrueBranchBB() const
    {
        return ifTrueBB_;
    }

    BasicBlock *GetFalseBranchBB() const
    {
        return ifFalseBB_;
    }

    void Dump(std::stringstream &ss) const override;

private:
    BasicBlock *ifTrueBB_ {nullptr};
    BasicBlock *ifFalseBB_ {nullptr};
};

class BgtInsn final : public BranchInsn {
public:
    BgtInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BGT, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class BeqInsn final : public BranchInsn {
public:
    BeqInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BEQ, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class BneInsn final : public BranchInsn {
public:
    BneInsn(Instruction *input1, Instruction *input2, BasicBlock *ifTrueBB, BasicBlock *ifFalseBB)
        : BranchInsn(Opcode::BNE, input1, input2, ifTrueBB, ifFalseBB)
    {
    }
};

class RetInsn final : public Instruction {
public:
    RetInsn(DataType retType, Instruction *input) : Instruction(Opcode::RET, retType), retValue_(input)
    {
        GetInputs()->SetInput(input, 0);
        input->AddUser(this);
    }

    void Dump(std::stringstream &ss) const override;

private:
    Instruction *retValue_ {nullptr};
};

class CallStaticInsn final : public Instruction {
public:
    CallStaticInsn(DataType retType, size_t methodId, std::initializer_list<std::pair<Instruction *, DataType>> inputs)
        : Instruction(Opcode::CALLSTATIC, retType), arguments_(std::move(inputs)), methodId_(methodId)
    {
        for (auto &argPair : arguments_) {
            auto *input = argPair.first;
            GetInputs()->AppendInput(input);
            input->AddUser(this);
        }
    }

    const std::initializer_list<std::pair<Instruction *, DataType>> &GetArgument() const
    {
        return arguments_;
    }

    size_t GetMethodId() const
    {
        return methodId_;
    }

    void Dump(std::stringstream &ss) const override;

private:
    std::initializer_list<std::pair<Instruction *, DataType>> arguments_;
    size_t methodId_ {0};
};

}  // namespace compiler

#endif  // IR_INSTRUCTIONS_H
