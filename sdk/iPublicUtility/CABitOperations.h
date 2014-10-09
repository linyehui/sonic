/*	Copyright � 2007 Apple Inc. All Rights Reserved.
	
	Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
			Apple Inc. ("Apple") in consideration of your agreement to the
			following terms, and your use, installation, modification or
			redistribution of this Apple software constitutes acceptance of these
			terms.  If you do not agree with these terms, please do not use,
			install, modify or redistribute this Apple software.
			
			In consideration of your agreement to abide by the following terms, and
			subject to these terms, Apple grants you a personal, non-exclusive
			license, under Apple's copyrights in this original Apple software (the
			"Apple Software"), to use, reproduce, modify and redistribute the Apple
			Software, with or without modifications, in source and/or binary forms;
			provided that if you redistribute the Apple Software in its entirety and
			without modifications, you must retain this notice and the following
			text and disclaimers in all such redistributions of the Apple Software. 
			Neither the name, trademarks, service marks or logos of Apple Inc. 
			may be used to endorse or promote products derived from the Apple
			Software without specific prior written permission from Apple.  Except
			as expressly stated in this notice, no other rights or licenses, express
			or implied, are granted by Apple herein, including but not limited to
			any patent rights that may be infringed by your derivative works or by
			other works in which the Apple Software may be incorporated.
			
			The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
			MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
			THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
			FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
			OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
			
			IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
			OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
			INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
			MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
			AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
			STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
			POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _CABitOperations_h_
#define _CABitOperations_h_

// return whether a number is a power of two
inline unsigned long IsPowerOfTwo(unsigned long x) 
{ 
	return (x & (x-1)) == 0;
}

// count the leading zeros in a word
// Metrowerks Codewarrior. powerpc native count leading zeros instruction:
// I think it's safe to remove this ...
//#define CountLeadingZeroes(x)  ((int)__cntlzw((unsigned int)x))

inline unsigned long CountLeadingZeroes(unsigned long arg)
{
// GNUC / LLVM has a builtin
#if defined(__GNUC__)
// on llvm and clang the result is defined for 0
#if (TARGET_CPU_X86 || TARGET_CPU_X86_64) && !defined(__llvm__)
	if (arg == 0) return 32;
#endif	// TARGET_CPU_X86 || TARGET_CPU_X86_64
	return __builtin_clz(arg);
#elif TARGET_OS_WIN32
	unsigned long tmp;
	__asm{
		bsr eax, arg
		mov ecx, 63
		cmovz eax, ecx
		xor eax, 31
		mov tmp, eax	// this moves the result in tmp to return.
    }
	return tmp;
#else
#error "Unsupported architecture"
#endif	// defined(__GNUC__)
}
// Alias (with different spelling)
#define CountLeadingZeros CountLeadingZeroes

inline unsigned long CountLeadingZeroesLong(unsigned long long arg)
{
// GNUC / LLVM has a builtin
#if defined(__GNUC__)
#if (TARGET_CPU_X86 || TARGET_CPU_X86_64) && !defined(__llvm__)
	if (arg == 0) return 64;
#endif	// TARGET_CPU_X86 || TARGET_CPU_X86_64
	return __builtin_clzll(arg);
#elif TARGET_OS_WIN32
	unsigned long x = CountLeadingZeroes((unsigned long)(arg >> 32));
	if(x < 32)
		return x;
	else
		return 32+CountLeadingZeroes((unsigned long)arg);
#else
#error "Unsupported architecture"
#endif	// defined(__GNUC__)
}
#define CountLeadingZerosLong CountLeadingZeroesLong

// count trailing zeroes
inline unsigned long CountTrailingZeroes(unsigned long x)
{
	return 32 - CountLeadingZeroes(~x & (x-1));
}

// count leading ones
inline unsigned long CountLeadingOnes(unsigned long x)
{
	return CountLeadingZeroes(~x);
}

// count trailing ones
inline unsigned long CountTrailingOnes(unsigned long x)
{
	return 32 - CountLeadingZeroes(x & (~x-1));
}

// number of bits required to represent x.
inline unsigned long NumBits(unsigned long x)
{
	return 32 - CountLeadingZeroes(x);
}

// base 2 log of next power of two greater or equal to x
inline unsigned long Log2Ceil(unsigned long x)
{
	return 32 - CountLeadingZeroes(x - 1);
}

// base 2 log of next power of two less or equal to x
inline unsigned long Log2Floor(unsigned long x)
{
	return 32 - CountLeadingZeroes(x) - 1;
}

// next power of two greater or equal to x
inline unsigned long NextPowerOfTwo(unsigned long x)
{
	return 1 << Log2Ceil(x);
}

// counting the one bits in a word
inline unsigned long CountOnes(unsigned long x)
{
	// secret magic algorithm for counting bits in a word.
	unsigned long t;
	x = x - ((x >> 1) & 0x55555555);
	t = ((x >> 2) & 0x33333333);
	x = (x & 0x33333333) + t;
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x << 8);
	x = x + (x << 16);
	return x >> 24;
}

// counting the zero bits in a word
inline unsigned long CountZeroes(unsigned long x)
{
	return CountOnes(~x);
}

// return the bit position (0..31) of the least significant bit
inline unsigned long LSBitPos(unsigned long x)
{
	return CountTrailingZeroes(x & -(signed long)x);
}

// isolate the least significant bit
inline unsigned long LSBit(unsigned long x)
{
	return x & -(signed long)x;
}

// return the bit position (0..31) of the most significant bit
inline unsigned long MSBitPos(unsigned long x)
{
	return 31 - CountLeadingZeroes(x);
}

// isolate the most significant bit
inline unsigned long MSBit(unsigned long x)
{
	return 1 << MSBitPos(x);
}

// Division optimized for power of 2 denominators
inline unsigned long DivInt(unsigned long numerator, unsigned long denominator)
{
	if(IsPowerOfTwo(denominator))
		return numerator >> (31 - CountLeadingZeroes(denominator));
	else
		return numerator/denominator;
}

#endif

