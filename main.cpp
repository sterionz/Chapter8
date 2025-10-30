// Name: Alex Jacobs, David Nguyen, Gabriel Lira, Jessy Zuniga, Cristine Llano, and Jorge Gonzalez
// Date: 11-1-2025
// Description: Chapter 8 Assignments
#include <iostream>
#include <vector>
#include <queue>
#include <random>

#include "input.h"
#include "Patient.h"

void option1();
void option2();

using namespace std;

int main()
{
    do
    {
        system("cls");

        cout << "\n\tCMPR131 Chapter 8: Applications using Queues by Alex Jacobs, David Nguyen, Gabriel Lira, Jessy Zuniga, Cristine Llano, and Jorge Gonzalez";
        cout << "\n\t" << string(100, char(205));
        cout << "\n\t\t1> Simulation of War (card game) using deque STL";
        cout << "\n\t\t2> Simulation of an emergency room (ER) using priority queue STL";
        cout << "\n\t\t3> Simulation of checkout lines at CostCo using multiple queues STL";
        cout << "\n\t" << string(100, char(196));
        cout << "\n\t\t0. Exit";
        cout << "\n\t" << string(100, char(205));
        int option = inputInteger("\n\t\tOption: ", 0, 3);
        switch (option)
        {
        case 1:
        {
            option1();
        }
        break;
        case 2:
        {
            option2();
        }
        break;
        case 3:
        {

        }
        break;
        case 0:
            return 0;
            break;
        }
    } while (true);

}

void option1()
{
    system("cls");
    cout << "\n\t1> Simulation of War (card game) using deque STL";

    int cardsPerSuite = inputInteger("Enther a number of cards per suite: ", 0, 13);
    int totalCards = cardsPerSuite * 4;

    deque<int> deck;

    for (int s = 0; s < 4; ++s)
    {
        for (int r = 1; r <= cardsPerSuite; ++r)
        {
            deck.push_back(r);
        }
    }

    shuffle(deck.begin(), deck.end(), default_random_engine(static_cast<unsigned>(time(0))));

    deque<int> player1, player2;
    for (size_t i = 0; i < deck.size(); ++i)
    {
        if (i % 2 == 0)
        {
            player1.push_back(deck[i]);
        }
        else
        {
            player2.push_back(deck[i]);
        }
    }

    int round = 1;
    while (!player1.empty() && !player2.empty()) 
    {
        int card1 = player1.front();
        int card2 = player2.front();
        player1.pop_front();
        player2.pop_front();

        cout << "\n\tPlayer1: " << card1 << "\tPlayer2: " << card2;

        if (card1 > card2) 
        {
            cout << " -> Player1 wins!";
            player1.push_back(card1);
            player1.push_back(card2);
        }
        else if (card2 > card1) 
        {
            cout << " -> Player2 wins!";
            player2.push_back(card2);
            player2.push_back(card1);
        }
        else 
        {
            cout << " -> Tie breaker begins!";
            deque<int> warPile = { card1, card2 };

            bool warResolved = false;
            while (!warResolved) 
            {
                if (player1.size() < 2) 
                {
                    cout << "\n\tPlayer1 takes all because Player2 has no more card.";
                    player1.insert(player1.end(), warPile.begin(), warPile.end());
                    player1.insert(player1.end(), player2.begin(), player2.end());
                    player2.clear();
                    break;
                }
                else if (player2.size() < 2) 
                {
                    cout << "\n\tPlayer2 takes all because Player1 has no more card.";
                    player2.insert(player2.end(), warPile.begin(), warPile.end());
                    player2.insert(player2.end(), player1.begin(), player1.end());
                    player1.clear();
                    break;
                }

                int war1 = player1.front(); player1.pop_front();
                int war2 = player2.front(); player2.pop_front();
                warPile.push_back(war1);
                warPile.push_back(war2);

                cout << "\n\tPlayer1: " << war1 << "\tPlayer2: " << war2;

                if (war1 > war2) 
                {
                    cout << " -> Player1 wins tie breaker!";
                    player1.insert(player1.end(), warPile.begin(), warPile.end());
                    warResolved = true;
                }
                else if (war2 > war1) 
                {
                    cout << " -> Player2 wins tie breaker!";
                    player2.insert(player2.end(), warPile.begin(), warPile.end());
                    warResolved = true;
                }
                else 
                {
                    cout << " -> Tie breaker again!";
                }
            }
        }
        round++;
    }

    if (player1.size() > player2.size())
        cout << "\n\nPlayer1 wins the war with most number of cards (" << totalCards << ").\n";
    else
        cout << "\n\nPlayer2 wins the war with most number of cards (" << totalCards << ").\n";
}

void option2()
{
    system("cls");
    cout << "\n\t2> Simulation of an emergency room (ER) using priority queue STL\n";

    priority_queue<Patient> pq2;

    vector<Patient> submit;
    do
    {
        cout << "\n\t\t A> Register a patient";
        cout << "\n\t\t B> Transfer patient(s) to the designation";
        cout << "\n\t\t C> Display transferred patients";
        cout << "\n\t\t 0> return\n";
        switch (toupper(inputChar("\t\t Option: ", "ABC0")))
        {
        case '0': return;
        case 'A':
        {
            Patient patient;
            string name;
            cout << "\n\t\tEnter the patient's name: ";
            getline(cin, name);
            patient.setName(name);
            patient.setAge(inputInteger("\t\tEnter the patient's age: ", true));
            patient.setCheckedInTime(time(0));
            patient.setGender(toupper(inputChar("\t\tChoose the patient's gender (F-female or M - male) : ", 'F', 'M')));
            patient.setPriority(inputInteger("\t\tChoose the ER level 1)Non-urgent, 2)Less Urgent, 3)Urgent, 4)Emergent, or 5)Resuscitation: ", 1, 5));
            pq2.push(patient); // enqueue
            cout << "\n\t\tPatient information has been registered.\n";
        }
        break;
        case 'B':
        {
            if (!pq2.empty())
            {
                if (pq2.top().getPriority() == 5)
                {
                    submit.push_back(pq2.top());
                    cout << "\n\t\t" << pq2.top().getName() << " - transfers to ICU...\n";
                    pq2.pop(); // dequeue
                    break;
                }
                else if (pq2.top().getPriority() == 4)
                {
                    submit.push_back(pq2.top());
                    cout << "\n\t\t" << pq2.top().getName() << " - transfers to  surgery room...\n";
                    pq2.pop();
                    break;
                }
                else if (pq2.top().getPriority() == 3)
                {
                    submit.push_back(pq2.top());
                    cout << "\n\t\t" << pq2.top().getName() << " - transfers to emergency room...\n";
                    pq2.pop();
                    break;
                }
                else if (pq2.top().getPriority() == 2)
                {
                    submit.push_back(pq2.top());
                    cout << "\n\t\t" << pq2.top().getName() << " - transfers to x - ray lab...\n";
                    pq2.pop();
                    break;
                }
                else if (pq2.top().getPriority() == 1)
                {
                    submit.push_back(pq2.top());
                    cout << "\n\t\t" << pq2.top().getName() << " - examines and gives prescription...\n";
                    pq2.pop();
                    break;
                }
            }
        }
        break;
        case 'C':
        {
            if (!submit.empty())
            {
                sort(submit.begin(), submit.end());
                reverse(submit.begin(), submit.end());
                cout << '\n';
                for (int i = 0; i < submit.size(); i++)
                    cout << "\t\t" << i + 1 << " - " << submit[i] << '\n';
            }
            else
                cout << "\n\t\tNo patient has been transferred.\n";
        }
        break;
        }
    } while (true);
}
