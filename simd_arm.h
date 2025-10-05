#include <arm_neon.h>
#define USE_NEON
typedef uint8x16_t Vector8;
typedef uint32x4_t Vector32;

/* load/store operations */
static inline void vector8_load(Vector8 *v, const uint8 *s);
static inline void vector32_load(Vector32 *v, const uint32 *s);

/* assignment operations */
static inline Vector8 vector8_broadcast(const uint8 c);
static inline Vector32 vector32_broadcast(const uint32 c);

/* element-wise comparisons to a scalar */
static inline bool vector8_has(const Vector8 v, const uint8 c);
static inline bool vector8_has_zero(const Vector8 v);
static inline bool vector8_has_le(const Vector8 v, const uint8 c);
static inline bool vector8_is_highbit_set(const Vector8 v);
static inline bool vector32_is_highbit_set(const Vector32 v);

/* arithmetic operations */
static inline Vector8 vector8_or(const Vector8 v1, const Vector8 v2);
static inline Vector32 vector32_or(const Vector32 v1, const Vector32 v2);
static inline Vector8 vector8_ssub(const Vector8 v1, const Vector8 v2);

/*
 * comparisons between vectors
 *
 * Note: These return a vector rather than boolean, which is why we don't
 * have non-SIMD implementations.
 */
static inline Vector8 vector8_eq(const Vector8 v1, const Vector8 v2);
static inline Vector32 vector32_eq(const Vector32 v1, const Vector32 v2);

/*
 * Load a chunk of memory into the given vector.
 */
static inline void
vector8_load(Vector8 *v, const uint8 *s)
{
	*v = _mm_loadu_si128((const __m128i *) s);
}

static inline void
vector32_load(Vector32 *v, const uint32 *s)
{
	*v = _mm_loadu_si128((const __m128i *) s);
}

/*
 * Create a vector with all elements set to the same value.
 */
static inline Vector8
vector8_broadcast(const uint8 c)
{
	return _mm_set1_epi8(c);
}

static inline Vector32
vector32_broadcast(const uint32 c)
{
	return _mm_set1_epi32(c);
}

/*
 * Return true if any elements in the vector are equal to the given scalar.
 */
static inline bool
vector8_has(const Vector8 v, const uint8 c)
{
	bool		result;

	result = vector8_is_highbit_set(vector8_eq(v, vector8_broadcast(c)));

	return result;
}

/*
 * Convenience function equivalent to vector8_has(v, 0)
 */
static inline bool
vector8_has_zero(const Vector8 v)
{
	return vector8_has(v, 0);
}

/*
 * Return true if any elements in the vector are less than or equal to the
 * given scalar.
 */
static inline bool
vector8_has_le(const Vector8 v, const uint8 c)
{
	bool		result = false;

	/*
	 * Use saturating subtraction to find bytes <= c, which will present as
	 * NUL bytes.  This approach is a workaround for the lack of unsigned
	 * comparison instructions on some architectures.
	 */
	result = vector8_has_zero(vector8_ssub(v, vector8_broadcast(c)));

	return result;
}

/*
 * Return true if the high bit of any element is set
 */
static inline bool
vector8_is_highbit_set(const Vector8 v)
{
	return _mm_movemask_epi8(v) != 0;
}

/*
 * Exactly like vector8_is_highbit_set except for the input type, so it
 * looks at each byte separately.
 *
 * XXX x86 uses the same underlying type for 8-bit, 16-bit, and 32-bit
 * integer elements, but Arm does not, hence the need for a separate
 * function. We could instead adopt the behavior of Arm's vmaxvq_u32(), i.e.
 * check each 32-bit element, but that would require an additional mask
 * operation on x86.
 */
static inline bool
vector32_is_highbit_set(const Vector32 v)
{
	return vector8_is_highbit_set(v);
}

/*
 * Return the bitwise OR of the inputs
 */
static inline Vector8
vector8_or(const Vector8 v1, const Vector8 v2)
{
	return _mm_or_si128(v1, v2);
}

static inline Vector32
vector32_or(const Vector32 v1, const Vector32 v2)
{
	return _mm_or_si128(v1, v2);
}

/*
 * Return the result of subtracting the respective elements of the input
 * vectors using saturation (i.e., if the operation would yield a value less
 * than zero, zero is returned instead).  For more information on saturation
 * arithmetic, see https://en.wikipedia.org/wiki/Saturation_arithmetic
 */
static inline Vector8
vector8_ssub(const Vector8 v1, const Vector8 v2)
{
	return _mm_subs_epu8(v1, v2);
}
/*
 * Return a vector with all bits set in each lane where the corresponding
 * lanes in the inputs are equal.
 */
static inline Vector8
vector8_eq(const Vector8 v1, const Vector8 v2)
{
	return _mm_cmpeq_epi8(v1, v2);
}

static inline Vector32
vector32_eq(const Vector32 v1, const Vector32 v2)
{
	return _mm_cmpeq_epi32(v1, v2);
}
