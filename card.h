#include <string>
#include <string_view>
#include <unordered_map>

class Card
{
public:
    enum class Suit
    {
        SPADES,
        CLUBS,
        DIAMONDS,
        HEARTS
    };

    enum class Value
    {
        DEUCE = 2,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK,
        QUEEN,
        KING,
        ACE
    };
    Card(const std::string_view name)
    {
        if( name.length() != 2 )
        {
            std::string err = "Card string must be of length 2, not ";
            err += name;
            throw std::runtime_error (err);
        }

        auto v = valueMap.find(name[0]);
        if ( v == valueMap.end() )
        {
            std::string err = "Card value contains illegal character, ";
            err += name;
            throw std::runtime_error (err);
        }
        Value value = v->second;

        auto s = suitMap.find(name[1]);
        if ( s == suitMap.end() )
        {
            std::string err = "Card suit contains illegal character, ";
            err += name;
            throw std::runtime_error (err);
        }
        Suit suit = s->second;

        mId = InitializeCard(value, suit);
    }

    Card(Value value, Suit suit)
    {
        mId = InitializeCard(value, suit);
    }

    uint32_t GetValue()
    {
        return mId >> SHIFT;
    }

    uint32_t GetSuit()
    {
        return mId & ( (1 << SHIFT) - 1 );
    }

    std::string GetStr()
    {
        std::string str(2, 'X');
        str[0] = revValueMap[ GetValue() ];
        str[1] = revSuitMap[ GetSuit() ];
        return str;
    }

private:

    static inline const std::unordered_map<char, Suit> suitMap {
        {'s', Suit::SPADES},
        {'c', Suit::CLUBS},
        {'d', Suit::DIAMONDS},
        {'h', Suit::HEARTS}
    };

    static inline const std::unordered_map<char, Value> valueMap {
        {'A', Value::ACE},
        {'K', Value::KING},
        {'Q', Value::QUEEN},
        {'J', Value::JACK},
        {'T', Value::TEN},
        {'9', Value::NINE},
        {'8', Value::EIGHT},
        {'7', Value::SEVEN},
        {'6', Value::SIX},
        {'5', Value::FIVE},
        {'4', Value::FOUR},
        {'3', Value::THREE},
        {'2', Value::DEUCE},
    };

    static inline const char revValueMap[15] = {
        '\0', '\0', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'
    };
    static inline const char revSuitMap[4] = {
        's', 'c', 'd', 'h'
    };

    static const int SHIFT = 2;
    
    uint32_t mId;

    uint32_t InitializeCard(Value value, Suit suit)
    {
        return ( static_cast<uint32_t>(value) << SHIFT ) + static_cast<uint32_t>(suit);
    }

};