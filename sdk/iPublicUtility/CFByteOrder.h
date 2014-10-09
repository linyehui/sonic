/*	CFByteOrder.h
	Copyright (c) 1995-2013, Apple Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFBYTEORDER__)
#define __COREFOUNDATION_CFBYTEORDER__ 1

#if ((TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)) || (TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)) && !defined(CF_USE_OSBYTEORDER_H)
#define CF_USE_OSBYTEORDER_H 1
#endif

////////////////////////////////////////////////////////////////////////
// add by linyehui

#if !defined(CF_INLINE)
#if defined(__GNUC__)
#define CF_INLINE static __inline__
#elif defined(__MWERKS__) || defined(__cplusplus)
#define CF_INLINE static inline
#elif defined(__WIN32__)
#define CF_INLINE static __inline__
#endif
#endif

#if !defined(CF_EXTERN_C_BEGIN)
#if defined(__cplusplus)
#define CF_EXTERN_C_BEGIN extern "C" {
#define CF_EXTERN_C_END   }
#else
#define CF_EXTERN_C_BEGIN
#define CF_EXTERN_C_END
#endif
#endif

#if __LLP64__
typedef unsigned long long CFTypeID;
typedef unsigned long long CFOptionFlags;
typedef unsigned long long CFHashCode;
typedef signed long long CFIndex;
#else
typedef unsigned long CFTypeID;
typedef unsigned long CFOptionFlags;
typedef unsigned long CFHashCode;
typedef signed long CFIndex;
#endif

// add by linyehui
////////////////////////////////////////////////////////////////////////

CF_EXTERN_C_BEGIN

enum __CFByteOrder {
    CFByteOrderUnknown,
    CFByteOrderLittleEndian,
    CFByteOrderBigEndian
};
typedef CFIndex CFByteOrder;

CF_INLINE CFByteOrder CFByteOrderGetCurrent(void) {
#if CF_USE_OSBYTEORDER_H
    int32_t byteOrder = OSHostByteOrder();
    switch (byteOrder) {
    case OSLittleEndian: return CFByteOrderLittleEndian;
    case OSBigEndian: return CFByteOrderBigEndian;
    default: break;
    }
    return CFByteOrderUnknown;
#else
#if __LITTLE_ENDIAN__
    return CFByteOrderLittleEndian;
#elif __BIG_ENDIAN__
    return CFByteOrderBigEndian;
#else
    return CFByteOrderUnknown;
#endif
#endif
}

CF_INLINE unsigned short CFSwapInt16(unsigned short arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapInt16(arg);
#else
    unsigned short result;
    result = (unsigned short)(((arg << 8) & 0xFF00) | ((arg >> 8) & 0xFF));
    return result;
#endif
}

CF_INLINE unsigned int CFSwapInt32(unsigned int arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapInt32(arg);
#else
    unsigned int result;
    result = ((arg & 0xFF) << 24) | ((arg & 0xFF00) << 8) | ((arg >> 8) & 0xFF00) | ((arg >> 24) & 0xFF);
    return result;
#endif
}

CF_INLINE unsigned long long CFSwapInt64(unsigned long long arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapInt64(arg);
#else
    union CFSwap {
        unsigned long long sv;
        unsigned int ul[2];
    } tmp, result;
    tmp.sv = arg;
    result.ul[0] = CFSwapInt32(tmp.ul[1]); 
    result.ul[1] = CFSwapInt32(tmp.ul[0]);
    return result.sv;
#endif
}

CF_INLINE unsigned short CFSwapInt16BigToHost(unsigned short arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapBigToHostInt16(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt16(arg);
#endif
}

CF_INLINE unsigned int CFSwapInt32BigToHost(unsigned int arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapBigToHostInt32(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt32(arg);
#endif
}

CF_INLINE unsigned long long CFSwapInt64BigToHost(unsigned long long arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapBigToHostInt64(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt64(arg);
#endif
}

CF_INLINE unsigned short CFSwapInt16HostToBig(unsigned short arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToBigInt16(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt16(arg);
#endif
}

CF_INLINE unsigned int CFSwapInt32HostToBig(unsigned int arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToBigInt32(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt32(arg);
#endif
}

CF_INLINE unsigned long long CFSwapInt64HostToBig(unsigned long long arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToBigInt64(arg);
#elif __BIG_ENDIAN__
    return arg;
#else
    return CFSwapInt64(arg);
#endif
}

CF_INLINE unsigned short CFSwapInt16LittleToHost(unsigned short arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapLittleToHostInt16(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt16(arg);
#endif
}

CF_INLINE unsigned int CFSwapInt32LittleToHost(unsigned int arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapLittleToHostInt32(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt32(arg);
#endif
}

CF_INLINE unsigned long long CFSwapInt64LittleToHost(unsigned long long arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapLittleToHostInt64(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt64(arg);
#endif
}

CF_INLINE unsigned short CFSwapInt16HostToLittle(unsigned short arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToLittleInt16(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt16(arg);
#endif
}

CF_INLINE unsigned int CFSwapInt32HostToLittle(unsigned int arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToLittleInt32(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt32(arg);
#endif
}

CF_INLINE unsigned long long CFSwapInt64HostToLittle(unsigned long long arg) {
#if CF_USE_OSBYTEORDER_H
    return OSSwapHostToLittleInt64(arg);
#elif __LITTLE_ENDIAN__
    return arg;
#else
    return CFSwapInt64(arg);
#endif
}

typedef struct {unsigned int v;} CFSwappedfloat;
typedef struct {unsigned long long v;} CFSwappeddouble;

CF_INLINE CFSwappedfloat CFConvertfloatHostToSwapped(float arg) {
    union CFSwap {
	float v;
	CFSwappedfloat sv;
    } result;
    result.v = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt32(result.sv.v);
#endif
    return result.sv;
}

CF_INLINE float CFConvertfloatSwappedToHost(CFSwappedfloat arg) {
    union CFSwap {
	float v;
	CFSwappedfloat sv;
    } result;
    result.sv = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt32(result.sv.v);
#endif
    return result.v;
}

CF_INLINE CFSwappeddouble CFConvertdoubleHostToSwapped(double arg) {
    union CFSwap {
	double v;
	CFSwappeddouble sv;
    } result;
    result.v = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt64(result.sv.v);
#endif
    return result.sv;
}

CF_INLINE double CFConvertdoubleSwappedToHost(CFSwappeddouble arg) {
    union CFSwap {
	double v;
	CFSwappeddouble sv;
    } result;
    result.sv = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt64(result.sv.v);
#endif
    return result.v;
}

CF_INLINE CFSwappedfloat CFConvertFloatHostToSwapped(float arg) {
    union CFSwap {
	float v;
	CFSwappedfloat sv;
    } result;
    result.v = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt32(result.sv.v);
#endif
    return result.sv;
}

CF_INLINE float CFConvertFloatSwappedToHost(CFSwappedfloat arg) {
    union CFSwap {
	float v;
	CFSwappedfloat sv;
    } result;
    result.sv = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt32(result.sv.v);
#endif
    return result.v;
}

CF_INLINE CFSwappeddouble CFConvertDoubleHostToSwapped(double arg) {
    union CFSwap {
	double v;
	CFSwappeddouble sv;
    } result;
    result.v = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt64(result.sv.v);
#endif
    return result.sv;
}

CF_INLINE double CFConvertDoubleSwappedToHost(CFSwappeddouble arg) {
    union CFSwap {
	double v;
	CFSwappeddouble sv;
    } result;
    result.sv = arg;
#if __LITTLE_ENDIAN__
    result.sv.v = CFSwapInt64(result.sv.v);
#endif
    return result.v;
}

CF_EXTERN_C_END

#endif /* ! __COREFOUNDATION_CFBYTEORDER__ */

