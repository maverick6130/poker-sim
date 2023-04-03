#include "card.h"
#include <vector>
#include <bitset>
#include <sstream>
#include <tuple>

class Rank
{
private:
    enum class Type {
        HIGH_CARD,
        PAIR,
        TWO_PAIR,
        TRIPS,
        STRAIGHT,
        FLUSH,
        BOAT,
        QUADS,
        STRAIGHT_FLUSH
    };
    static inline const char* cards[15] = { 
        "INVALID", "Ace", "Deuce", "Three", "Four", "Five", "Six", "Seven",
        "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"
    };

    Type mType;
    int mHigh;

public:

    Rank( const std::vector<Card>& cards )
    {

        int suits[4] = {0};
        int values[15] = {0};
        for(Card card : cards)
        {
            int val = card.GetValue();
            suits[card.GetSuit()] |= ( (1 << val) + (static_cast<Card::Value>(val) == Card::Value::ACE ? 2 : 0) );
            values[val]++;
        }

        std::tie(mType, mHigh) = CheckValueHand(values);

        int potentialHigh = 0;
        if ( (potentialHigh = CheckStraightFlush(suits)) )
        {
            mType =  Type::STRAIGHT_FLUSH;
            mHigh = potentialHigh;
        }

        if ( static_cast<int>(mType) <= static_cast<int>(Type::TRIPS) )
        {
            if ( (potentialHigh = CheckFlush(suits)) )
            {
                mType = Type::FLUSH;
                mHigh = potentialHigh;
            }
            if ( ! potentialHigh && ( potentialHigh = CheckStraight(suits) ) )
            {
                mType = Type::STRAIGHT;
                mHigh = potentialHigh;
            }
        }

    }

    int CheckStraightFlush(int *suits)
    {
        for (int i = 0; i < 4; ++i)
        {
            int res = CheckStraightBits(suits[i]);
            if ( res )
                return res;
        }
        return 0;
    }

    int CheckFlush(int* suits)
    {
        int high = 0;
        for (int i = 0; i < 4; ++i)
        {
            int copy = suits[i] & ( ( (1 << 16) - 1 ) << 2 );
            int count = std::bitset<32>(copy).count();

            if (count < 5)
                continue;

            while(count > 5)
            {
                copy = copy & (copy - 1);
                count --;
            }
            high = std::max(high, copy);
        }
        return high;
    }

    int CheckStraight(int* suits)
    {
        return CheckStraightBits( suits[0] | suits[1] | suits[2] | suits[3] );
    }

    int CheckStraightBits(int hand)
    {
        constexpr int fiveRow = 31;
        for(int shift = 10; shift > 0; shift --)
        {
            if ( ((hand >> shift) & fiveRow) == fiveRow )
                return shift;
        }
        return 0;
    }

    std::pair<Rank::Type, int> CheckValueHand(int* values)
    {
        int maxFreqElement = 15;
        int maxFreq = -1;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] > maxFreq)
            {
                maxFreq = values[i];
                maxFreqElement = i;
            }
        }

        int high = 0;
        Type type;
        switch (maxFreq)
        {
        case 4:
            high = CheckQuads(values, maxFreqElement);
            type = Type::QUADS;
            break;
        
        case 3:
            std::tie(type, high) = CheckBoatTrips(values, maxFreqElement);
            break;

        case 2:
            std::tie(type, high) = CheckPairTwoPair(values, maxFreqElement);
            break;

        case 1:
            high = CheckHighCard(values);
            type = Type::HIGH_CARD;
            break;

        default:
            break;
        }

        return {type, high};
    }

    int CheckQuads(int* values, int quad)
    {
        int high = 0;
        for (int i  = 14; i > 1; --i)
        {
            if ( values[i] > 0 && quad != i )
            {
                high = i;
                break;
            }
        }

        return (quad << 4) | high;
    }

    std::pair<Rank::Type, int> CheckBoatTrips(int* values, int trips)
    {
        values[trips] = 0;

        int maxSecondFreqElement = 15;
        int maxSecondFreq = -1;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] > maxSecondFreq)
            {
                maxSecondFreq = values[i];
                maxSecondFreqElement = i;
            }
        }

        int high;
        Type type;
        if (maxSecondFreq >= 2)
        {
            high = (trips << 4) | maxSecondFreqElement;
            type = Type::BOAT;
        }
        else
        {
            high = (trips << 8) | GetHighCards(values, 2);
            type = Type::TRIPS;
        }

        values[trips] = 3;

        return { type, high };
    }

    std::pair<Rank::Type, int> CheckPairTwoPair(int* values, int pair)
    {
        values[pair] = 0;

        int maxSecondFreqElement = 15;
        int maxSecondFreq = -1;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] > maxSecondFreq)
            {
                maxSecondFreq = values[i];
                maxSecondFreqElement = i;
            }
        }

        int high;
        Type type;
        if (maxSecondFreq == 2)
        {
            values[maxSecondFreqElement] = 0;
            high = (pair << 8) | (maxSecondFreqElement << 4) | GetHighCards(values, 1) ;
            values[maxSecondFreqElement] = maxSecondFreq;
            type = Type::TWO_PAIR;
        }
        else
        {
            high = (pair << 12) | GetHighCards(values, 3);
            type = Type::PAIR;
        }

        values[pair] = 2;

        return { type, high };
    }

    int CheckHighCard(int *values)
    {
        return GetHighCards(values, 5);
    }

    int GetHighCards(int * values, int count)
    {
        int high[13] = {0};

        int x = 0;
        for (int i  = 14; i > 1; --i)
        {
            if ( values[i] > 0 )
            {
                high[x++] = i;
                if (x == count)
                    break;
            }
        }

        int ret = 0;
        for (int i = 0; i < x; ++i)
        {
            ret <<= 4;
            ret |= high[i];
        }

        return ret;
    }
    
    std::string Verbose()
    {
        std::stringstream s;
        s << static_cast<int>(mType) << " " << mHigh << " ";
        switch (mType)
        {
        case Type::HIGH_CARD:
            s << "High Cards ";
            for (int shift = 16; shift >= 0; shift -= 4)
                s << cards[(mHigh >> shift) & 15] << " ";
            s << "\n";
            break;
        
        case Type::PAIR:
            s << "Pair of " << cards[(mHigh >> 12)] << "s with high cards ";
            for (int shift = 8; shift >= 0; shift -= 4)
                s << cards[(mHigh >> shift) & 15] << " ";
            s << "\n";
            break;

        case Type::TWO_PAIR:
            s << "Two Pair " << cards[(mHigh >> 8)] 
              << "s and " << cards[(mHigh >> 4) & 15] << "s with ";
            s << cards[mHigh & 15] << " high\n";
            break;

        case Type::TRIPS:
            s << "A set of " << cards[(mHigh >> 8)] << "s with "
              << cards[(mHigh >> 4) & 15] << " " << cards[mHigh & 15] << " high\n";
            break;

        case Type::STRAIGHT:
            s << "Straight from " << cards[mHigh] << " to " << cards[mHigh + 4] << "\n";
            break;
        
        case Type::FLUSH:
            s << "Flush with high cards " << std::bitset<16>(mHigh) << "\n";
            break;
        
        case Type::BOAT:
            s << cards[(mHigh >> 4)] << "s full of " << cards[mHigh & 15] << "s\n";
            break;

        case Type::QUADS:
            s << "Quad " << cards[(mHigh>>4)] << "s with kicker " << cards[mHigh & 15] << "\n";
            break;
        
        case Type::STRAIGHT_FLUSH:
            s << "Straight flush from " << cards[mHigh] << " to " << cards[mHigh + 4] << "\n";
            break;

        default:
            break;
        }

        return s.str();
    }

};