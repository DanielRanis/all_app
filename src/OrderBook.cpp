//
// Created by daniel on 11/23/26.
//

#include "OrderBook.h"
#include <algorithm>

using namespace AllAppDefinitions;

OrderBook::BookResult OrderBook::applyQuoteUpdate(const Side side,
                                                  const Instrument instrumentID,
                                                  const std::uint64_t price, const std::uint32_t quantity) noexcept {
    // Check that its instrumentID matches the OrderBook’s instrumentID_.
    if (instrumentID == Instrument::None || instrumentID != instrumentID_) {
        return BookError::InvalidInstrumentID;
    }
    if (price == 0) {
        return BookError::InvalidPrice;
    }
    if (side != Side::Bid && side != Side::Ask) {
        return BookError::InvalidSide;
    }
    // Choose the bid or ask array from the update’s side.
    // Find the level with the matching price.
    const auto foundPriceLevel = findActivePriceLevel(side, price);
    // Price Level not found
    if (foundPriceLevel == nullptr && quantity != 0) {
        // New Price Level, Add Price Level
        const auto newPriceLevel = PriceLevel{.price = price, .quantity = quantity};
        return addActivePriceLevel(&newPriceLevel, side);
    }
    // Existing Price Level
    if (quantity == 0) {
        // Remove Price Level
        return removeActivePriceLevel(foundPriceLevel, side);
    }

    // Update Price Level
    foundPriceLevel->price = price;
    foundPriceLevel->quantity = quantity;


    return bookState_ = BookState::Active;
}


OrderBook::PriceLevel *
OrderBook::findActivePriceLevel(const Side level_side, const std::uint64_t level_price) noexcept {
    if (level_side == Side::Bid) {
        if (const auto bidPriceLevel = std::ranges::find_if(bidLevels_.begin(),
                                                            bidLevels_.begin() + activeBidCount_,
                                                            [level_price](const PriceLevel &item) {
                                                                return item.price == level_price;
                                                            }); bidPriceLevel != bidLevels_.begin() +
                                                                activeBidCount_) {
            return bidPriceLevel;
        }
    } else {
        if (const auto askPriceLevel = std::ranges::find_if(askLevels_.begin(),
                                                            askLevels_.begin() + activeAskCount_,
                                                            [level_price](const PriceLevel &item) {
                                                                return item.price == level_price;
                                                            }); askPriceLevel != askLevels_.begin() +
                                                                activeAskCount_) {
            return askPriceLevel;
        }
    }
    return nullptr;
}

OrderBook::BookResult OrderBook::addActivePriceLevel(const PriceLevel *priceLevel,
                                                     const Side side) noexcept {
    if (priceLevel == nullptr) {
        return BookError::InvalidPriceLevel;
    }
    if (side == Side::Bid) {
        if (activeBidCount_ == MAX_PRICE_LEVELS) {
            return BookError::PriceLevelCapacityReached;
        }
        // Find Price Level Index
        std::size_t bidIndex = 0;
        while (bidIndex < activeBidCount_ && priceLevel->price < bidLevels_[bidIndex].price) {
            ++bidIndex;
        }
        // Shift one slot to the right.
        for (std::size_t i = activeBidCount_; i > bidIndex; --i) {
            bidLevels_[i] = bidLevels_[i - 1];
        }
        // Add Price Level to Position
        bidLevels_[bidIndex] = PriceLevel{.price = priceLevel->price, .quantity = priceLevel->quantity};
        ++activeBidCount_;
    } else {
        if (activeAskCount_ == MAX_PRICE_LEVELS) {
            return BookError::PriceLevelCapacityReached;
        }
        std::size_t askIndex = 0;
        while (askIndex < activeAskCount_ && priceLevel->price > askLevels_[askIndex].price) {
            ++askIndex;
        }
        for (std::size_t i = activeAskCount_; i > askIndex; --i) {
            askLevels_[i] = askLevels_[i - 1];
        }
        askLevels_[askIndex] = PriceLevel{.price = priceLevel->price, .quantity = priceLevel->quantity};
        ++activeAskCount_;
    }
    bookState_ = (activeBidCount_ == MAX_PRICE_LEVELS && activeAskCount_ == MAX_PRICE_LEVELS) ? BookState::Full : BookState::Active;
    return bookState_;
}

OrderBook::BookResult
OrderBook::removeActivePriceLevel(const PriceLevel *priceLevel, const Side side) noexcept {
    if (priceLevel == nullptr) {
        return BookError::InvalidPriceLevel;
    }
    if (side == Side::Bid) {
        if (activeBidCount_ == 0) {
            return BookError::InvalidPriceLevel;
        }
        // Find Price Level Index
        std::size_t bidIndex = 0;
        while (bidIndex < activeBidCount_ && priceLevel->price != bidLevels_[bidIndex].price) {
            ++bidIndex;
        }
        // Shift elements to index.
        for (std::size_t i = bidIndex; i + 1 < activeBidCount_; ++i) {
            bidLevels_[i] = bidLevels_[i + 1];
        }
        // Delete Price Level at Position
        bidLevels_[activeBidCount_ - 1] = PriceLevel{};
        --activeBidCount_;
    } else {
        if (activeAskCount_ == 0) {
            return BookError::InvalidPriceLevel;
        }
        std::size_t askIndex = 0;
        while (askIndex < activeAskCount_ && priceLevel->price != askLevels_[askIndex].price) {
            ++askIndex;
        }
        for (std::size_t i = askIndex; i + 1 < activeAskCount_; ++i) {
            askLevels_[i] = askLevels_[i + 1];
        }
        askLevels_[activeAskCount_ - 1] = PriceLevel{};
        --activeAskCount_;
    }
    bookState_ = (activeBidCount_ == 0 && activeAskCount_ == 0) ? BookState::Empty : BookState::Active;
    return bookState_;
}
