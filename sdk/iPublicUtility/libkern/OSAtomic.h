/*
 * Copyright (C) 2011 Dmitry Skiba
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

#ifndef _OSATOMIC_INCLUDED_
#define _OSATOMIC_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Memory barrier
 */

void OSMemoryBarrier(void);


/*
 * Arithmetic functions
 *
 * All functions return new value.
 */

int32_t OSAtomicAdd32(int32_t value,volatile int32_t* target);
int32_t OSAtomicIncrement32(volatile int32_t* target);
int32_t OSAtomicDecrement32(volatile int32_t* target);

int32_t OSAtomicAdd32Barrier(int32_t value,volatile int32_t* target);
int32_t OSAtomicIncrement32Barrier(volatile int32_t* target);
int32_t OSAtomicDecrement32Barrier(volatile int32_t* target);

/*
int64_t OSAtomicAdd64(int64_t value,volatile int64_t* target);
int64_t OSAtomicIncrement64(volatile int64_t* target);
int64_t OSAtomicDecrement64(volatile int64_t* target);

int64_t OSAtomicAdd64Barrier(int64_t value,volatile int64_t* target);
int64_t OSAtomicIncrement64Barrier(volatile int64_t* target);
int64_t OSAtomicDecrement64Barrier(volatile int64_t* target);
*/

/*
 * Bitwise functions
 *
 * 'Orig' functions return original value, others return new value.
 */

int32_t OSAtomicAnd32(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicOr32(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicXor32(uint32_t value,volatile uint32_t* target);

int32_t OSAtomicAnd32Orig(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicOr32Orig(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicXor32Orig(uint32_t value,volatile uint32_t* target);

int32_t OSAtomicAnd32Barrier(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicOr32Barrier(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicXor32Barrier(uint32_t value,volatile uint32_t* target);

int32_t OSAtomicAnd32OrigBarrier(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicOr32OrigBarrier(uint32_t value,volatile uint32_t* target);
int32_t OSAtomicXor32OrigBarrier(uint32_t value,volatile uint32_t* target);

/*
 * Compare-and-swap functions
 *
 * Functions return 1 if swap occurred, 0 otherwise.
 */

int OSAtomicCompareAndSwapInt(int oldValue,int newValue,volatile int* target);
int OSAtomicCompareAndSwapPtr(void* oldValue,void* newValue,void* volatile* target);
int OSAtomicCompareAndSwapLong(long oldValue,long newValue,volatile long* target);
int OSAtomicCompareAndSwap32(int32_t oldValue,int32_t newValue,volatile int32_t* target);

int OSAtomicCompareAndSwapIntBarrier(int oldValue,int newValue,volatile int* target);
int OSAtomicCompareAndSwapPtrBarrier(void* oldValue,void* newValue,void* volatile* target);
int OSAtomicCompareAndSwapLongBarrier(long oldValue,long newValue,volatile long* target);
int OSAtomicCompareAndSwap32Barrier(int32_t oldValue,int32_t newValue,volatile int32_t* target);

/*
int OSAtomicCompareAndSwap64(int64_t oldValue,int64_t newValue,volatile int64_t* target);
int OSAtomicCompareAndSwap64Barrier(int64_t oldValue,int64_t newValue,volatile int64_t* target);
*/

/*
 * Bit functions
 *
 * Lowest bit has index of 1, 'bit/8' specifies byte in target.
 * Functions return original value of the bit (1 or 0).
 */

/*
int OSAtomicTestAndSet(uint32_t bit,volatile void* target);
int OSAtomicTestAndClear(uint32_t bit,volatile void* target);

int OSAtomicTestAndSetBarrier(uint32_t bit,volatile void* target);
int OSAtomicTestAndClearBarrier(uint32_t bit,volatile void* target);
*/

/*
 * Spinlock
 */

typedef int32_t OSSpinLock;

#define OS_SPINLOCK_INIT 0

/* OSSpinLockTry returns 1 on success, 0 otherwise. */
int OSSpinLockTry(volatile OSSpinLock* lock);
    
void OSSpinLockLock(volatile OSSpinLock* lock);
void OSSpinLockUnlock(volatile OSSpinLock* lock);

/*
 * Atomic queue
 *
 * Not yet implemented.
 */

/*
typedef struct {
    ???
} OSQueueHead;

#define OS_ATOMIC_QUEUE_INIT {...}

void OSAtomicEnqueue(OSQueueHead* list,void* item,size_t offset);
void* OSAtomicDequeue(OSQueueHead* list,size_t offset);
*/

#ifdef __cplusplus
}
#endif

#endif /* _OSATOMIC_INCLUDED_ */
