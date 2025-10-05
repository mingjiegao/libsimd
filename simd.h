#ifndef SIMD_H
#define SIMD_H

#include <stdbool.h>

#ifndef HAVE_INT8
typedef signed char int8;		/* == 8 bits */
typedef signed short int16;		/* == 16 bits */
typedef signed int int32;		/* == 32 bits */
#endif							/* not HAVE_INT8 */

#ifndef HAVE_UINT8
typedef unsigned char uint8;	/* == 8 bits */
typedef unsigned short uint16;	/* == 16 bits */
typedef unsigned int uint32;	/* == 32 bits */
#endif							/* not HAVE_UINT8 */



#if (defined(__x86_64__) || defined(_M_AMD64))
	#include "simd_x86.h"
#elif defined(__aarch64__) && defined(__ARM_NEON)
	#include "simd_arm.h"
#else
	#error "Unsupported platform: No SIMD implementation available for this architecture."
#endif

extern bool lfind8(uint8 key, uint8 *base, uint32 nelem);
extern bool lfind8_le(uint8 key, uint8 *base, uint32 nelem);
extern bool lfind32(uint32 key, uint32 *base, uint32 nelem);

#endif	/* SIMD_H */
