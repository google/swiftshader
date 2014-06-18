/*
 * Simple sanity test of memcpy, memmove, and memset intrinsics.
 * (fixed length buffers, variable length buffers, etc.).
 * There is no include guard since this will be included multiple times,
 * under different namespaces.
 */

/* Declare first buf as uint8_t * and second as void *, to avoid C++
 * name mangling's use of substitutions. Otherwise Subzero's name
 * mangling injection will need to bump each substitution sequence ID
 * up by one (e.g., from S_ to S0_ and S1_ to S2_).
 */
int memcpy_test(uint8_t *buf, void *buf2, uint8_t init, size_t length);
int memmove_test(uint8_t *buf, void *buf2, uint8_t init, size_t length);
int memset_test(uint8_t *buf, void *buf2, uint8_t init, size_t length);

int memcpy_test_fixed_len(uint8_t init);
int memmove_test_fixed_len(uint8_t init);
int memset_test_fixed_len(uint8_t init);
