
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// PART 1: Specify Logic Behavior
// PART 2: Modularization using P and Q modules
// Compare linear and balanced-tree architectures
// Output:
// gt = 1 if A > B
// gt = 0 otherwise

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

    // Print A bit headings
    for (int i = static_cast<int>(n) - 1; i >= 0; i--)
        cout << setw(5) << ("A" + to_string(i));

    // Print B bit headings
    for (int i = static_cast<int>(n) - 1; i >= 0; i--)
        cout << setw(5) << ("B" + to_string(i));

    // Print local one-bit greater-than signals
    for (int i = static_cast<int>(n) - 1; i >= 0; i--)
        cout << setw(5) << ("G" + to_string(i));

    cout << setw(5) << "gt" << "\n";
    cout << string((3 * n + 1) * 5, '-') << "\n";

    for (uint32_t A = 0; A < total; A++) {
        for (uint32_t B = 0; B < total; B++) {

            for (int i = static_cast<int>(n) - 1; i >= 0; i--)
                cout << setw(5) << ((A >> i) & 1u);

            for (int i = static_cast<int>(n) - 1; i >= 0; i--)
                cout << setw(5) << ((B >> i) & 1u);

            for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
                bool a = ((A >> i) & 1u) != 0;
                bool b = ((B >> i) & 1u) != 0;

                cout << setw(5) << (a && !b);
            }

            cout << setw(5)
                << behavioralComparator(A, B) << "\n";
        }
    }
}

// PART 2: MODULARIZATION
// P module produces greater and equal signals
struct CompareResult {
    bool greater;
    bool equal;
};

// P: One-bit comparator module
CompareResult compareOneBit(bool a, bool b) {
    return {
        a && !b,
        a == b
    };
}

// Q for linear architecture
// Accepts a, b and the previous comparison result
// The previous result acts as carry-in
CompareResult linearQ(
    bool a,
    bool b,
    CompareResult carryIn
) {
    CompareResult current = compareOneBit(a, b);

    return {
        current.greater ||
        (current.equal && carryIn.greater),

        current.equal && carryIn.equal
    };
}

// Q for tree architecture
// Combines two comparison modules
// G = G_high OR (E_high AND G_low)
// E = E_high AND E_low
CompareResult combineModules(
    CompareResult high,
    CompareResult low
) {
    return {
        high.greater ||
        (high.equal && low.greater),

        high.equal && low.equal
    };
}

// Linear comparator
// 1 P module followed by n-1 Q modules
CompareResult linearComparator(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    // P compares the least significant bits
    CompareResult result = compareOneBit(
        (A & 1u) != 0,
        (B & 1u) != 0
    );

    // Each Q accepts the previous result as carry-in
    for (unsigned i = 1; i < n; i++) {

        bool a = ((A >> i) & 1u) != 0;
        bool b = ((B >> i) & 1u) != 0;

        result = linearQ(a, b, result);
    }

    return result;
}

// Store tree comparison modules
struct CircuitModule {
    CompareResult result;
    string bitsA;
    string bitsB;
    string id;
};

// Tree comparator
// n P modules followed by n-1 Q modules
CompareResult treeComparator(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    vector<CompareResult> modules;

    // All P modules operate independently
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {

        bool a = ((A >> i) & 1u) != 0;
        bool b = ((B >> i) & 1u) != 0;

        modules.push_back(compareOneBit(a, b));
    }

    // Q modules combine adjacent results
    while (modules.size() > 1) {

        vector<CompareResult> nextLevel;

        for (size_t i = 0; i < modules.size(); i += 2) {

            if (i + 1 < modules.size()) {
                nextLevel.push_back(
                    combineModules(modules[i], modules[i + 1])
                );
            }
            else {
                nextLevel.push_back(modules[i]);
            }
        }

        modules = nextLevel;
    }

    return modules[0];
}

// Display comparison result
string comparisonText(CompareResult result) {
    if (result.greater)
        return "A > B";

    if (result.equal)
        return "A == B";

    return "A < B";
}

// Analyze P module behavior
void explainPAndQ() {

    cout << "\nPART 2: MODULARIZATION\n";

    cout << "\nP MODULE TRUTH TABLE\n";
    cout << "a b | G E\n";

    for (int a = 0; a <= 1; a++) {
        for (int b = 0; b <= 1; b++) {

            CompareResult r = compareOneBit(a, b);

            cout << a << " " << b << " | "
                << r.greater << " "
                << r.equal << "\n";
        }
    }

    cout << "\nP Module:\n";
    cout << "G = a AND (NOT b)\n";
    cout << "E = a XNOR b\n";

    cout << "\nLinear Q Module:\n";
    cout << "G_out = (a AND NOT b) OR "
        << "((a XNOR b) AND G_in)\n";

    cout << "E_out = (a XNOR b) AND E_in\n";

    cout << "\nTree Q Module:\n";
    cout << "G = G_high OR (E_high AND G_low)\n";
    cout << "E = E_high AND E_low\n";
}

// Calculate balanced-tree Q depth
unsigned calculateTreeDepth(unsigned n) {
    unsigned depth = 0;

    while (n > 1) {
        n = (n + 1) / 2;
        depth++;
    }

    return depth;
}

// Analyze both architectures using the truth table
void analyzeArchitectures(unsigned n) {

    uint32_t total = 1u << n;

    uint64_t linearErrors = 0;
    uint64_t treeErrors = 0;

    for (uint32_t A = 0; A < total; A++) {
        for (uint32_t B = 0; B < total; B++) {

            bool expected = behavioralComparator(A, B);

            if (linearComparator(A, B, n).greater != expected)
                linearErrors++;

            if (treeComparator(A, B, n).greater != expected)
                treeErrors++;
        }
    }

    unsigned linearP = 1;
    unsigned linearQCount = n - 1;
    unsigned linearDepth = n - 1;

    unsigned treeP = n;
    unsigned treeQCount = n - 1;
    unsigned treeDepth = calculateTreeDepth(n);

    cout << "\n================================\n";
    cout << "LINEAR ARCHITECTURE\n";
    cout << "================================\n";

    cout << "P modules needed: " << linearP << "\n";
    cout << "Q modules needed: " << linearQCount << "\n";
    cout << "Q circuit depth: " << linearDepth << "\n";

    cout << "Truth table mismatches: "
        << linearErrors << "\n";

    cout << "Connection:\n";
    cout << "P -> Q -> Q -> ... -> Final Output\n";
    cout << "Each Q receives the previous result as carry-in.\n";

    cout << "\n================================\n";
    cout << "TREE ARCHITECTURE\n";
    cout << "================================\n";

    cout << "P modules needed: " << treeP << "\n";
    cout << "Q modules needed: " << treeQCount << "\n";
    cout << "Q circuit depth: " << treeDepth << "\n";

    cout << "Truth table mismatches: "
        << treeErrors << "\n";

    cout << "Connection:\n";
    cout << "All P modules compare bits independently.\n";
    cout << "Adjacent outputs feed Q modules.\n";
    cout << "Q results combine until one output remains.\n";

    cout << "\n================================\n";
    cout << "ARCHITECTURE COMPARISON\n";
    cout << "================================\n";

    cout << "Linear: P=" << linearP
        << ", Q=" << linearQCount
        << ", Depth=" << linearDepth << "\n";

    cout << "Tree:   P=" << treeP
        << ", Q=" << treeQCount
        << ", Depth=" << treeDepth << "\n";

    if (linearErrors != 0 && treeErrors != 0) {
        cout << "Selected: No valid architecture.\n";
    }
    else if (n == 1) {
        cout << "Selected: P module only.\n";
    }
    else if (treeErrors == 0 &&
        (linearErrors != 0 ||
            treeDepth < linearDepth)) {

        cout << "Selected: BALANCED TREE\n";
        cout << "Reason: Lower dependent Q depth.\n";
    }
    else {
        cout << "Selected: LINEAR\n";
        cout << "Reason: Same or lower Q depth "
            << "with fewer P modules.\n";
    }
}

// Display linear circuit path
void printLinearPath(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    cout << "\nLINEAR CIRCUIT PATH\n";

    bool a = (A & 1u) != 0;
    bool b = (B & 1u) != 0;

    CompareResult result = compareOneBit(a, b);

    cout << "P Bit 0: " << a << " vs " << b
        << " => G=" << result.greater
        << ", E=" << result.equal << "\n";

    for (unsigned i = 1; i < n; i++) {

        a = ((A >> i) & 1u) != 0;
        b = ((B >> i) & 1u) != 0;

        result = linearQ(a, b, result);

        cout << "Q Bit " << i << ": "
            << a << " vs " << b
            << " with carry-in"
            << " => G=" << result.greater
            << ", E=" << result.equal << "\n";
    }

    cout << "\nLINEAR FINAL OUTPUT\n";
    cout << "gt = " << result.greater << "\n";
    cout << "eq = " << result.equal << "\n";
}

// Display balanced-tree circuit path
void printTreePath(
    uint32_t A,
    uint32_t B,
    unsigned n
) {
    vector<CircuitModule> modules;

    cout << "\nLevel 0: One-bit comparator modules\n";

    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {

        bool a = ((A >> i) & 1u) != 0;
        bool b = ((B >> i) & 1u) != 0;

        CompareResult current = compareOneBit(a, b);

        modules.push_back({
            current,
            string(1, a ? '1' : '0'),
            string(1, b ? '1' : '0'),
            "P" + to_string(i)
            });

        cout << "  Bit " << i << ": "
            << a << " vs " << b
            << " => G=" << current.greater
            << ", E=" << current.equal << "\n";
    }

    unsigned level = 1;

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
                    << combined.bitsA << " vs "
                    << combined.bitsB << " => "
                    << comparisonText(combined.result)
                    << "; G=" << combined.result.greater
                    << ", E=" << combined.result.equal
                    << "\n";
            }
            else {
                nextLevel.push_back(modules[i]);

                cout << "  Carry "
                    << modules[i].bitsA << " vs "
                    << modules[i].bitsB << "\n";
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

    explainPAndQ();

    analyzeArchitectures(n);

    cout << "\nCOMPARISON OF ENTERED NUMBERS\n";

    cout << "A = " << binaryA << "\n";
    cout << "B = " << binaryB << "\n";

    cout << "\nPart 1 Result:\n";
    cout << "gt = " << behavioralComparator(A, B) << "\n";

    CompareResult linear = linearComparator(A, B, n);
    CompareResult tree = treeComparator(A, B, n);

    cout << "\nLinear Result:\n";
    cout << "gt = " << linear.greater << "\n";
    cout << "eq = " << linear.equal << "\n";

    cout << "\nTree Result:\n";
    cout << "gt = " << tree.greater << "\n";
    cout << "eq = " << tree.equal << "\n";

    // Show linear and tree comparison paths
    printLinearPath(A, B, n);
    printTreePath(A, B, n);

    // Select tree only when it reduces Q depth
    bool useTree = calculateTreeDepth(n) < n - 1;

    return 0;
}
