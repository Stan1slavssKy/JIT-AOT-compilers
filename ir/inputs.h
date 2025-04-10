#ifndef IR_INPUTS_H
#define IR_INPUTS_H

#include "utils/macros.h"

#include <array>
#include <vector>

namespace compiler {

class Instruction;

class InputsContainerIface {
public:
    NO_COPY_SEMANTIC(InputsContainerIface);
    NO_MOVE_SEMANTIC(InputsContainerIface);

    InputsContainerIface() = default;
    virtual ~InputsContainerIface() = default;

    virtual bool SetInput([[maybe_unused]] Instruction *input, [[maybe_unused]] size_t idx)
    {
        return false;
    };
    virtual bool AppendInput([[maybe_unused]] Instruction *insn)
    {
        return false;
    };

    virtual Instruction *GetInput(size_t idx) = 0;
    virtual const Instruction *GetInput(size_t idx) const = 0;

    virtual bool SwapInputs()
    {
        return false;
    };
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

private:
    std::vector<Instruction *> inputs_;
};

}  // namespace compiler

#endif  // IR_INPUTS_H
