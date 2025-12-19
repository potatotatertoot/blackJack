#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iomanip>
#include <map>

using namespace std;

struct Card {
    string suit;
    string rank;
    int value;
    
    Card(string s, string r, int v) : suit(s), rank(r), value(v) {}
    
    string toString() const {
        return rank + " of " + suit;
    }
};

class Deck {
private:
    vector<Card> cards;
    int currentCard;
    
public:
    Deck() {
        currentCard = 0;
        string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
        string ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
        
        for (const string& suit : suits) {
            for (int i = 0; i < 13; i++) {
                int value;
                if (i == 0) value = 11;
                else if (i >= 10) value = 10;
                else value = i + 1;
                
                cards.push_back(Card(suit, ranks[i], value));
            }
        }
    }
    
    void shuffle() {
        random_device rd;
        mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
        currentCard = 0;
    }
    
    Card dealCard() {
        return cards[currentCard++];
    }
    
    vector<Card> getRemainingCards() const {
        vector<Card> remaining;
        for (int i = currentCard; i < cards.size(); i++) {
            remaining.push_back(cards[i]);
        }
        return remaining;
    }
    
    int getCardsRemaining() const {
        return cards.size() - currentCard;
    }
};

class Hand {
private:
    vector<Card> cards;
    
public:
    void addCard(const Card& card) {
        cards.push_back(card);
    }
    
    int getValue() const {
        int value = 0;
        int aces = 0;
        
        for (const Card& card : cards) {
            value += card.value;
            if (card.rank == "A") aces++;
        }
        
        while (value > 21 && aces > 0) {
            value -= 10;
            aces--;
        }
        
        return value;
    }
    
    bool isSoft() const {
        int value = 0;
        bool hasAce = false;
        
        for (const Card& card : cards) {
            value += card.value;
            if (card.rank == "A") hasAce = true;
        }
        
        return hasAce && value <= 21 && (value - 10) <= 11;
    }
    
    bool isBlackjack() const {
        return cards.size() == 2 && getValue() == 21;
    }
    
    bool isBust() const {
        return getValue() > 21;
    }
    
    void display(bool hideFirst = false) const {
        for (size_t i = 0; i < cards.size(); i++) {
            if (i == 0 && hideFirst) {
                cout << "[Hidden Card]";
            } else {
                cout << cards[i].toString();
            }
            if (i < cards.size() - 1) cout << ", ";
        }
    }
    
    void clear() {
        cards.clear();
    }
    
    size_t getCardCount() const {
        return cards.size();
    }
    
    Card getCard(int index) const {
        return cards[index];
    }
};

class ProbabilityCalculator {
public:
    static double calculateWinProbability(const Hand& playerHand, const Hand& dealerHand, 
                                         const vector<Card>& remainingCards) {
        int playerValue = playerHand.getValue();
        
        if (playerHand.isBust()) return 0.0;
        
        // Get dealer's visible card
        int dealerUpCard = dealerHand.getCard(1).value;
        
        // Simulate dealer outcomes
        map<int, int> dealerOutcomes;
        int totalSimulations = 0;
        
        // For each possible dealer hand completion
        simulateDealerHands(dealerHand.getValue(), remainingCards, dealerOutcomes, totalSimulations);
        
        // Calculate win probability
        int wins = 0;
        int losses = 0;
        int ties = 0;
        
        for (const auto& outcome : dealerOutcomes) {
            int dealerFinal = outcome.first;
            int count = outcome.second;
            
            if (dealerFinal > 21) {
                wins += count; // Dealer busts
            } else if (playerValue > dealerFinal) {
                wins += count;
            } else if (playerValue < dealerFinal) {
                losses += count;
            } else {
                ties += count;
            }
        }
        
        if (totalSimulations == 0) return 0.0;
        return (double)wins / totalSimulations * 100.0;
    }
    
private:
    static void simulateDealerHands(int dealerValue, const vector<Card>& remaining,
                                   map<int, int>& outcomes, int& total, int depth = 0) {
        // Limit recursion depth for performance
        if (depth > 5) {
            outcomes[dealerValue]++;
            total++;
            return;
        }
        
        // Dealer stands on 17 or more
        if (dealerValue >= 17) {
            outcomes[dealerValue]++;
            total++;
            return;
        }
        
        // Dealer must hit on 16 or less
        if (remaining.empty()) {
            outcomes[dealerValue]++;
            total++;
            return;
        }
        
        // Try each remaining card
        map<int, int> cardValueCounts;
        for (const Card& card : remaining) {
            int val = card.value;
            cardValueCounts[val]++;
        }
        
        for (const auto& cardCount : cardValueCounts) {
            int cardValue = cardCount.first;
            int count = cardCount.second;
            
            int newValue = dealerValue + cardValue;
            
            // Handle ace adjustment
            if (newValue > 21 && cardValue == 11) {
                newValue -= 10;
            }
            
            // Calculate probability weight
            double weight = (double)count / remaining.size();
            
            // Recursive call with reduced deck
            vector<Card> newRemaining = remaining;
            auto it = find_if(newRemaining.begin(), newRemaining.end(),
                            [cardValue](const Card& c) { return c.value == cardValue; });
            if (it != newRemaining.end()) {
                newRemaining.erase(it);
                simulateDealerHands(newValue, newRemaining, outcomes, total, depth + 1);
            }
        }
    }
};

// Game class
class BlackjackGame {
private:
    Deck deck;
    Hand playerHand;
    Hand dealerHand;
    double bet;
    
public:
    BlackjackGame() : bet(10.0) {}
    
    void play() {
        cout << "\n========================================\n";
        cout << "        BLACKJACK GAME START\n";
        cout << "========================================\n";
        
        deck.shuffle();
        playerHand.clear();
        dealerHand.clear();
        
        // Initial deal
        playerHand.addCard(deck.dealCard());
        dealerHand.addCard(deck.dealCard()); // Hole card
        playerHand.addCard(deck.dealCard());
        dealerHand.addCard(deck.dealCard()); // Up card
        
        displayHands(true);
        
        // Calculate initial probability
        cout << "\n--- Initial Hand Analysis ---\n";
        double initialProb = ProbabilityCalculator::calculateWinProbability(
            playerHand, dealerHand, deck.getRemainingCards());
        cout << "Player's winning probability: " << fixed << setprecision(2) 
             << initialProb << "%\n";
        
        // Check for blackjacks
        if (playerHand.isBlackjack() && dealerHand.isBlackjack()) {
            cout << "\nBoth have Blackjack! Push!\n";
            return;
        }
        if (playerHand.isBlackjack()) {
            cout << "\nPlayer has Blackjack! Player wins 1.5x bet!\n";
            cout << "Payout: $" << (bet * 2.5) << "\n";
            return;
        }
        if (dealerHand.isBlackjack()) {
            cout << "\nDealer has Blackjack! Player loses.\n";
            return;
        }
        
        // Player's turn
        playerTurn();
        
        if (playerHand.isBust()) {
            cout << "\nPlayer busts! Player loses $" << bet << "\n";
            return;
        }
        
        // Dealer's turn
        dealerTurn();
        
        // Determine winner
        determineWinner();
    }
    
private:
    void displayHands(bool hideDealerCard = false) {
        cout << "\nDealer's Hand: ";
        dealerHand.display(hideDealerCard);
        if (!hideDealerCard) {
            cout << " (Total: " << dealerHand.getValue() << ")";
        } else {
            cout << " (Showing: " << dealerHand.getCard(1).value << ")";
        }
        
        cout << "\n\nPlayer's Hand: ";
        playerHand.display(false);
        cout << " (Total: " << playerHand.getValue();
        if (playerHand.isSoft()) cout << " - Soft";
        cout << ")\n";
    }
    
    void playerTurn() {
        while (true) {
            cout << "\n--- Player's Turn ---\n";
            cout << "Options: (h)it, (s)tand\n";
            cout << "Choice: ";
            
            char choice;
            cin >> choice;
            
            if (choice == 'h' || choice == 'H') {
                playerHand.addCard(deck.dealCard());
                displayHands(true);
                
                if (!playerHand.isBust()) {
                    double prob = ProbabilityCalculator::calculateWinProbability(
                        playerHand, dealerHand, deck.getRemainingCards());
                    cout << "\nPlayer's winning probability after hit: " 
                         << fixed << setprecision(2) << prob << "%\n";
                }
                
                if (playerHand.isBust()) {
                    break;
                }
            } else if (choice == 's' || choice == 'S') {
                cout << "Player stands.\n";
                break;
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        }
    }
    
    void dealerTurn() {
        cout << "\n--- Dealer's Turn ---\n";
        displayHands(false);
        
        while (dealerHand.getValue() < 17) {
            cout << "\nDealer hits...\n";
            dealerHand.addCard(deck.dealCard());
            cout << "Dealer's Hand: ";
            dealerHand.display(false);
            cout << " (Total: " << dealerHand.getValue() << ")\n";
        }
        
        if (dealerHand.getValue() >= 17) {
            cout << "Dealer stands.\n";
        }
    }
    
    void determineWinner() {
        cout << "\n========================================\n";
        cout << "              FINAL RESULTS\n";
        cout << "========================================\n";
        
        displayHands(false);
        
        int playerTotal = playerHand.getValue();
        int dealerTotal = dealerHand.getValue();
        
        cout << "\n";
        
        if (dealerHand.isBust()) {
            cout << "Dealer busts! Player wins $" << (bet * 2) << "\n";
        } else if (playerTotal > dealerTotal) {
            cout << "Player wins $" << (bet * 2) << "\n";
        } else if (playerTotal < dealerTotal) {
            cout << "Dealer wins. Player loses $" << bet << "\n";
        } else {
            cout << "Push! Player keeps bet of $" << bet << "\n";
        }
    }
};

int main() {
    cout << "Welcome to Blackjack!\n";
    cout << "Bet amount: $10.00 per hand\n";
    
    char playAgain;
    do {
        BlackjackGame game;
        game.play();
        
        cout << "\nPlay another hand? (y/n): ";
        cin >> playAgain;
        
    } while (playAgain == 'y' || playAgain == 'Y');
    
    cout << "\nThanks for playing!\n";
    return 0;
}
