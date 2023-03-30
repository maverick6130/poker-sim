#include "card.h"
#include <vector>
#include <bitset>
#include <sstream>

class Rank
{
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

        int straightFlush = CheckStraightFlush(suits);
        if ( straightFlush )
        {
            mType =  Type::STRAIGHT_FLUSH;
            mHigh = straightFlush;
        }
        else
        {
            int quads = CheckQuads(values);
            if (quads)
            {
                mType = Type::QUADS;
                mHigh = quads;
            }
            else
            {
                int boat = CheckBoat(values);
                if (boat)
                {
                    mType = Type::BOAT;
                    mHigh = boat;
                }
                else
                {
                    int flush = CheckFlush(suits);
                    if (flush)
                    {
                        mType = Type::FLUSH;
                        mHigh = flush;
                    }
                    else
                    {
                        int straight = CheckStraight(suits);
                        if (straight)
                        {
                            mType = Type::STRAIGHT;
                            mHigh = straight;
                        }
                        else
                        {
                            int trips = CheckTrips(values);
                            if (trips)
                            {
                                mType = Type::TRIPS;
                                mHigh = trips;
                            }
                            else
                            {
                                int two_pair = CheckTwoPair(values);
                                if (two_pair)
                                {
                                    mType = Type::TWO_PAIR;
                                    mHigh = two_pair;
                                }
                                else
                                {
                                    int pair = CheckPair(values);
                                    if (pair)
                                    {
                                        mType = Type::PAIR;
                                        mHigh = pair;
                                    }
                                    else
                                    {
                                        mType = Type::HIGH_CARD;
                                        mHigh = CheckHighCard(values);
                                    }
                                }
                            }
                        }
                    }
                }
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
    }

    int CheckQuads(int* values)
    {
        int quad = 0;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] >= 4)
            {
                quad = i;
                break;
            }
        }

        int high = 0;
        if (quad)
        {
            for (int i  = 14; i > 1; --i)
            {
                if ( values[i] > 0 && quad != i )
                {
                    high = i;
                    break;
                }
            }
        }

        return (quad << 4) + high;
    }

    int CheckBoat(int* values)
    {
        int boat = 0;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] >= 3)
            {
                boat = i;
                break;
            }
        }

        int full = 0;
        if (boat)
        {
            for (int i  = 14; i > 1; --i)
            {
                if ( values[i] >= 2 && boat != i )
                {
                    full = i;
                    break;
                }
            }
        }

        if (boat && full)
            return (boat << 4) + full;

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

    int CheckTrips(int* values)
    {
        int trips = 0;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] >= 3)
            {
                trips = i;
                break;
            }
        }

        int high1 = 0, high2 = 0;
        if (trips)
        {
            for (int i  = 14; i > 1; --i)
            {
                if ( values[i] > 0 && trips != i )
                {
                    if (high1 == 0)
                        high1 = i;
                    else
                    {
                        high2 = i;
                        break;
                    }
                }
            }
        }

        return (trips << 8) | (high1 << 4) | high2;
    }

    int CheckTwoPair(int* values)
    {
        int pair1, pair2 = 0;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] >= 2)
            {
                if (pair1 == 0)
                    pair1 = i;
                else
                {
                    pair2 = i;
                    break;
                }
            }
        }

        int high = 0;
        if (pair1 & pair2)
        {
            for (int i  = 14; i > 1; --i)
            {
                if ( values[i] > 0 && pair1 != i & pair2 != i )
                {
                    high = i;
                    break;
                }
            }
        }

        return (pair1 << 8) | (pair2 << 4) | high;
    }

    int CheckPair(int * values)
    {
        int pair = 0;
        for (int i = 14; i > 1; --i)
        {
            if (values[i] >= 2)
            {
                pair = i;
            }
        }

        int high[3] = {0};

        int x = 0;
        if (pair)
        {
            for (int i  = 14; i > 1; --i)
            {
                if ( values[i] > 0 && pair != i )
                {
                    high[x++] = i;
                    if (x == 3)
                        break;
                }
            }
        }

        return (pair << 12) | (high[0] << 8) | (high[1] << 4) | high[2];   
    }

    int CheckHighCard(int * values)
    {
        int high[5] = {0};

        int x = 0;
        for (int i  = 14; i > 1; --i)
        {
            if ( values[i] > 0 )
            {
                high[x++] = i;
                if (x == 5)
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

    int CheckStraightBits(int hand)
    {
        int fiveRow = (( 1 << 6 ) - 1 );
        for(int shift = 10; shift > 0; shift --)
        {
            std::bitset<32> straight ( hand & (fiveRow << shift) );
            if (straight.count() == 5)
            {
                return static_cast<int> (straight.to_ulong());
            }
        }
        return 0;
    }
    
    std::string Verbose()
    {
        std::stringstream s;
        switch (mType)
        {
        case Type::HIGH_CARD:
            s << "High Card ";
            for (int shift = 16; shift >= 0; shift -= 4)
                s << cards[(mHigh >> shift) & 15];
            break;
        default:
            break;
        }
    }

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
        "Ace", "King", "Queen", "Jack", "Ten", "Nine", "Eight", "Seven",
         "Six", "Five", "Four", "Three", "Deuce", "Ace", "INVALID"  };

    Type mType;
    int mHigh;

};