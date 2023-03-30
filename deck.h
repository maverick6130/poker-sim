#include "card.h"
#include <vector>
#include <random>

class Deck {
    
    std::random_device mRandomDevice;
    std::mt19937 mRng;
    std::vector<Card> mCards;

public:
    Deck() : mRng( mRandomDevice() )
    {
        for (int val = 2; val < 15; ++val)
            for (int suit = 0; suit < 4; ++suit)
                mCards.push_back( Card( static_cast<Card::Value> (val), static_cast<Card::Suit> (suit) ) );
        Shuffle();
    }

    void Shuffle()
    {
        std::shuffle( std::begin(mCards), std::end(mCards), mRng );
    }

    std::vector<Card> Draw(uint8_t num)
    {
        if (num > 52)
        {
            throw std::runtime_error("Can draw at-most 52 cards");
        }

        return std::vector<Card>( begin(mCards), begin(mCards) + num );
    }
};