#ifndef TESTS_TEST_HELPER
#define TESTS_TEST_HELPER

#include <gtest/gtest.h>
#include "ir/graph.h"

namespace compiler::tests {

// For debug purposes.
inline void DumpGraph(Graph &graph)
{
    std::stringstream ss;
    graph.Dump(ss);
    std::cout << ss.str() << std::endl;
}

template <size_t expectedNumInputs>
inline void CompareInputs(Instruction *insnToCheck, std::array<Instruction *, expectedNumInputs> expectedInputs)
{
    ASSERT_NE(insnToCheck, nullptr);

    if (insnToCheck->HasVectorInputs()) {
        auto &inputs = insnToCheck->GetInputs()->AsVectorInputs()->GetInputs();
        ASSERT_EQ(inputs.size(), expectedInputs.size());

        for (size_t idx = 0; idx < inputs.size(); ++idx) {
            ASSERT_EQ(inputs[idx], expectedInputs[idx]);
        }
    } else {
        ASSERT_EQ(expectedInputs.size(), 2);
        ASSERT_EQ(insnToCheck->GetInputs()->GetInput(0), expectedInputs[0]);
        ASSERT_EQ(insnToCheck->GetInputs()->GetInput(1), expectedInputs[1]);
    }
}

}  // namespace compiler::tests

#endif  // TESTS_TEST_HELPER
