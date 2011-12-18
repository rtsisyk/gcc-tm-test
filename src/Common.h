#ifndef LOCKS_H
#define LOCKS_H



#if defined(LOCKTYPE_NONE)
#define BEGIN_CRITICAL_SECTION() {
#define END_CRITICAL_SECTION() }
#define LOCKTYPE "NONE"
#elif defined(LOCKTYPE_MUTEX)
#define BEGIN_CRITICAL_SECTION() std::lock_guard<std::mutex> locker(lock); {
#define END_CRITICAL_SECTION() }
#define LOCKTYPE "MUTEX"
#elif defined(LOCKTYPE_TM)
#define BEGIN_CRITICAL_SECTION() __transaction_atomic {
#define END_CRITICAL_SECTION() }
#define LOCKTYPE "TM"
#else
#error Please define LOCKTYPE_NONE or LOCKTYPE_TM or LOCKTYPE_MUTEX
#endif


#endif // LOCKS_H
