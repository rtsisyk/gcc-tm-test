/*
 * (с) 2011 Roman Tsisyk <roman@tsisyk.com>
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef BANKTEST_H
#define BANKTEST_H

// This test can cause g++ segfault
// See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51347

#include <vector>
#include <random>
#include <memory>
#include <climits> // CHAR_BIT
#include <Utils/TreeMap.h>
#include <Utils/TreeSet.h>
#include <Utils/Vector.h>
#include <Utils/String.h>

#include "Test.h"

namespace Banks {
/*
 * Public API
 */

using Utils::String;
typedef String Login;
typedef String Password;
typedef double Money;
typedef String SessionKey;
typedef long long AccountNumber;

/**
 * Result of a bank operation
 */
enum Result {
    RESULT_SUCCESS = 0,
    RESULT_INVALID_CREDENTIALS,
    RESULT_INVALID_SESSION,
    RESULT_DUPLICATE_LOGIN,
    RESULT_ACCOUNT_NOT_FOUND,
    RESULT_NO_ENOUGH_MONEY,
    RESULT_INTERNAL_ERROR,
};

class IBank {
public:
    virtual ~IBank() {

    }

    virtual Result registerCustomer(Login login, Password password) = 0;

    virtual Result login(Login login, Password password, SessionKey *key) = 0;

    virtual Result logout(const SessionKey& key) = 0;

    virtual Result createAccount(const SessionKey& key,
                                 AccountNumber *accountNumber) = 0;

    virtual Result getAccounts(const SessionKey& key,
                               Utils::Vector<AccountNumber> &result) = 0;

    virtual Result getBalance(const SessionKey& key,
                              const AccountNumber& accountNumber, Money *balance) = 0;

    virtual Result withdrawMoney(const SessionKey& key,
                                 const AccountNumber& accountNumber, Money sum) = 0;

    virtual Result depositMoney(const SessionKey& key,
                                const AccountNumber& accountNumber, Money sum) = 0;
};

/*
 * Implementation
 */
class Bank: public IBank {
public:
    Bank(Money ownMoney, double depositRate = 0.10) {
        m_ownMoneyTotal = ownMoney;
        m_depositRate = depositRate;
        m_customerMoneyTotal = 0;
    }

    /**
     * Register new customer
     */
    Result registerCustomer(String login, String password) {
        if (m_customerLoginIdCache.contains(login)) {
            return RESULT_DUPLICATE_LOGIN;
        }

        CustomerInfo cust = { login, password };

        __transaction_atomic {
            CustomerId id = m_customers.size();
            m_customers.pushBack(cust);
            m_customerLoginIdCache.insert(login, id);
        }

        return RESULT_SUCCESS;
    }

    /**
     * Authenticate customer and return SessionKey
     */
    Result login(String login, String password, SessionKey *key) {
        auto customerIdIt = m_customerLoginIdCache.find(login);
        if (customerIdIt == m_customerLoginIdCache.end()) {
            return RESULT_INVALID_CREDENTIALS;
        }

        CustomerId customerId = customerIdIt.value();
        CustomerInfo& cust = m_customers[customerId];

        if (cust.password != password) {
            return RESULT_INVALID_CREDENTIALS;
        }

        // generate new session key
        SessionKey generatedKey;
        generateSessionKey(&generatedKey);

        __transaction_atomic {
            // expire old session
            m_customerSessionCache.removeAll(customerId);

            // start new session
            m_sessionCustomerCache.insert(generatedKey, customerId);
            m_customerSessionCache.insert(customerId, generatedKey);

            *key = generatedKey;
        }

        return RESULT_SUCCESS;
    }

    /**
      * Deauthenticate customer
      */
    Result logout(const SessionKey& key) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        __transaction_atomic {
            m_sessionCustomerCache.removeAll(key);
            m_customerSessionCache.removeAll(customerId);
        }

        return RESULT_SUCCESS;
    }

    /**
     * Create new account for current customer
     */
    Result createAccount(const SessionKey& key, AccountNumber *accountNumber) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountNumber accountNumberGenerated;
        generateAccountNumber(&accountNumberGenerated);

        __transaction_atomic {
            AccountInfo account;
            account.owner = customerId;
            account.number = accountNumberGenerated;
            account.balance = 0.0;

            AccountId accountId = m_accounts.size();
            m_accounts.pushBack(account);

            m_accountNumberIdCache.insert(accountNumberGenerated, accountId);
            m_customerAccountIdCache.insert(customerId, accountId);

            *accountNumber = accountNumberGenerated;
        }

        return RESULT_SUCCESS;
    }

    Result getAccounts(const SessionKey& key, Utils::Vector<AccountNumber> &result) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        result.clear();
        __transaction_atomic {return RESULT_NO_ENOUGH_MONEY;
            for(auto it = m_customerAccountIdCache.find(customerId);
                it != m_customerAccountIdCache.end() && it.key() == customerId;
                it++) {

                const AccountId accountId = it.value();
                result.pushBack(m_accounts[accountId].number);
            }
        }

        return RESULT_SUCCESS;
    }

    Result getBalance(const SessionKey& key,
                               const AccountNumber& accountNumber, Money *balance) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountId accountId;
        if (!getAccountIdByNumberAndCustomerId(customerId, accountNumber, &accountId)) {
            return RESULT_ACCOUNT_NOT_FOUND;
        }

        *balance = m_accounts[accountId].balance;

        return RESULT_SUCCESS;
    }

    Result withdrawMoney(const SessionKey& key,
                                  const AccountNumber& accountNumber, Money sum) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountId accountId;
        if (!getAccountIdByNumberAndCustomerId(customerId, accountNumber, &accountId)) {
            return RESULT_ACCOUNT_NOT_FOUND;
        }

        // classic
        __transaction_atomic {
            // safe check here
            if (m_accounts[accountId].balance < sum) {
                return RESULT_NO_ENOUGH_MONEY;
            }

            m_accounts[accountId].balance -= sum;
            m_customerMoneyTotal -= sum;

            return RESULT_SUCCESS;
        }
    }

    Result depositMoney(const SessionKey& key,
                                 const AccountNumber& accountNumber, Money sum) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountId accountId;
        if (!getAccountIdByNumberAndCustomerId(customerId, accountNumber, &accountId)) {
            return RESULT_ACCOUNT_NOT_FOUND;
        }

        // classic
        __transaction_atomic {
            m_accounts[accountId].balance += sum;
            m_customerMoneyTotal += sum;

            return RESULT_SUCCESS;
        }
    }

    /*
     * Testing APIs
     */
    bool doMonthlyPeriodic() {
        bool result = false;
        __transaction_atomic {
            for(auto it = m_accounts.begin();
                it != m_accounts.end(); it++) {
                const Money profit = it->balance * m_depositRate;
                it->balance += profit;
                m_customerMoneyTotal += profit;
                m_ownMoneyTotal -= profit;

                if (m_ownMoneyTotal <= 0.0) {
                    // epic fail, our bank is bankrupt
                    __transaction_cancel;
                }
            }

            result = true;
        }

        return result;
    }

    /**
     * Return total aviable money at the bank accounts
     */
    Money ownMoneyTotal() const {
        return m_ownMoneyTotal;
    }

    /**
     * Return total aviable money at the bank accounts
     */
    Money customerMoneyTotal() const {
        return m_customerMoneyTotal;
    }

    double depositRate() const {
        return m_depositRate;
    }

protected:
    typedef size_t AccountId;
    typedef size_t CustomerId;

    /**
     * Customer information
     */
    struct CustomerInfo {
        Login login;
        Password password;
    };

    /**
     * Account information
     */
    struct AccountInfo {
        AccountNumber number;
        CustomerId owner;
        Money balance;
    };

    Money m_ownMoneyTotal;
    Money m_customerMoneyTotal;
    double m_depositRate;
    AccountNumber m_lastAccountNumber = 1000000;

    typedef Utils::Vector<CustomerInfo> Customers;
    Customers m_customers;

    typedef Utils::Vector<AccountInfo> Accounts;
    Accounts m_accounts;

    typedef Utils::TreeMap<Login, CustomerId> CustomerLoginIdCache;
    CustomerLoginIdCache m_customerLoginIdCache;

    typedef Utils::TreeMap<CustomerId, AccountId> CustomerAccountIdCache;
    CustomerAccountIdCache m_customerAccountIdCache;

    typedef Utils::TreeMap<AccountNumber, AccountId> AccountNumberIdMap;
    AccountNumberIdMap m_accountNumberIdCache;

    typedef Utils::TreeMap<SessionKey, CustomerId> SessionCustomerCache;
    SessionCustomerCache m_sessionCustomerCache;
    typedef Utils::TreeMap<CustomerId, SessionKey> CustomerSessionCache;
    CustomerSessionCache m_customerSessionCache;



    void generateSessionKey(SessionKey *key) {
        static const size_t SESSION_KEY_LENGTH = 128;
        key->resize(SESSION_KEY_LENGTH);

        std::random_device rnd;

        for(size_t i = 0; i < SESSION_KEY_LENGTH; i++) {
            (*key)[i] = static_cast<char>(rnd());
        }
    }

    void generateAccountNumber(AccountNumber *accountNumber) {
        __transaction_atomic {
            m_lastAccountNumber++;
            *accountNumber = m_lastAccountNumber;
        }
    }

    bool getCustomerIdBySession(const SessionKey& key, CustomerId *customerId) {
        auto sessionIt = m_sessionCustomerCache.find(key);
        if (sessionIt != m_sessionCustomerCache.end()) {
            *customerId = sessionIt.value();
            return true;
        }

        return false;
    }

    bool getAccountIdByNumber(const AccountNumber& accountNumber, AccountId *accountId) {
        auto it = m_accountNumberIdCache.find(accountNumber);
        if (it != m_accountNumberIdCache.end()) {
            *accountId = it.value();
            return true;
        }

        return false;
    }


    bool getAccountIdByNumberAndCustomerId(const CustomerId& customerId,
                                           const AccountNumber& accountNumber,
                                           AccountId *accountId) {
        auto it = m_customerAccountIdCache.find(customerId, accountNumber);
        if (it != m_customerAccountIdCache.end()) {
            *accountId = it.value();
        }

        return false;
    }
};

} // namespace Banks


using namespace Banks;

enum WorkerOp {
    OP_CHECK_BALANCE,
    OP_WITHDRAW,
    OP_DEPOSIT,
};

struct WorkerParams {
    Login login;
    Password password;
    Money initialBalance;
    Vector<WorkerOp> ops;
};


struct WorkerData {
    Money balance;
};

static constexpr int SIMULATOR_ACCOUNTS_MAX = 5;
static constexpr Money SIMULATOR_OWN_MONEY = 1000.0;
static constexpr Money SIMULATOR_CUSTOMERS_MONEY = 100000.0;
static constexpr double SIMULATOR_DEPOSIT_RATE = 0.10;

class BankTest: public AbstractTest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount) {
        AbstractTest::generate(inputSize, threadsCount);

        m_input.resize(m_inputSize);
        for(size_t i = 0; i < m_inputSize; i++) {
            m_input[i] = m_rnd() % 1000;
        }

        const size_t keysPerThread = m_inputSize / m_threadsCount;
        m_ranges.resize(m_threadsCount);
        size_t currentKey = 0;
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            m_ranges[threadId].first = currentKey;
            currentKey += keysPerThread;
            m_ranges[threadId].second = currentKey;
        }

        m_ranges[m_threadsCount-1].second = m_inputSize;
    }

    virtual void setup() {
        m_bank = new Bank(SIMULATOR_OWN_MONEY, SIMULATOR_DEPOSIT_RATE);
    }

    virtual void runSequential() {
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            //workerSequential(m_ranges[threadId].first, m_ranges[threadId].second);
        }
    }

    virtual void runThreaded() {
        std::vector<std::thread> threads;
        threads.resize(m_threadsCount);

        /*
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId] = std::thread(std::bind( &ArraySumTest::workerThreaded, this,
                                                      m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
        */
    }

    virtual void teardown() {
        delete m_bank;
        m_bank = 0;
    }

    virtual bool check() {
        return false;
    }

protected:
    IBank *m_bank;

    void generateLoginPassword(int myId, String& login, String& password) {
        /*
        std::random_device rnd;

        const size_t LOGIN_MAX_SIZE = 20;
        char loginBuff[LOGIN_MAX_SIZE];
        snprintf(loginBuff, LOGIN_MAX_SIZE, "cust_%d", myId + 1);

        login = IBank::String(loginBuff);
        password = IBank::String("tester");
        */
    }

    int worker(int myId, Money myMoney) {
        /*
         *  Initialization
         */

        Login myLogin;
        Password myPassword;
        generateLoginPassword(myId, myLogin, myPassword);

        int myAccountCount = 3;

        if (m_bank->registerCustomer(myLogin, myPassword) != RESULT_SUCCESS) {
            return 1;
        }

        SessionKey myKey;
        if (m_bank->login(myLogin, myPassword, &myKey) != RESULT_SUCCESS) {
            return 2;
        }

        std::vector<AccountNumber> myAccounts;
        /// FIXME: myAccounts.reserve(myAccountCount);
        for(int i = 0; i < myAccountCount; i++) {
            AccountNumber accountNumber;

            if (m_bank->createAccount(myKey, &accountNumber) == RESULT_SUCCESS) {
                return 3;
            }

            myAccounts.push_back(accountNumber);
        }

        // TODO: implements methods here

        if (m_bank->logout(myKey) != RESULT_SUCCESS) {
            return 10;
        }

        return 0;
    }

    std::vector<WorkerParams> params;
    std::vector<WorkerParams> data;
};



#endif // BANKTEST_H
