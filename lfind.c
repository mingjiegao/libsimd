#include "simd.h"


/*
 * lfind8
 *
 * Return true if there is an element in 'base' that equals 'key', otherwise
 * return false.
 */
bool
lfind8(uint8 key, uint8 *base, uint32 nelem)
{
	uint32		i;

	/* round down to multiple of vector length */
	uint32		tail_idx = nelem & ~(sizeof(Vector8) - 1);
	Vector8		chunk;

	for (i = 0; i < tail_idx; i += sizeof(Vector8))
	{
		vector8_load(&chunk, &base[i]);
		if (vector8_has(chunk, key))
			return true;
	}

	/* Process the remaining elements one at a time. */
	for (; i < nelem; i++)
	{
		if (key == base[i])
			return true;
	}

	return false;
}

/*
 * lfind8_le
 *
 * Return true if there is an element in 'base' that is less than or equal to
 * 'key', otherwise return false.
 */
bool
lfind8_le(uint8 key, uint8 *base, uint32 nelem)
{
	uint32		i;

	/* round down to multiple of vector length */
	uint32		tail_idx = nelem & ~(sizeof(Vector8) - 1);
	Vector8		chunk;

	for (i = 0; i < tail_idx; i += sizeof(Vector8))
	{
		vector8_load(&chunk, &base[i]);
		if (vector8_has_le(chunk, key))
			return true;
	}

	/* Process the remaining elements one at a time. */
	for (; i < nelem; i++)
	{
		if (base[i] <= key)
			return true;
	}

	return false;
}


bool
lfind32(uint32 key, uint32 *base, uint32 nelem)
{
	uint32		i = 0;

	/*
	 * For better instruction-level parallelism, each loop iteration operates
	 * on a block of four registers.  Testing for SSE2 has showed this is ~40%
	 * faster than using a block of two registers.
	 */
	const Vector32 keys = vector32_broadcast(key);	/* load copies of key */
	const uint32 nelem_per_vector = sizeof(Vector32) / sizeof(uint32);
	const uint32 nelem_per_iteration = 4 * nelem_per_vector;

	/* round down to multiple of elements per iteration */
	const uint32 tail_idx = nelem & ~(nelem_per_iteration - 1);

	for (i = 0; i < tail_idx; i += nelem_per_iteration)
	{
		Vector32	vals1,
					vals2,
					vals3,
					vals4,
					result1,
					result2,
					result3,
					result4,
					tmp1,
					tmp2,
					result;

		/* load the next block into 4 registers */
		vector32_load(&vals1, &base[i]);
		vector32_load(&vals2, &base[i + nelem_per_vector]);
		vector32_load(&vals3, &base[i + nelem_per_vector * 2]);
		vector32_load(&vals4, &base[i + nelem_per_vector * 3]);

		/* compare each value to the key */
		result1 = vector32_eq(keys, vals1);
		result2 = vector32_eq(keys, vals2);
		result3 = vector32_eq(keys, vals3);
		result4 = vector32_eq(keys, vals4);

		/* combine the results into a single variable */
		tmp1 = vector32_or(result1, result2);
		tmp2 = vector32_or(result3, result4);
		result = vector32_or(tmp1, tmp2);

		/* see if there was a match */
		if (vector32_is_highbit_set(result))
		{
			return true;
		}
	}

	/* Process the remaining elements one at a time. */
	for (; i < nelem; i++)
	{
		if (key == base[i])
		{
			return true;
		}
	}

	return false;
}