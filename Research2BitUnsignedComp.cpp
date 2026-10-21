
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;

//balanced tree structure
//O(logn) circuit depth
// total number of comparisons/combination modules: O(n)
// PART 1: Specify Logic Behavior
// PART 2: Modularization using 1-bit comparators
// Output:
// gt = 1 if A > B
// gt = 0 otherwise
// Convert an unsigned integer to an n-bit binary string

//use n=1 bit to output 
//what GENERAL function should help us distsinguish between the two values, n=1, n=2 bits
//modular, linearly architecture, tree
//what works/doesn't work for P, in the P and Q circuit structure
//how should they be connected together?

string toBinary(uint32_t value, unsigned n) {
    string result(n, '0');

    for (unsigned i = 0; i < n; i++) {
        result[n - 1 - i] =
            ((value >> i) & 1u) ? '1' : '0';
    }

    return result;
}

// Convert an n-bit binary string to an unsigned integer
uint32_t fromBinary(const string& bits) {
    uint32_t value = 0;

    for (char bit : bits) {
        value = (value << 1) | (bit - '0');
    }

    return value;
}

// Check that the input contains exactly n binary bits
bool validBinary(const string& bits, unsigned n) {
    return bits.size() == n &&
        bits.find_first_not_of("01") == string::npos;
}

// PART 1: Specify Logic Behavior
// Behavioral specification of an unsigned comparator
bool behavioralComparator(uint32_t A, uint32_t B) {
    return A > B;
}

// Generate truth table
void part1(unsigned n) {
    uint32_t total = 1u << n;

    cout << "Logic Definition\n";
    cout << "gt = 1 if A > B\n";
    cout << "gt = 0 if A <= B\n";

    cout << "\nTruth Table for n = " << n << "\n\n";

    // Print separate column headings for A bits
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
        cout << setw(5) << ("A" + to_string(i));
    }

    // Print separate column headings for B bits
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
        cout << setw(5) << ("B" + to_string(i));
    }

    // Print separate column headings for one-bit gt signals
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
        cout << setw(5) << ("gt" + to_string(i));
    }

    // Final comparator output
    cout << setw(5) << "gt" << "\n";

    // Print separator line
    for (unsigned i = 0; i < 3 * n + 1; i++) {
        cout << "-----";
    }
    cout << "\n";

    // Generate all input combinations
    for (uint32_t A = 0; A < total; A++) {
        for (uint32_t B = 0; B < total; B++) {

            // Print each bit of A separately
            for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
                bool a = ((A >> i) & 1u) != 0;
                cout << setw(5) << a;
            }

            // Print each bit of B separately
            for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
                bool b = ((B >> i) & 1u) != 0;
                cout << setw(5) << b;
            }

            // Print each individual one-bit comparator result
            for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
                bool a = ((A >> i) & 1u) != 0;
                bool b = ((B >> i) & 1u) != 0;

                bool gtBit = a && !b;

                cout << setw(5) << gtBit;
            }

            // Print the final n-bit comparison result
            bool gt = behavioralComparator(A, B);

            cout << setw(5) << gt << "\n";
        }
    }
}

// PART 2: MODULARIZATION
// Each comparator module produces two signals:
// greater: whether A > B
// equal: whether A == B

struct CompareResult {
    bool greater;
    bool equal;
};

// One-bit comparator module
CompareResult compareOneBit(bool a, bool b) {

    CompareResult result;

    // G = a AND (NOT b)
    result.greater = a && !b;

    // E = a XNOR b
    result.equal = (a == b);

    return result;
}

// Combine a higher-order comparator with a lower-order
// comparator.
//
// G = G_high OR (E_high AND G_low)
// E = E_high AND E_low

CompareResult combineModules(
    CompareResult high,
    CompareResult low
) {
    CompareResult result;

    result.greater =
        high.greater ||
        (high.equal && low.greater);

    result.equal =
        high.equal && low.equal;

    return result;
}

// Store a comparison module and its binary groups
struct CircuitModule {
    CompareResult result;
    string bitsA;
    string bitsB;
    string gNode;
    string eNode;
};

// General n-bit modular comparator
CompareResult modularComparator(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    vector<CompareResult> modules;

    // Build one-bit comparison modules independently
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {

        bool a = ((A >> i) & 1u) != 0;
        bool b = ((B >> i) & 1u) != 0;

        CompareResult current = compareOneBit(a, b);
        modules.push_back(current);
    }

    // Combine adjacent modules in a balanced tree
    while (modules.size() > 1) {

        vector<CompareResult> nextLevel;

        for (size_t i = 0; i < modules.size(); i += 2) {

            if (i + 1 < modules.size()) {

                // Left module contains higher-order bits
                CompareResult combined =
                    combineModules(modules[i], modules[i + 1]);

                nextLevel.push_back(combined);
            }
            else {
                nextLevel.push_back(modules[i]);
            }
        }

        modules = nextLevel;
    }

    return modules[0];
}

// Display Part 2 equations without another truth table
void part2(unsigned n) {

    cout << "\nPART 2: MODULARIZATION\n";

    cout << "\nOne-Bit Comparator:\n";
    cout << "G = a AND (NOT b)\n";
    cout << "E = a XNOR b\n";

    cout << "\nUsing a balanced-tree comparator for n = "
        << n << "\n";
}

// Display the comparison outcome
string comparisonText(const CompareResult& result) {

    if (result.greater)
        return "A > B";

    if (result.equal)
        return "A == B";

    return "A < B";
}

// Display the balanced-tree circuit comparison path
void printCircuitPath(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    vector<CircuitModule> modules;

    cout << "\nLevel 0: One-bit comparator modules\n";

    // All individual bit comparisons
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {

        bool a = ((A >> i) & 1u) != 0;
        bool b = ((B >> i) & 1u) != 0;

        CompareResult current = compareOneBit(a, b);

        CircuitModule module;

        module.result = current;
        module.bitsA = string(1, a ? '1' : '0');
        module.bitsB = string(1, b ? '1' : '0');

        modules.push_back(module);

        cout << "  Bit " << i << ": "
            << a << " vs " << b
            << " => G=" << current.greater
            << ", E=" << current.equal << "\n";
    }

    unsigned level = 1;

    // Combine modules pairwise until one remains
    while (modules.size() > 1) {

        vector<CircuitModule> nextLevel;

        cout << "\nLevel " << level
            << ": Combine adjacent results\n";

        for (size_t i = 0; i < modules.size(); i += 2) {

            if (i + 1 < modules.size()) {

                CircuitModule high = modules[i];
                CircuitModule low = modules[i + 1];

                CircuitModule combined;

                combined.result =
                    combineModules(high.result, low.result);

                combined.bitsA = high.bitsA + low.bitsA;
                combined.bitsB = high.bitsB + low.bitsB;

                nextLevel.push_back(combined);

                cout << "  Compare "
                    << combined.bitsA
                    << " vs "
                    << combined.bitsB
                    << " => "
                    << comparisonText(combined.result)
                    << "; G="
                    << combined.result.greater
                    << ", E="
                    << combined.result.equal
                    << "\n";
            }
            else {

                // Carry unmatched module forward
                nextLevel.push_back(modules[i]);

                cout << "  Carry "
                    << modules[i].bitsA
                    << " vs "
                    << modules[i].bitsB
                    << " => "
                    << comparisonText(modules[i].result)
                    << "; G="
                    << modules[i].result.greater
                    << ", E="
                    << modules[i].result.equal
                    << "\n";
            }
        }

        modules = nextLevel;
        level++;
    }

    cout << "\nFINAL CIRCUIT OUTPUT\n";
    cout << "gt = " << modules[0].result.greater << "\n";
    cout << "eq = " << modules[0].result.equal << "\n";
}

//main program
int main() {

    unsigned n;
    string binaryA, binaryB;

    cout << "Enter bit width n (1-8): ";
    cin >> n;

    if (!cin || n < 1 || n > 8) {
        cout << "Invalid bit width.\n";
        return 1;
    }

    cout << "Enter A (" << n << " bits): ";
    cin >> binaryA;

    if (!validBinary(binaryA, n)) {
        cout << "Invalid binary input.\n";
        return 1;
    }

    cout << "Enter B (" << n << " bits): ";
    cin >> binaryB;

    if (!validBinary(binaryB, n)) {
        cout << "Invalid binary input.\n";
        return 1;
    }

    uint32_t A = fromBinary(binaryA);
    uint32_t B = fromBinary(binaryB);

    cout << "\nPART 1: SPECIFY LOGIC BEHAVIOR\n";
    part1(n);

    // Part 2 prints its equations without another truth table
    part2(n);

    cout << "\nCOMPARISON OF ENTERED NUMBERS\n";

    cout << "A = " << binaryA << "\n";
    cout << "B = " << binaryB << "\n";

    cout << "\nPart 1 Result:\n";
    cout << "gt = " << behavioralComparator(A, B) << "\n";

    CompareResult result = modularComparator(A, B, n);

    cout << "\nPart 2 Result:\n";
    cout << "gt = " << result.greater << "\n";
    cout << "eq = " << result.equal << "\n";

    // Print the comparisons in the exact requested format
    printCircuitPath(A, B, n);

    return 0;
}
