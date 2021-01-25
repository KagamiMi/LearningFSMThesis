#pragma once

struct SeedValues {
    std::uint64_t initialSeed;
    std::uint64_t stateSeed;
    std::uint64_t actionSeed;
    std::uint64_t parentsSeed;
    std::uint64_t parentsSelSeed;
    std::uint64_t alleleSelSeed;
    std::uint64_t mutationSeed;
    std::uint64_t mutStateOrAction;
};