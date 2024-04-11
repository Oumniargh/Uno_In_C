#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>



// Define card colors
enum Color {
    RED,
    BLUE,
    GREEN,
    YELLOW,
    SPECIAL
};

// Define card types
enum Type {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, SKIP, REVERSE, DRAW_TWO, WILD, WILD_DRAW
};

// Define a card structure
struct Card {
    enum Color color;
    enum Type type;
    struct Card* next; // Pointer to the next card in the linked list
};

// Define a player structure
struct Player {
    char name[20];           // Name of the player
    struct Card* hand;       // Circular linked list representing the player's hand
    struct Player* prev;     // Pointer to the previous player in the doubly linked list
    struct Player* next;     // Pointer to the next player in the doubly linked list
};



struct Card* createCard(enum Color color, enum Type type);
void addCardToDeck(struct Card** deck, struct Card* newCard);
void initializeDeck(struct Card** deck);
void shuffleDeck(struct Card** deck);
const char* getColorName(enum Color color);
const char* getTypeName(enum Type type);
void printCard(struct Card* card);

struct Player* createPlayer(const char* name);
void addPlayerToList(struct Player** firstPlayer, struct Player* newPlayer);
void dealCard(struct Card** deck, struct Player* player);
void dealCards(struct Card** deck, struct Player* firstPlayer, int numPlayers, int numCardsPerPlayer);
void displayHand(struct Player* player);
void removeCardFromHand(struct Player* player, struct Card* cardToRemove);
int checkValidMove(struct Card* topCard, struct Card* playedCard);
void displayLastCard(struct Player* player);
void freePlayer(struct Player* player) ;
void freePlayerList(struct Player* firstPlayer) ;


void SkipTurn(struct Player** currentPlayer);
void Wild(struct Card* card);
void Draw_Two(struct Card** deck, struct Player* nextplayer);
void WildDraw(struct Card** deck, struct Player* nextplayer, struct Card* currentcard);
void reverseDirection(struct Player* firstPlayer);
void printPlayerList(struct Player* firstPlayer);
void playTurn(struct Card** deck, struct Player** currentPlayer, struct Card** discardPile);
void playGame();
void displayInstructions();
void displayCredits();


int main() {
    int choice;

    // Seed for random number generation
    srand(time(NULL));

    printf("Welcome to Uno Game!\n");

    do {
        // Display menu options
        printf("\nMenu:\n");
        printf("1. Play Game\n");
        printf("2. Instructions\n");
        printf("3. Credits\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Perform action based on user choice
        switch (choice) {
            case 1:
                playGame();
                break;
            case 2:
                displayInstructions();
                break;
            case 3:
                displayCredits();
                break;
            case 4:
                printf("Exiting the game. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");

        }
    } while (choice != 4);

    return 0;
}

// Function to create a card
struct Card* createCard(enum Color color, enum Type type) {
    struct Card* newCard = (struct Card*)malloc(sizeof(struct Card));
    newCard->color = color;
    newCard->type = type;
    newCard->next = NULL;
    return newCard;
}

// Function to add a new card to the deck
void addCardToDeck(struct Card** deck, struct Card* newCard) {
    if (*deck == NULL) {
        *deck = newCard;
    } else {
        struct Card* current = *deck;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newCard;
    }
}

// Function to initialize the deck
void initializeDeck(struct Card** deck) {
    // Initialize number cards (0-9)
    for (int i = 0; i < 2; i++) {
        for (enum Color color = RED; color <= YELLOW; color++) {
            for (enum Type type = ONE; type <= NINE; type++) {
                struct Card* newCard = createCard(color, type);
                // Add newCard to the deck (linked list)
                addCardToDeck(deck, newCard);
            }
        }
    }

    // Creating Zeros
    for (enum Color color = RED; color <= YELLOW; color++) {
        struct Card* newCard = createCard(color, ZERO);
        // Add newCard to the deck (linked list)
        addCardToDeck(deck, newCard);
    }

    // Add special cards (Skip, Reverse, Draw Two)
    for (int i = 0; i < 2; i++) {
        for (enum Color color = RED; color <= YELLOW; color++) {
            addCardToDeck(deck, createCard(color, SKIP));
            addCardToDeck(deck, createCard(color, REVERSE));
            addCardToDeck(deck, createCard(color, DRAW_TWO));
        }
    }

    // Add Wild cards (Wild, Wild Draw Four)
    for (int i = 0; i < 4; i++) {
        addCardToDeck(deck, createCard(SPECIAL, WILD));
        addCardToDeck(deck, createCard(SPECIAL, WILD_DRAW));
    }
}

// Function to shuffle the deck
void shuffleDeck(struct Card** deck) {
    struct Card* current = *deck;
    int numCards = 0;
    while (current != NULL) {
        numCards++;
        current = current->next;
    }

    // Fisher-Yates shuffle algorithm
    current = *deck;
    for (int i = 0; i < numCards - 1; i++) {
        int j = i + rand() % (numCards - i);
        struct Card* temp1 = *deck;
        struct Card* temp2 = *deck;
        for (int k = 0; k < i; k++) {
            temp1 = temp1->next;
        }
        for (int k = 0; k < j; k++) {
            temp2 = temp2->next;
        }
        // Swap card data
        enum Color temp_color = temp1->color;
        enum Type temp_type = temp1->type;
        temp1->color = temp2->color;
        temp1->type = temp2->type;
        temp2->color = temp_color;
        temp2->type = temp_type;
    }
}

// Function to get the name of the color
const char* getColorName(enum Color color) {
    switch (color) {
        case RED: return "Red";
        case BLUE: return "Blue";
        case GREEN: return "Green";
        case YELLOW: return "Yellow";
        default: return "Special";
    }
}

// Function to get the name of the type
const char* getTypeName(enum Type type) {
    switch (type) {
        case ZERO: return "0";
        case ONE: return "1";
        case TWO: return "2";
        case THREE: return "3";
        case FOUR: return "4";
        case FIVE: return "5";
        case SIX: return "6";
        case SEVEN: return "7";
        case EIGHT: return "8";
        case NINE: return "9";
        case SKIP: return "Skip";
        case REVERSE: return "Reverse";
        case DRAW_TWO: return "Draw Two";
        case WILD: return "Wild";
        case WILD_DRAW: return "Wilddraw";
        default: return "Unknown";
    }}

// Function to print a card
void printCard(struct Card* card) {
    printf("[%s, %s]", getColorName(card->color), getTypeName(card->type));
}


// Function to deallocate a single card
void freeCard(struct Card* card) {
    free(card);
}

// Function to deallocate the entire deck
void freeDeck(struct Card* deck) {
    struct Card* current = deck;
    while (current != NULL) {
        struct Card* next = current->next;
        freeCard(current);
        current = next;
    }
}

// Function to create players
struct Player* createPlayer(const char* name) {
    struct Player* newPlayer = (struct Player*)malloc(sizeof(struct Player));
    strcpy(newPlayer->name, name);
    newPlayer->hand = NULL;
    newPlayer->next = NULL;
    newPlayer->prev = NULL;
    return newPlayer;
}

// Function to add a player to the list
void addPlayerToList(struct Player** firstPlayer, struct Player* newPlayer) {
    if (*firstPlayer == NULL) {
        *firstPlayer = newPlayer;
    } else {
        struct Player* current = *firstPlayer;
        while (current->next != NULL && current->next != *firstPlayer) {
            current = current->next;
        }
        current->next = newPlayer;
        newPlayer->prev = current;
    }
    // Make the list circular
    newPlayer->next = *firstPlayer;
    (*firstPlayer)->prev = newPlayer;
}

// Function to deal a card
void dealCard(struct Card** deck, struct Player* player) {
    struct Card* current = *deck;
    *deck = (*deck)->next;
    current->next = NULL;
    if (player->hand == NULL) {
        player->hand = current;
    } else {
        struct Card* temp = player->hand;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = current;
    }
}

// Function to deal cards to players
void dealCards(struct Card** deck, struct Player* firstPlayer, int numPlayers, int numCardsPerPlayer) {
    struct Player* currentPlayer = firstPlayer;
    for (int i = 0; i < numPlayers; i++) {
        for (int j = 0; j < numCardsPerPlayer; j++) {
            dealCard(deck, currentPlayer);
        }
        currentPlayer = currentPlayer->next; // Move to the next player
    }
}

// Function to display a player's hand
void displayHand(struct Player* player) {
    printf("\nHand of %s:\n", player->name);
    struct Card* currentCard = player->hand;
    while (currentCard != NULL) {
        // Print card details
        printf("[%s, %s]\n", getColorName(currentCard->color), getTypeName(currentCard->type));
        currentCard = currentCard->next;
    }
}

// Function to remove a card from a player's hand
void removeCardFromHand(struct Player* player, struct Card* cardToRemove) {
    struct Card* current = player->hand;
    struct Card* prev = NULL;

    // Traverse the linked list to find the card to remove
    while (current != NULL) {
        if (current == cardToRemove) {
            // If the card to remove is the first card in the hand
            if (prev == NULL) {
                player->hand = current->next;
            } else {
                prev->next = current->next;
            }
            return;
        }
        prev = current;
        current = current->next;
    }

    // If the card to remove is not found in the hand
    printf("Card not found in hand.\n");
}

// Function to check if a move is valid
int checkValidMove(struct Card* topCard, struct Card* playedCard) {
    // Check if the color or type of the played card matches the top card on the pile
    return (playedCard->color == topCard->color || playedCard->type == topCard->type || playedCard->color == SPECIAL );
}

// function to display last card
void displayLastCard(struct Player* player) {



    // Traverse the hand linked list to reach the last node
    struct Card* currentCard = player->hand;
    while (currentCard->next != NULL) {
        currentCard = currentCard->next;
    }

    // Print the details of the last card
    printf("[%s, %s]\n", getColorName(currentCard->color), getTypeName(currentCard->type));
}

// Function to deallocate a single player
void freePlayer(struct Player* player) {
    free(player);
}

// Function to deallocate the entire list of players
void freePlayerList(struct Player* firstPlayer) {
    struct Player* current = firstPlayer;
    do {
        struct Player* next = current->next;
        freePlayer(current);
        current = next;
    } while (current != firstPlayer);
}


// SkipTurn function
void SkipTurn(struct Player** currentPlayer) {
    printf("Skipping the next player's turn.\n");
    *currentPlayer = (*currentPlayer)->next; // Move to the next player
}

// Wild card Function
void Wild(struct Card* card) {
    printf("Choose the color for the next play:\n");
    printf("1. RED\n2. BLUE\n3. GREEN\n4. YELLOW\n");
    int choice;
    scanf("%d", &choice);

    // Update the color of the Wild card based on the player's choice
    switch(choice) {
        case 1:
            card->color = RED;
            break;
        case 2:
            card->color = BLUE;
            break;
        case 3:
            card->color = GREEN;
            break;
        case 4:
            card->color = YELLOW;
            break;
        default:
            printf("Invalid choice. ReEnter Your Choice .\n");
            Wild(card);
    }
}

// Draw Two Function
void Draw_Two(struct Card** deck,struct Player* nextplayer) {
    printf("\nPlayer %s Drew The Following Cards:\n", *nextplayer);
    for (int i = 0; i < 2; i++) {
dealCard(deck,nextplayer) ;
displayLastCard(nextplayer) ;
    }
    return ;
}

// WILD DRAW FOUR function
void WildDraw (struct Card** deck,struct Player* nextplayer,struct Card* currentcard) {
Wild(currentcard) ;
printf("\nPlayer %s Drew The Following Cards:\n", nextplayer->name);
   for (int i = 0; i < 4; i++) {
dealCard(deck,nextplayer) ;
displayLastCard(nextplayer) ;
printf("\n");

    }  }

// function to handle reverse <3 Working
void reverseDirection(struct Player* firstPlayer) {
    struct Player* current = firstPlayer;
    struct Player* temp = NULL ;

    // swap next and prev pointers for each player
    do {
        temp = current->next;
        current->next = current->prev;
        current->prev = temp;
        current = temp; // Move to the next node
    } while (current != firstPlayer);

    // Update the pointer to the first player
    firstPlayer = current->prev;
}

//most important function
void playTurn(struct Card** deck, struct Player** currentPlayer, struct Card** discardPile) {
    // Print the top card on the pile
    printf("Top card on the pile: ");
    printCard(*discardPile);
    displayHand(*currentPlayer);

    while (1) {
        // Choose a card to play or type "Draw" to draw a card
        printf("Choose a card to play (enter color and type) or type 'Draw' to draw a card or 'Exit': ");
        char input[20];
        scanf("%s", input);
        // Convert input to lowercase
        for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }
            // Check if the player wants to exit the game
        if (strcmp(input, "exit") == 0) {
            printf("Exiting the game...\n");
             if (deck != NULL) {
                    freeDeck(*deck);  // Free the deck
                    deck = NULL;     // Set deck pointer to NULL after freeing
                }
                if (currentPlayer != NULL) {
                    freePlayerList(*currentPlayer);  // Free the player list
                    currentPlayer = NULL;            // Set player list pointer to NULL after freeing
                }
            exit(0); // Exit the program
        }

        // Convert input to lowercase
        for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }
        // Check if the player chose to draw a card
        if (strcmp(input, "draw") == 0) {
                 if (*deck == NULL) {
        printf("Deck is empty! Shuffling discard pile back into the deck.\\n");
        *deck = *discardPile;
        *discardPile = NULL;
        shuffleDeck(deck); // Reshuffle the deck
    }
            dealCard(deck, *currentPlayer);
            printf("Player %s drew a card.\n", (*currentPlayer)->name);
            displayLastCard(*currentPlayer);
            //get last card
            struct Card* lastcard = (*currentPlayer)->hand;
            while (lastcard->next != NULL) {
                lastcard = lastcard->next;
            }
            while (1) {
            if (checkValidMove(*discardPile, lastcard)) {
                char choice;
                printf("Do you want to play the card? [Y/N]: ");
                scanf(" %c", &choice);
                  if (choice == 'Y' || choice == 'y') {
                    // Play the card
                     printf("Player %s played ", (*currentPlayer)->name);
                    printCard(lastcard);
                // Remove played card from player's hand
                removeCardFromHand(*currentPlayer, lastcard);
                // Put currentcard on top of the deck
                if ((*discardPile)->type == WILD || (*discardPile)->type == WILD_DRAW) {
                    (*discardPile)->color = SPECIAL;
                }
               struct Card* currentCard = lastcard;

                currentCard->next = *discardPile;
                *discardPile = currentCard;

                    currentCard->next = *discardPile;

// handle special cards
                         switch ((*discardPile)->type) {

                        case SKIP:
SkipTurn(currentPlayer);

                            // Skip the next player's turn
                            break;
                        case REVERSE:
reverseDirection(*currentPlayer);
   break;
                        case WILD_DRAW:
                            WildDraw(deck, (*currentPlayer)->next, *discardPile);
                                          SkipTurn(currentPlayer);

                            break;
                        case DRAW_TWO:
                            Draw_Two(deck, (*currentPlayer)->next);
                            SkipTurn(currentPlayer);

                            break;
                        case WILD:
                            Wild(*discardPile);
                            break;
                        default:
                            break;;}

           } else if (choice == 'N' || choice == 'n'){
                    printf("Card was not played.\n");
                    // Continue the turn
                    return;
                } else {
    printf("Invalid choice. Please enter 'Y' or 'N'.\n");
 continue;
                }
            } else {
                printf("Card is not playable.\n");
                // Continue the turn
                break;
            }
            return ;
        } return;}
        for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }
        // Convert the input to color and type
        enum Color color;
        enum Type type;
        if (strcmp(input, "red") == 0) {
            color = RED;
        } else if (strcmp(input, "blue") == 0) {
            color = BLUE;
        } else if (strcmp(input, "green") == 0) {
            color = GREEN;
        } else if (strcmp(input, "yellow") == 0) {
            color = YELLOW;
        } else if (strcmp(input, "special") == 0) {
            color = SPECIAL;
        } else {
            printf("Invalid input. Try again.\n");
            continue; // Prompt the player to enter their choice again
        }

        scanf("%s", input); // Read the type
        for (int i = 0; input[i]; i++) {
            input[i] = tolower(input[i]);
        }
        if (strcmp(input, "0") == 0) {
            type = ZERO;
        } else if (strcmp(input, "1") == 0) {
            type = ONE;
        } else if (strcmp(input, "2") == 0) {
            type = TWO;
        } else if (strcmp(input, "3") == 0) {
            type = THREE;
        } else if (strcmp(input, "4") == 0) {
            type = FOUR;
        } else if (strcmp(input, "5") == 0) {
            type = FIVE;
        } else if (strcmp(input, "6") == 0) {
            type = SIX;
        } else if (strcmp(input, "7") == 0) {
            type = SEVEN;
        } else if (strcmp(input, "8") == 0) {
            type = EIGHT;
        } else if (strcmp(input, "9") == 0) {
            type = NINE;
        } else if (strcmp(input, "skip") == 0) {
            type = SKIP;
        } else if (strcmp(input, "reverse") == 0) {
            type = REVERSE;
        } else if (strcmp(input, "draw") == 0) {
            type = DRAW_TWO;
        } else if (strcmp(input, "wild") == 0) {
            type = WILD;
        } else if (strcmp(input, "wilddraw") == 0) {
            type = WILD_DRAW;
        } else {
            printf("Invalid input. Try again.\n");
            continue; // Prompt the player to enter their choice again
        }

        // Check if the chosen card is playable
        struct Card* currentCard = (*currentPlayer)->hand;
        int cardFound = 0; // Flag to track if the card is found in the player's hand
        while (currentCard != NULL) {
            if (currentCard->color == color && currentCard->type == type) {
                cardFound = 1;
                // Found the playable card
                if (checkValidMove(*discardPile, currentCard)) {
                    printf("Player %s played ", (*currentPlayer)->name);
                    printCard(currentCard);
                    printf("\n");
                    // Remove played card from player's hand
                    removeCardFromHand(*currentPlayer, currentCard);
                    // Put currentcard on top of the deck
                    if ((*discardPile)->type == WILD || (*discardPile)->type == WILD_DRAW) {
                        (*discardPile)->color = SPECIAL;
                    }
                    (currentCard)->next = *discardPile;
                    *discardPile = currentCard;
                      switch ((*discardPile)->type) {

                        case SKIP:
SkipTurn(currentPlayer);

                            // Skip the next player's turn
                            break;
                        case REVERSE:
reverseDirection(*currentPlayer);
   break;
                        case WILD_DRAW:
                            WildDraw(deck, (*currentPlayer)->next, *discardPile);
                                          SkipTurn(currentPlayer);

                            break;
                        case DRAW_TWO:
                            Draw_Two(deck, (*currentPlayer)->next);
                            SkipTurn(currentPlayer);

                            break;
                        case WILD:
                            Wild(*discardPile);
                            break;
                        default:
                            break;;}
                    return; // Player's turn completed
                } else {
                    printf("Invalid move. Try again.\n");
                    break; // Prompt the player to enter the card details again
                }
            }
            currentCard = currentCard->next;
        }
        if (!cardFound) {
            printf("You do not have that card in your hand!\n");
            continue;
        }
    }
}

// Function to start the game
void playGame() {
    printf("Starting the game...\n");

 srand(time(NULL)); // Seed for random number generation



    struct Card* deck = NULL;
    initializeDeck(&deck);
    shuffleDeck(&deck);
   // printDeck(deck);
       char a[100]; // Buffer to clear input

    int numPlayers;

        do {
        printf("Enter number of players (between 2 and 10): ");
        if (scanf("%d", &numPlayers) != 1) {
            printf("Invalid input. Please enter a valid number.\n");
            scanf("%s", a); // Clear input buffer
            continue; // Continue to next iteration
        }

        if (numPlayers < 2 || numPlayers > 10) {
            printf("Invalid number of players. Please enter a number between 2 and 10.\n");
        }
    } while (numPlayers < 2 || numPlayers > 10);




    struct Player* firstPlayer = NULL;
    for (int i = 0; i < numPlayers; i++) {
        char playerName[20];
        printf("Enter player %d's name: ", i + 1);
        scanf("%s", playerName);
        struct Player* newPlayer = createPlayer(playerName);
        addPlayerToList(&firstPlayer, newPlayer);
    }

    dealCards(&deck, firstPlayer, numPlayers, 7);
    // Setting up the initial discard pile
struct Card* topCard = deck;
deck = deck->next; // Move to the next card in the deck

// Check if the top card is a Wild or Special card
while (topCard->color == SPECIAL || topCard->type == DRAW_TWO || topCard->type ==  REVERSE || topCard->type ==  SKIP  ) {
    // If it's a Wild or Special card, put it back in the deck and move to the next card
    topCard->next = deck;
    deck = topCard;
    topCard = topCard->next;
}

// Initialize the discard pile with the non-Wild, non-Special top card from the deck
struct Card* discardPile = topCard;
discardPile->next = NULL; // Ensure the discard pile starts as a single card

    printf("\nGame Started\n");

    struct Player* currentPlayer = firstPlayer;
    while (1) {
playTurn(&deck, &currentPlayer, &discardPile);

// Check if the current player's hand is empty
        if (currentPlayer->hand == NULL) {
            printf("Player %s wins the game!\n", currentPlayer->name);
                    // Free dynamically allocated memory
                if (deck != NULL) {
                    freeDeck(deck);  // Free the deck
                    deck = NULL;     // Set deck pointer to NULL after freeing
                }
                if (firstPlayer != NULL) {
                    freePlayerList(firstPlayer);  // Free the player list
                    firstPlayer = NULL;            // Set player list pointer to NULL after freeing
                }
            break; // End the game
        }
    currentPlayer = currentPlayer->next; // Move to the next player
   // clearScreen() ;

    }



    return ;
}

// Function to display game instructions
void displayInstructions() {
    printf("\nInstructions:\n");
    printf("1. The goal of the game is to be the first player to get rid of all your cards.\n");
    printf("2. Players take turns playing a card from their hand that matches the top card or draw a card.\n");
    printf("3. If a player cannot play a card or Do Not Want to, they must draw a card from the deck.\n");
    printf("4. Normal cards are played by typing the cards color followed by its number in digit (eg : “red 4” ) .\n");
    printf("5. Other special but colored cards “Color function” “Draw” for draw two “Skip/Reverse” for skip/reverse .\n");
    printf("6. For special cards : “special wild” / “special wilddraw” .\n ");


}

// Function to display game credits
void displayCredits() {
    printf("\nCredits:\n");
    printf("Game developed by [Oumnia Reghioui] And [Cerine Hanane Cheddad]\n");
    printf("Esi ALg.\n");
}



