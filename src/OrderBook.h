//
// Created by daniel on 11/23/26.
//

#ifndef ALL_APP_ORDERBOOK_H
#define ALL_APP_ORDERBOOK_H
#include <array>
#include <cstdint>
#include <variant>
#include <stdexcept>
#include "AllAppDefinitions.h"


class OrderBook {
public:
    enum class BookState {
        Empty,
        Active,
        Full,
    };

    enum class BookError {
        InvalidSide,
        InvalidInstrumentID,
        InvalidPrice,
        InvalidQuantity,
        InvalidPriceLevel,
        PriceLevelCapacityReached
    };

    struct PriceLevel {
        std::uint64_t price = 0;
        std::uint32_t quantity = 0;
    };

    using BookResult = std::variant<BookState, BookError>;

    explicit OrderBook(const AllAppDefinitions::Instrument instrumentID) {
        if (instrumentID == AllAppDefinitions::Instrument::EURUSD ||
            instrumentID == AllAppDefinitions::Instrument::GBPUSD) {
            instrumentID_ = instrumentID;
            activeAskCount_ = 0;
            activeBidCount_ = 0;
            bidLevels_ = {};
            askLevels_ = {};
            bookState_ = BookState::Empty;
        } else {
            throw std::invalid_argument("Invalid instrument ID");
        }
    }

    BookResult applyQuoteUpdate(AllAppDefinitions::Side side, AllAppDefinitions::Instrument instrumentID,
                                std::uint64_t price, std::uint32_t quantity) noexcept;

    [[nodiscard]] BookState getBookState() const noexcept {
        return bookState_;
    }

private:
    static constexpr std::size_t MAX_PRICE_LEVELS = 8;

    using PriceLevels = std::array<PriceLevel, MAX_PRICE_LEVELS>;

    PriceLevels bidLevels_;
    PriceLevels askLevels_;

    BookState bookState_;

    std::size_t activeBidCount_;
    std::size_t activeAskCount_;

    AllAppDefinitions::Instrument instrumentID_;

    [[nodiscard]] PriceLevel *findActivePriceLevel(AllAppDefinitions::Side side, std::uint64_t price) noexcept;

    BookResult addActivePriceLevel(const PriceLevel* priceLevel, AllAppDefinitions::Side side) noexcept;

    BookResult removeActivePriceLevel(const PriceLevel* priceLevel, AllAppDefinitions::Side side) noexcept;
};


#endif //ALL_APP_ORDERBOOK_H
