#include "deck.h"
#include <iostream>

int main()
{
    Deck deck;
    std::vector<Card> cards = deck.Draw(5);
    for (Card card : cards)
        std::cout << card.GetStr() << " ";
    std::cout << std::endl;
}