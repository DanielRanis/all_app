//
// Created by daniel on 11/23/26.
//

#ifndef ALL_APP_ALLAPPDEFINITIONS_H
#define ALL_APP_ALLAPPDEFINITIONS_H
#include <cstdint>

namespace AllAppDefinitions {
    enum class Side : std::uint8_t {
        None = 0,
        Bid = 1,
        Ask = 2,
    };

    enum class Instrument : std::uint32_t {
        None = 0,
        EURUSD = 1,
        GBPUSD = 2
    };
}

#endif //ALL_APP_ALLAPPDEFINITIONS_H
