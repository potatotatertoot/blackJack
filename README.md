# blackJack
Blackjack Game Simulator

Project Title: Blackjack Game Simulator with Probability Analysis

Team Members: Nathan Wong

Date Published: December 18, 2025

Project Description

This project is a C++ Blackjack game simulator designed to provide an interactive and educational gameplay experience. The program allows a player to play against a dealer following standard Blackjack rules, while also calculating and displaying the player’s probability of winning at each stage of the game. It integrates discrete structures and object-oriented programming to manage cards, decks, hands, and probability computations efficiently.

Programming Approaches Used:

Object-Oriented Programming: Classes and structs are used to represent cards, hands, decks, and the game logic.

Vectors: Store collections of cards for decks, hands, and remaining cards.

Maps: Track outcomes in probability simulations for dealer hands.

Algorithms: std::shuffle for randomizing decks, recursion for dealer outcome simulation, and iteration for hand value calculations.

Probability Calculation: The program simulates potential dealer hands to estimate the player’s chances of winning in real time.

How to Use

Compile the Program: Use a C++ compiler such as g++.

g++ blackjack.cpp -o blackjack


Run the Program:

./blackjack


Gameplay Instructions:

Each hand starts with a default bet of $10.

The player is dealt two cards, and the dealer receives two cards (one hidden).

The program displays the player’s hand, dealer’s visible card, and the winning probability.

During the player’s turn, choose Hit (h) to take another card or Stand (s) to keep your current hand.

The dealer then plays according to standard Blackjack rules (hits below 17, stands at 17 or higher).

The program announces the winner and payout or loss at the end of each hand.

Optionally, the player can choose to play another hand.

Note: This program runs entirely in the console and does not include advanced Blackjack options like splitting or doubling down.
