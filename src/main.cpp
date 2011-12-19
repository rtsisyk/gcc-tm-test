#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <chrono>

// Tests
#include "Tests/ArraySumTest.h"
#include "Tests/ArrayInsertTest.h"
#include "Tests/ListInsertTest.h"
#include "Tests/TreeInsertTest.h"
#include "Tests/TreeRemoveTest.h"
#include "Tests/HashInsertTest.h"
// #include "Tests/BankTest.h"

using namespace std;

typedef map< string, function<ITest* (void)> > TestsMap;

static const TestsMap TESTS = {
    { "ArraySumTest", [] { return new ArraySumTest(); } },
    { "ArrayInsertTest", [] { return new ArrayInsertTest(); } },
    { "ListInsertTest", [] { return new ListInsertTest(); } },
    { "TreeInsertTest", [] { return new TreeInsertTest(); } },
    { "TreeRemoveTest", [] { return new TreeRemoveTest(); } },
    { "HashInsertTest", [] { return new HashInsertTest(); } },
//    { "BankTest", [] { return new BankTest(); } },
};

typedef std::chrono::high_resolution_clock Clock;



int main(int argc, char *argv[])
{
    std::cerr << "Tester compiled with LOCKTYPE_" << LOCKTYPE << endl;

    if (argc > 1) {
        cerr << "Usage: " << argv[0] << " < tests.cfg" << endl << flush;
        return 1;
    }

    cout << "Wating for configuration data from stdin..." << endl;

    size_t testsCount = 0;
    while(true) {
        string testName;
        size_t threadsCount;
        size_t inputSize;
        size_t repeatCount;

        const int sym = cin.get();
        if (sym == -1) {
            // end of file
            break;
        } else if (!std::isalpha(sym)) {
            // skip comment and white space
            cin.unget();
            string line;
            getline(cin, line);
            continue;
        } else {
            cin.unget();
            cin >> testName >> threadsCount >> inputSize >> repeatCount;
            if (!cin.good()) {
                continue;
            }
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

#ifdef LOCKTYPE_NONE
        if (threadsCount > 1) {
            cerr << "Skipping multithread test due to LOCKTYPE_NONE defined" << endl;
            continue;
        }
#endif

        ITest *test = it->second();
        double msThreadedAv = 0.0;

        bool isOk = true;

        for (size_t i = 0; i < repeatCount; i++) {
            test->generate(inputSize, threadsCount);

            test->setup();

            cout << setw(20) << left << "\tRun...";
            Clock::time_point t0 = Clock::now();
            test->run();
            Clock::time_point t1 = Clock::now();

            if (test->check()) {
                const double ms = std::chrono::nanoseconds(t1 - t0).count() * 1e-6;
                const size_t opsPerSec = static_cast<size_t>(ceil((double) inputSize / ms));

                msThreadedAv += ms;
                cout << "OK " << fixed << setprecision(3) << ms << " ms, " << opsPerSec << " ops/s" << endl;
            } else {
                isOk = false;
                cout << "FAIL " << endl;
            }

            test->teardown();

            cout << flush;
        }

        if (isOk) {
            msThreadedAv /= repeatCount;
            
            size_t opsPerSecAv = static_cast<size_t>(ceil((double) inputSize / msThreadedAv));

            cout << endl << "> " << testName << " " << LOCKTYPE << " OK "
                 << inputSize << " " << threadsCount << " " <<
                    repeatCount << " " <<
                    msThreadedAv << " " << opsPerSecAv << endl;
        } else {
            cout << endl << "> " << testName << " fail" << endl;
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
