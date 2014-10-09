/*
 * Copyright (C) 2011 Dmitry Skiba
 * Copyright (C) The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <libkern/OSAtomic.h>
#include <sys/atomics.h>

/*
 * Lock a non-recursive mutex.
 *
 * There are three states:
 *   0 (unlocked, no contention)
 *   1 (locked, no contention)
 *   2 (locked, contention)
 *
 * Non-recursive mutexes don't use the thread-id or counter fields, and the
 * "type" value is zero, so the only bits that will be set are the ones in
 * the lock state field.
 */
void OSSpinLockLock(volatile OSSpinLock* lock) {
    /*
     * The common case is an unlocked mutex, so we begin by trying to
     * change the lock's state from 0 to 1.  __atomic_cmpxchg() returns 0
     * if it made the swap successfully.  If the result is nonzero, this
     * lock is already held by another thread.
     */
    if (__atomic_cmpxchg(0, 1, lock ) != 0) {
        /*
         * We want to go to sleep until the mutex is available, which
         * requires promoting it to state 2.  We need to swap in the new
         * state value and then wait until somebody wakes us up.
         *
         * __atomic_swap() returns the previous value.  We swap 2 in and
         * see if we got zero back; if so, we have acquired the lock.  If
         * not, another thread still holds the lock and we wait again.
         *
         * The second argument to the __futex_wait() call is compared
         * against the current value.  If it doesn't match, __futex_wait()
         * returns immediately (otherwise, it sleeps for a time specified
         * by the third argument; 0 means sleep forever).  This ensures
         * that the mutex is in state 2 when we go to sleep on it, which
         * guarantees a wake-up call.
         */
        while (__atomic_swap(2, lock ) != 0)
            __futex_wait(lock, 2, 0);
    }
}

int OSSpinLockTry(volatile OSSpinLock* lock) {
    return __atomic_cmpxchg(0, 1, lock ) == 0;
}

/*
 * Release a non-recursive mutex.  The caller is responsible for determining
 * that we are in fact the owner of this lock.
 */
void OSSpinLockUnlock(volatile OSSpinLock* lock) {
    /*
     * The mutex value will be 1 or (rarely) 2.  We use an atomic decrement
     * to release the lock.  __atomic_dec() returns the previous value;
     * if it wasn't 1 we have to do some additional work.
     */
    if (__atomic_dec(lock) != 1) {
        /*
         * Start by releasing the lock.  The decrement changed it from
         * "contended lock" to "uncontended lock", which means we still
         * hold it, and anybody who tries to sneak in will push it back
         * to state 2.
         *
         * Once we set it to zero the lock is up for grabs.  We follow
         * this with a __futex_wake() to ensure that one of the waiting
         * threads has a chance to grab it.
         *
         * This doesn't cause a race with the swap/wait pair in
         * _normal_lock(), because the __futex_wait() call there will
         * return immediately if the mutex value isn't 2.
         */
        *lock = 0;

        /*
         * Wake up one waiting thread.  We don't know which thread will be
         * woken or when it'll start executing -- futexes make no guarantees
         * here.  There may not even be a thread waiting.
         *
         * The newly-woken thread will replace the 0 we just set above
         * with 2, which means that when it eventually releases the mutex
         * it will also call FUTEX_WAKE.  This results in one extra wake
         * call whenever a lock is contended, but lets us avoid forgetting
         * anyone without requiring us to track the number of sleepers.
         *
         * It's possible for another thread to sneak in and grab the lock
         * between the zero assignment above and the wake call below.  If
         * the new thread is "slow" and holds the lock for a while, we'll
         * wake up a sleeper, which will swap in a 2 and then go back to
         * sleep since the lock is still held.  If the new thread is "fast",
         * running to completion before we call wake, the thread we
         * eventually wake will find an unlocked mutex and will execute.
         * Either way we have correct behavior and nobody is orphaned on
         * the wait queue.
         */
        __futex_wake(lock, 1);
    }
}
