#ifndef IR_INPUTS_H
#define IR_INPUTS_H

#include "utils/macros.h"

#include <array>
#include <vector>

namespace compiler {

class Instruction;
class DefaultInputs;
class VectorInputs;

class InputsContainerIface {
public:
    NO_COPY_SEMANTIC(InputsContainerIface);
    NO_MOVE_SEMANTIC(InputsContainerIface);

    InputsContainerIface() = default;
    virtual ~InputsContainerIface() = default;

    virtual bool SetInput([[maybe_unused]] Instruction *input, [[maybe_unused]] size_t idx)
    {
        return false;
    }

    virtual bool AppendInput([[maybe_unused]] Instruction *insn)
    {
        return false;
    }

    virtual Instruction *GetInput(size_t idx) = 0;
    virtual const Instruction *GetInput(size_t idx) const = 0;
    virtual void RemoveUsers(Instruction *insnToRemove) = 0;

    virtual bool SwapInputs()
    {
        return false;
    };

    virtual DefaultInputs *AsDefaultInputs()
    {
        UNREACHABLE();
    }

    virtual const DefaultInputs *AsDefaultInputs() const
    {
        UNREACHABLE();
    }

    virtual VectorInputs *AsVectorInputs()
    {
        UNREACHABLE();
    }

    virtual const VectorInputs *AsVectorInputs() const
    {
        UNREACHABLE();
    }
};

class DefaultInputs final : public InputsContainerIface {
public:
    NO_COPY_SEMANTIC(DefaultInputs);
    NO_MOVE_SEMANTIC(DefaultInputs);

    DefaultInputs() = default;
    ~DefaultInputs() = default;

    bool SetInput(Instruction *input, size_t idx) override
    {
        assert(input != nullptr);
        assert(idx < inputs_.size());
        inputs_[idx] = input;
        return true;
    }

    Instruction *GetInput(size_t idx) override
    {
        assert(idx < inputs_.size());
        return inputs_[idx];
    }

    const Instruction *GetInput(size_t idx) const override
    {
        assert(idx < inputs_.size());
        return inputs_[idx];
    }

    bool SwapInputs() override
    {
        auto *temp = inputs_[0];
        inputs_[0] = inputs_[1];
        inputs_[1] = temp;
        return true;
    }

    void RemoveUsers(Instruction *insnToRemove) override;

    DefaultInputs *AsDefaultInputs() override
    {
        return static_cast<DefaultInputs *>(this);
    }

    const DefaultInputs *AsDefaultInputs() const override
    {
        return static_cast<const DefaultInputs *>(this);
    }

private:
    std::array<Instruction *, 2U> inputs_;
};

class VectorInputs final : public InputsContainerIface {
public:
    NO_COPY_SEMANTIC(VectorInputs);
    NO_MOVE_SEMANTIC(VectorInputs);

    VectorInputs() = default;
    ~VectorInputs() = default;

    bool AppendInput(Instruction *insn) override
    {
        inputs_.push_back(insn);
        return true;
    }

    Instruction *GetInput(size_t idx) override
    {
        return inputs_[idx];
    }

    const Instruction *GetInput(size_t idx) const override
    {
        return inputs_[idx];
    }

    void RemoveUsers(Instruction *insnToRemove) override;

    VectorInputs *AsVectorInputs() override
    {
        return static_cast<VectorInputs *>(this);
    }

    const VectorInputs *AsVectorInputs() const override
    {
        return static_cast<const VectorInputs *>(this);
    }

    std::vector<Instruction *> &GetInputs()
    {
        return inputs_;
    }

    const std::vector<Instruction *> &GetInputs() const
    {
        return inputs_;
    }

private:
    std::vector<Instruction *> inputs_;
};

}  // namespace compiler

#endif  // IR_INPUTS_H
