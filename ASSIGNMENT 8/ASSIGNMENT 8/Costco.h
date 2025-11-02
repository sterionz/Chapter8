#pragma once
// THIS IS ALL BASED ON CHAPTER 8 of the book. The code was modified from the washing .cpp and .h and main.
#ifndef COSTCO
#define COSTCO

#ifndef NAMESPACE_AS_8
#define NAMESPACE_AS_8
#include <cassert> // Provides assert for precondition checks
#include <cstdlib> // Provides rand and RAND_MAX for random number generation
// CLASSES implemented: bool_source
//
// INVARIANT for the bool_source ADT:
// 1. The member variable probability is the approximate probability that
//    query( ) returns true. It remains constant after construction and ensures
//    consistent behavior across multiple queries.
//
// CONSTRUCTOR for the bool_source class:
//   bool_source(double p = 0.5)
//   Precondition: 0 <= p <= 1 (probability must be a valid fraction between 0 and 1 inclusive).
//   Postcondition: The bool_source has been initialized so that p is the
//   approximate probability of returning true in any subsequent activation
//   of query( ). The private member probability is set to p.
//
// CONSTANT MEMBER FUNCTION for the bool_source class:
//   bool query( ) const
//   Precondition: The object has been properly initialized with a valid probability.
//   Postcondition: The return value is either true or false, with the
//   probability of a true value being approximately p (from the constructor).
//   This function does not modify the object's state.
//
namespace as_8
{
    class bool_source
    {
    public:
        // CONSTRUCTOR
        bool_source(double p = 0.5);
        // CONSTANT function
        bool query() const;
    private:
        double probability; // Stores the probability (0.0 to 1.0) of query() returning true
    };
    bool_source::bool_source(double p)
    {
        assert(p >= 0); // Enforce precondition: probability >= 0
        assert(p <= 1); // Enforce precondition: probability <= 1
        probability = p;
    }
    bool bool_source::query() const
    {
        // Uses rand() to generate a random value and compares it against the scaled probability
        // RAND_MAX is the maximum value from rand(), so probability * RAND_MAX sets the threshold
        return (rand() < probability * RAND_MAX);
    }
}
#endif // !NAMESPACE_AS_8

#include <cassert> // Provides assert (not used here, included for consistency)
#include <cstdlib> // Provides rand and RAND_MAX (redundant)
#include <iostream> // For console output (cout)
#include <iomanip> // For output formatting (setw)
#include <vector> // For vector of deques (registers)
#include <deque> // For deque<Customer> to represent checkout lines (allows easy front access and iteration)
#include <ctime> // For time(0) in seeding rand (SEED IN MAIN)
#include <algorithm> // For std::min in item processing
#include <thread> // For std::this_thread::sleep_for to control simulation pacing
#include <chrono> // For std::chrono::milliseconds in sleep_for
#include <string> // For std::string (cart_symbol)
#include "input.h" // Provides inputInteger and inputDouble for user input validation

// Function to generate a random integer in [min, max] inclusive
// Precondition: min <= max, and rand() has been seeded elsewhere (e.g., in main).
// Postcondition: Returns a uniformly distributed random integer between min and max (inclusive).
int randomInRange(const int& min, const int& max) {
    
    return min + rand() % (max - min + 1);
}

// Settings function to modify simulation parameters
// Precondition: References to arrival_prob, display_interval, and sim_speed_ms are valid and can be modified.
//               inputDouble and inputInteger are available from "input.h".
// Postcondition: User is prompted for new values; 
// arrival_prob is updated to a value in [0.0, 1.0],
// display_interval to an integer in [1, 60], 
// sim_speed_ms to a value based on speed_option (0, 10, 25, 50, or 100 ms).
// No return value; modifications are made via references.
void settings(double& arrival_prob, unsigned int& display_interval, unsigned int& sim_speed_ms) {
    cout << "\nSimulation Settings:\n";
    
    arrival_prob = inputDouble("\tEnter customer arrival probability (0.0–1.0): ", 0.0, 1.0);
    
    display_interval = static_cast<unsigned int>(inputInteger("\tEnter display update interval in seconds (1–60): ", 1, 60));

    int speed_option = inputInteger("\tEnter simulation speed (1: instant, 2: faster, 3: fast, 4: slow, 5: slowest): ", 1, 5);
    switch (speed_option) {
    case 1: sim_speed_ms = 0; break; // Instant (no delay)
    case 2: sim_speed_ms = 10; break; // Faster
    case 3: sim_speed_ms = 25; break; // Fast
    case 4: sim_speed_ms = 50; break; // Slow
    case 5: sim_speed_ms = 100; break; // Slowest
    }
}

// Represents a customer with a cart of N items
// This is a simple struct with no invariants or complex behavior.
struct Customer {
    unsigned int items; // Number of items in the customer's cart (1-100 typically)
    // Constructor
    // Precondition: i >= 0 (though not enforced; assumes valid input).
    // Postcondition: Initializes items to the provided value (default 0).
    Customer(unsigned int i = 0) : items(i) {}
};

// Simulates multiple checkout lines
// Precondition: total_time >= 0, num_registers >= 1, 0 <= arrival_prob <= 1,
//               display_interval >= 1, sim_speed_ms >= 0.
//               rand() should be seeded before calling.
// Postcondition: Runs the simulation for total_time seconds, processing customers and displaying updates.
//                Outputs final report with total served and average customers/sec.
//                System pauses at the end for user to view results.
void costco_simulate(unsigned int total_time, unsigned int num_registers, double arrival_prob,
    unsigned int display_interval = 10, unsigned int sim_speed_ms = 50) {
    as_8::bool_source arrival(arrival_prob); // Bool source for random customer arrivals based on probability

    // Use deques for easy iteration during display
    std::vector<std::deque<Customer>> registers(num_registers); // Vector of deques, each representing a checkout line (register)

    unsigned int current_second = 0; // Tracks the current simulated second (starts at 0, increments to total_time)
    unsigned int total_served = 0; // Cumulative count of customers fully served across all registers
    unsigned int items_per_second = randomInRange(1, 5); // Randomly selected processing rate per second per register (1-5 items/sec)
    std::string cart_symbol = "#"; // Symbol used to represent carts in display (e.g., #12# for front customer)

    // Main simulation loop: Runs for each simulated second
    // Steps 0-4 are repeated until the time is up.
    while (current_second < total_time) {
        ++current_second; // Increment the time counter (Step 0: Advance time)

        // 1. Randomly create a new customer
        //   - Uses arrival.query() to decide if a customer arrives this second (based on probability)
        //   - If yes, generates cart_size (1-100 items randomly)
        //   - Finds the shortest line by iterating over registers and comparing sizes
        //   - Adds the new Customer to the back of the shortest deque
        if (arrival.query()) {
            unsigned int cart_size = randomInRange(1, 100); // 1–100 items
            // Find the shortest line
            size_t shortest = 0;
            for (size_t i = 1; i < registers.size(); ++i) {
                if (registers[i].size() < registers[shortest].size()) {
                    shortest = i;
                }
            }
            registers[shortest].push_back(Customer(cart_size));
        }

        // 2. Each register processes up to items_per_second
        //   - For each register: If not empty, access front customer
        //   - Subtract min(remaining items, items_per_second) to simulate processing
        //   - If items reach 0 after subtraction, pop the front (customer served), increment total_served
        for (size_t i = 0; i < registers.size(); ++i) {
            if (!registers[i].empty()) {
                Customer& front = registers[i].front();
                front.items -= std::min(front.items, items_per_second);
                if (front.items == 0) {
                    registers[i].pop_front();
                    ++total_served;
                }
            }
        }

        // 3. Display snapshot periodically
        //   - Checks if current_second is a multiple of display_interval or the last second
        //   - If yes, clears screen 
        //   - Outputs time remaining and total served
        //   - For each register: Outputs header, then iterates over deque
        //     - For front customer (first=true): Shows detailed #NN# (NN = items)
        //     - For others: Shows generic ####
        if (current_second % display_interval == 0 || current_second == total_time) {
            system("cls"); // Clear console screen
            cout << "CostCo time remaining open: "
                << (total_time - current_second)
                << " sec\tNumber of served customers: "
                << total_served << "\n\n";
            for (size_t i = 0; i < registers.size(); ++i) {
                cout << " Cash register #" << (i + 1) << ":\n\t\t";
                bool first = true; // Flag to identify the front customer for detailed display
                for (const auto& customer : registers[i]) {
                    if (first) {
                        cout << cart_symbol << setw(2)
                            << customer.items << cart_symbol << " ";
                        first = false;
                    }
                    else {
                        cout << cart_symbol << cart_symbol
                            << cart_symbol << cart_symbol << " ";
                    }
                }
                cout << "\n\n";
            }
        }

        // 4. Control simulation pacing (set 0 for instant run)
        //   - Pauses the thread for sim_speed_ms milliseconds to simulate real-time pacing
        //   - If 0, runs instantly without delay
        std::this_thread::sleep_for(std::chrono::milliseconds(sim_speed_ms));
    }

    // 5. Final report
    //   - Outputs closure message, total served, and average (total_served / total_time, handling division by zero)
    cout << "\nStore closed!\n";
    cout << "Total customers served: " << total_served << "\n";
    cout << "Average service (customers/sec): "
        << (total_time > 0 ? total_served / static_cast<double>(total_time) : 0)
        << "\n";
    system("pause"); // Pause console before exit
}
#endif // !COSTCO