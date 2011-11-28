#ifndef BANKTEST_H
#define BANKTEST_H

#include <vector>
#include <random>
#include <memory>
#include <climits> // CHAR_BIT
#include <Utils/TreeMap.h>
#include <Utils/TreeSet.h>
#include <Utils/Vector.h>
#include <Utils/String.h>

#include "Test.h"

#error This test is currently broken (GCC segfault)

namespace Banks {

typedef Utils::String String;
typedef String Login;
typedef String Password;
typedef double Money;
typedef String SessionKey;
typedef long long AccountNumber;

/**
 * Result of a bank operation
 */
enum OperationResult {
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

    virtual OperationResult registerCustomer(Login login, Password password) = 0;
    virtual OperationResult login(Login login, Password password, SessionKey *key) = 0;
    virtual OperationResult logout(const SessionKey& key) = 0;
    virtual OperationResult createAccount(const SessionKey& key, AccountNumber *accountNumber) = 0;
    virtual OperationResult getAccounts(const SessionKey& key, Utils::Vector<AccountNumber> &result) = 0;
    virtual OperationResult getBalance(const SessionKey& key, const AccountNumber& accountNumber, Money *balance) = 0;
    virtual OperationResult withdrawMoney(const SessionKey& key, const AccountNumber& accountNumber, Money sum) = 0;
    virtual OperationResult depositMoney(const SessionKey& key, const AccountNumber& accountNumber, Money sum) = 0;
};

class Bank: public IBank {
public:
    Bank(Money ownMoney, double depositRate = 0.10) {
        m_ownMoneyTotal = ownMoney;
        m_depositRate = depositRate;
        m_customerMoneyTotal = 0;
    }

    /*
     *  Public APIs from IBank
     */

    /**
     * Register new customer
     */
    OperationResult registerCustomer(String login, String password) {
        if (m_customerLoginIdMap.count(login) > 0) {
            return RESULT_DUPLICATE_LOGIN;
        }

        CustomerInfo cust;
        cust.login = login;
        cust.password = password;

        __transaction_atomic {
            CustomerId id = m_customers.size();
            m_customers.pushBack(cust);
            /// FIXME
            m_customerLoginIdMap.insert(login, id);
        }

        return RESULT_SUCCESS;
    }

    /**
     * Authenticate customer and return SessionKey
     */
    OperationResult login(String login, String password, SessionKey *key) {
        CustomerId customerId;
        if (!getCustomerIdByLogin(login, &customerId)) {
            return RESULT_INVALID_CREDENTIALS;
        }

        if (m_customers[customerId].password != password) {
            return RESULT_INVALID_CREDENTIALS;
        }


        // generate new
        SessionKey generatedKey;
        generateSessionKey(&generatedKey);

        /*
        __transaction_atomic {

            CustomerSessionMap::Iterator sessionIt = m_customerSessions.find(customerId);
            if (sessionIt != m_customerSessions.end()) {
                // reuse key
                (*key) = sessionIt.value();
                return RESULT_SUCCESS;
            } else {
                /// FIXME: m_sessions.insert(generatedKey, customerId);
                /// FIXME: m_customerSessions.insert(customerId, generatedKey);
                *key = generatedKey;
                return RESULT_SUCCESS;
            }
        }
        */
    }

    /**
      * Deauthenticate customer
      */
    OperationResult logout(const SessionKey& key) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        /// FIXME:
        /*
        __transaction_atomic {
            m_sessions.removeAll(key);
            m_customerSessions.removeAll(customerId);
            return RESULT_SUCCESS;
        }
        */
    }

    /**
     *
     */
    OperationResult createAccount(const SessionKey& key, AccountNumber *accountNumber) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountNumber accountNumberGenerated;
        generateAccountNumber(&accountNumberGenerated);

        __transaction_atomic {
            AccountInfo account;
            account.balance = 0.0;

            AccountId accountId = m_accounts.size();
            m_accounts.pushBack(account);

            /// FIXME
            /// m_accountNumberIdMap.insert(accountNumberGenerated, accountId);
            /// m_customerAccounts.insert(customerId, accountNumberGenerated);

            *accountNumber = accountNumberGenerated;
            return RESULT_SUCCESS;
        }
    }

    OperationResult getAccounts(const SessionKey& key, Utils::Vector<AccountNumber> &result) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        /// FIXME
        /*
        __transaction_atomic {
            result.clear();

            for(CustomerAccountNumberMap::Iterator it = m_customerAccounts.find(customerId);
                it != m_customerAccounts.end() && it.key() == customerId; it++) {
                result.pushBack(it.value());
            }

            return RESULT_SUCCESS;
        }
        */
    }

    OperationResult getBalance(const SessionKey& key, const AccountNumber& accountNumber, Money *balance) {
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

    OperationResult withdrawMoney(const SessionKey& key, const AccountNumber& accountNumber, Money sum) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountId accountId;
        if (!getAccountIdByNumberAndCustomerId(customerId, accountNumber, &accountId)) {
            return RESULT_ACCOUNT_NOT_FOUND;
        }

        // classical
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

    OperationResult depositMoney(const SessionKey& key, const AccountNumber& accountNumber, Money sum) {
        CustomerId customerId;
        if (!getCustomerIdBySession(key, &customerId)) {
            return RESULT_INVALID_SESSION;
        }

        AccountId accountId;
        if (!getAccountIdByNumberAndCustomerId(customerId, accountNumber, &accountId)) {
            return RESULT_ACCOUNT_NOT_FOUND;
        }

        // classical
        __transaction_atomic {
            // safe check here
            if (m_accounts[accountId].balance < sum) {
                return RESULT_NO_ENOUGH_MONEY;
            }

            m_accounts[accountId].balance += sum;
            m_customerMoneyTotal += sum;

            return RESULT_SUCCESS;
        }
    }

    /*
     * Testing APIs
     */

    void doMonthlyPeriodics() {
        for(AccountVector::Iterator it = m_accounts.begin();
            it != m_accounts.end(); it++) {
            __transaction_atomic {
                Money profit = it->balance * m_depositRate;
                it->balance += profit;
                m_customerMoneyTotal += profit;
                m_ownMoneyTotal -= profit;
            }
        }
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
     * Account information
     */
    struct AccountInfo {
        Money balance;
    };

    /**
      * Customer info for the bank
      */
    struct CustomerInfo {
        Login login;
        Password password;
    };

    void generateSessionKey(SessionKey *key) {
        static const size_t SESSION_KEY_LENGTH = 128;
        key->resize(SESSION_KEY_LENGTH);

        std::random_device rnd;

        for(size_t i = 0; i < SESSION_KEY_LENGTH; i++) {
            (*key)[i] = static_cast<char>(rnd());
        }
    }

    void generateAccountNumber(AccountNumber *accountNumber) {
        std::random_device rnd;

        *accountNumber = rnd();
    }

    bool getCustomerIdByLogin(const String& login, CustomerId *customerId) const {
        CustomerLoginIdMap::Iterator customerIdIt = m_customerLoginIdMap.find(login);
        if (customerIdIt == m_customerLoginIdMap.end()) {
            return false;
        }

        *customerId = customerIdIt.value();
        return true;
    }

    bool getCustomerIdBySession(const SessionKey& key, CustomerId *customerId) {
        SessionCustomerMap::Iterator sessionIt = m_sessions.find(key);
        if (sessionIt != m_sessions.end()) {
            *customerId = sessionIt.value();
            return true;
        }

        return false;
    }

    bool getAccountIdByNumber(const AccountNumber& accountNumber, AccountId *accountId) {
        AccountNumberIdMap::Iterator it = m_accountNumberIdMap.find(accountNumber);
        if (it != m_accountNumberIdMap.end()) {
            *accountId = it.value();
            return true;
        }

        return false;
    }


    bool getAccountIdByNumberAndCustomerId(const CustomerId& customerId,
                                           const AccountNumber& accountNumber,
                                           AccountId *accountId) {
        for(CustomerAccountNumberMap::Iterator it = m_customerAccounts.find(customerId);
            it != m_customerAccounts.end() && it.key() == customerId; it++) {
            if (it.value() == accountNumber) {
                return getAccountIdByNumber(accountNumber, accountId);
            }
        }

        return false;
    }

    Money m_ownMoneyTotal;
    Money m_customerMoneyTotal;
    double m_depositRate;

    typedef Utils::Vector<CustomerInfo> CustomerVector;
    CustomerVector m_customers;
    typedef Utils::TreeMap<Login, AccountId> CustomerLoginIdMap;
    CustomerLoginIdMap m_customerLoginIdMap;

    typedef Utils::Vector<AccountInfo> AccountVector;
    AccountVector m_accounts;
    typedef Utils::TreeMap<AccountNumber, AccountId> AccountNumberIdMap;
    AccountNumberIdMap m_accountNumberIdMap;

    typedef Utils::TreeMap<CustomerId, AccountNumber> CustomerAccountNumberMap;
    CustomerAccountNumberMap m_customerAccounts;

    typedef Utils::TreeMap<SessionKey, CustomerId> SessionCustomerMap;
    SessionCustomerMap m_sessions;
    typedef Utils::TreeMap<CustomerId, SessionKey> CustomerSessionMap;
    CustomerSessionMap m_customerSessions;
};

} // namespace Banks

using namespace Banks;

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

    static constexpr int SIMULATOR_ACCOUNTS_MAX = 5;
    static constexpr Money SIMULATOR_OWN_MONEY = 1000.0;
    static constexpr Money SIMULATOR_CUSTOMERS_MONEY = 100000.0;
    static constexpr double SIMULATOR_DEPOSIT_RATE =0.10;

    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
};



#endif // BANKTEST_H
