#ifndef TREEREMOVETEST_H
#define TREEREMOVETEST_H

#include "Test.h"
#include "TreeInsertTest.h"

class TreeRemoveTest: public TreeInsertTest {
public:
    virtual void setup() {
        m_sharedSet.clear();
        for(int val: m_input) {
            m_sharedSet.insertMulti(val);
        }
    }

    virtual void teardown() {
        m_sharedSet.clear();
    }

    virtual bool check() {
        return m_sharedSet.isEmpty();
    }

protected:
   virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            BEGIN_CRITICAL_SECTION();
                m_sharedSet.removeAll(m_input[i]);
            END_CRITICAL_SECTION();
        }
    }
};

#endif // TREEREMOVETEST_H
