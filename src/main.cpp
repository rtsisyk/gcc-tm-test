#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <chrono>

// Tests
#include "Tests/ArraySumTest.h"
#include "Tests/VectorInsertTest.h"
#include "Tests/TreeInsertTest.h"
#include "Tests/TreeRemoveTest.h"
// #include "Tests/BankTest.h"

#include "Utils/Vector.h"

using namespace std;

typedef map< string, function<ITest* (void)> > TestsMap;

static const TestsMap TESTS = {
    { "ArraySumTest", [] { return new ArraySumTest(); } },
    { "VectorInsertTest", [] { return new VectorInsertTest(); } },
    { "TreeInsertTest", [] { return new TreeInsertTest(); } },
    { "TreeRemoveTest", [] { return new TreeRemoveTest(); } },
//    { "BankTest", [] { return new BankTest(); } },
};

typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char *argv[])
{
    if (argc > 1) {
        cerr << "Usage: " << argv[0] << " < tests.cfg" << endl << flush;
        return 1;
    }

    size_t testsCount = 0;
    while(true) {
        string testName;
        size_t threadsCount;
        size_t inputSize;
        size_t repeatCount;

        // skip comment
        if (cin.get() == '#') {
            string line;
            getline(cin, line);
            continue;
        } else {
            cin.unget();
        }

        cin >> testName >> threadsCount >> inputSize >> repeatCount;
        if (!cin.good()) {
            break;
        }

        TestsMap::const_iterator it = TESTS.find(testName);
        if (it == TESTS.end()) {
            cerr << "Test not found: " << testName << endl;
            continue;
        }

        cout << "Test: " << testName << endl;
        cout << "Threads count: " << threadsCount << endl;
        cout << "Input size: " << inputSize << endl;
        cout << "Repeat count: " << repeatCount << endl;

        ITest *test = it->second();
        for (size_t i = 0; i < repeatCount; i++) {
            cout << "Generating data..." << flush;
            test->generate(inputSize, threadsCount);
            cout << " OK" << endl;

            cout << "Running..." << endl << flush;
            cout << setw(20) << left << "\tThreaded...";
            test->setup();

            Clock::time_point t0 = Clock::now();
            test->runThreaded();
            Clock::time_point t1 = Clock::now();

            if (test->check()) {
                double ms = std::chrono::nanoseconds(t1 - t0).count() * 1e-6;
                cout << "OK " << fixed << setprecision(3) << ms << " ms" << endl;
            } else {
                cout << "FAIL " << endl;
            }

            test->teardown();

            cout << flush;

            cout << setw(20) << left << "\tSequencial...";
            test->setup();

            Clock::time_point t2 = Clock::now();
            test->runSequential();
            Clock::time_point t3 = Clock::now();

            if (test->check()) {
                double ms = std::chrono::nanoseconds(t3 - t2).count() * 1e-6;
                cout << "OK " << fixed << setprecision(3) << ms << " ms" << endl;
            } else {
                cout << "FAIL " << endl;
            }

            test->teardown();

            cout << endl << flush;
        }

        delete test;

        cout << endl << endl;
        testsCount++;
    }

    if (testsCount > 0) {
        return 0;
    } else {
        return 1;
    }
}
