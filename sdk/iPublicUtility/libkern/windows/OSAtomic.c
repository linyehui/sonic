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

#include <libkern/OSAtomic.h>
#include <windows.h>

/*
 * Memory barrier
 */

void OSMemoryBarrier(void) {
    MemoryBarrier();
}

/*
 * Arithmetic functions
 */

int32_t OSAtomicAdd32Barrier(int32_t value,volatile int32_t* target) {
    return value+InterlockedExchangeAdd(target,value);
}

int32_t OSAtomicIncrement32Barrier(volatile int32_t* target) {
    return InterlockedIncrement(target);
}

int32_t OSAtomicDecrement32Barrier(volatile int32_t* target) {
    return InterlockedDecrement(target);
}

/*
int64_t OSAtomicAdd64(int64_t value,volatile int64_t* target);
int64_t OSAtomicIncrement64(volatile int64_t* target);
int64_t OSAtomicDecrement64(volatile int64_t* target);

int64_t OSAtomicAdd64Barrier(int64_t value,volatile int64_t* target);
int64_t OSAtomicIncrement64Barrier(volatile int64_t* target);
int64_t OSAtomicDecrement64Barrier(volatile int64_t* target);
*/

/* inlines */

int32_t OSAtomicAdd32(int32_t value,volatile int32_t* target) {
    return OSAtomicAdd32Barrier(value,target);
}

int32_t OSAtomicIncrement32(volatile int32_t* target) {
    return OSAtomicIncrement32Barrier(target);
}    
int32_t OSAtomicDecrement32(volatile int32_t* target) {
    return OSAtomicDecrement32Barrier(target);
}

/*
 * Bitwise functions
 */

#define IMPLEMENT_BITWISE_ORIG(Name,Operator) \
    int32_t Name(uint32_t value,volatile uint32_t* target) { \
        while (1) { \
            uint32_t targetValue=*target; \
            if (targetValue==InterlockedCompareExchange( \
                    target,(targetValue Operator value),targetValue)) \
            { \
                return targetValue; \
            } \
        } \
    }

IMPLEMENT_BITWISE_ORIG(OSAtomicAnd32OrigBarrier,&);
IMPLEMENT_BITWISE_ORIG(OSAtomicOr32OrigBarrier,|);
IMPLEMENT_BITWISE_ORIG(OSAtomicXor32OrigBarrier,^);

#undef IMPLEMENT_BITWISE_ORIG

/* inlines */

int32_t OSAtomicAnd32Orig(uint32_t value,volatile uint32_t* target) {
    return OSAtomicAnd32OrigBarrier(value,target);
}
int32_t OSAtomicOr32Orig(uint32_t value,volatile uint32_t* target) {
    return OSAtomicOr32OrigBarrier(value,target);
}
int32_t OSAtomicXor32Orig(uint32_t value,volatile uint32_t* target) {
    return OSAtomicXor32OrigBarrier(value,target);
}

int32_t OSAtomicAnd32(uint32_t value,volatile uint32_t* target) {
    return OSAtomicAnd32Orig(value,target) & value;
}
int32_t OSAtomicOr32(uint32_t value,volatile uint32_t* target) {
    return OSAtomicOr32Orig(value,target) | value;
}
int32_t OSAtomicXor32(uint32_t value,volatile uint32_t* target) {
    return OSAtomicXor32Orig(value,target) ^ value;
}

int32_t OSAtomicAnd32Barrier(uint32_t value,volatile uint32_t* target) {
    return OSAtomicAnd32OrigBarrier(value,target) & value;
}
int32_t OSAtomicOr32Barrier(uint32_t value,volatile uint32_t* target) {
    return OSAtomicOr32OrigBarrier(value,target) | value;
}
int32_t OSAtomicXor32Barrier(uint32_t value,volatile uint32_t* target) {
    return OSAtomicXor32OrigBarrier(value,target) ^ value;
}

/*
 * Compare-and-swap functions
 */

int OSAtomicCompareAndSwapIntBarrier(int oldValue,int newValue,volatile int* target) {
    return InterlockedCompareExchange(target,newValue,oldValue)==oldValue;
}

int OSAtomicCompareAndSwapPtrBarrier(void* oldValue,void* newValue,void* volatile* target) {
    return InterlockedCompareExchangePointer(target,newValue,oldValue)==oldValue;
}

int OSAtomicCompareAndSwapLongBarrier(long oldValue,long newValue,volatile long* target) {
    return OSAtomicCompareAndSwapIntBarrier(oldValue,newValue,target);
}

int OSAtomicCompareAndSwap32Barrier(int32_t oldValue,int32_t newValue,volatile int32_t* target) {
    return OSAtomicCompareAndSwapIntBarrier(oldValue,newValue,target);
}

/*
int OSAtomicCompareAndSwap64(int64_t oldValue,int64_t newValue,volatile int64_t* target);
int OSAtomicCompareAndSwap64Barrier(int64_t oldValue,int64_t newValue,volatile int64_t* target);
*/

/* inlines */

int OSAtomicCompareAndSwapInt(int oldValue,int newValue,volatile int* target) {
    return OSAtomicCompareAndSwapIntBarrier(oldValue,newValue,target);
}
int OSAtomicCompareAndSwapPtr(void* oldValue,void* newValue,void* volatile* target) {
    return OSAtomicCompareAndSwapPtrBarrier(oldValue,newValue,target);
}
int OSAtomicCompareAndSwapLong(long oldValue,long newValue,volatile long* target) {
    return OSAtomicCompareAndSwapInt((int)oldValue,(int)newValue,(volatile int*)target);
}
int OSAtomicCompareAndSwap32(int32_t oldValue,int32_t newValue,volatile int32_t* target) {
    return OSAtomicCompareAndSwapInt((int)oldValue,(int)newValue,(volatile int*)target);
}

/*
 * Bit functions
 */

/*
int OSAtomicTestAndSet(uint32_t bit,volatile void* target);
int OSAtomicTestAndSetBarrier(uint32_t bit,volatile void* target);
int OSAtomicTestAndClear(uint32_t bit,volatile void* target);
int OSAtomicTestAndClearBarrier(uint32_t bit,volatile void* target);
*/

/*
 * Spinlock
 */

void OSSpinLockLock(volatile OSSpinLock* lock) {
    while (!OSSpinLockTry(lock)) {
        Sleep(1);
    }
}

void OSSpinLockUnlock(volatile OSSpinLock* lock) {
    InterlockedExchange(lock,0);
}

int OSSpinLockTry(volatile OSSpinLock* lock) {
    return InterlockedCompareExchange(lock,1,0)==0;
}

/*
 * Atomic queue
 */

/*
void OSAtomicEnqueue(OSQueueHead* list,void* item,size_t offset);
void* OSAtomicDequeue(OSQueueHead* list,size_t offset);
*/
