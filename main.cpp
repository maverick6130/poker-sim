#include "deck.h"
#include "ranker.h"
#include <iostream>
#include <chrono>

int main()
{
    Deck deck;
    int N = 1000000;

    std::chrono::nanoseconds totalDuration;

    while(N--)
    {
        deck.Shuffle();
        std::vector<Card> hand = deck.Draw(7);
        for (Card card : hand)
        {
            std::cout << card.GetStr() << " ";
        }
        std::cout << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        Rank rank(hand);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        totalDuration += duration;

        std::cout << rank.Verbose();
    }

    std::cout << totalDuration.count()/1.0e9 << "seconds";
    std::cout << std::endl;
}