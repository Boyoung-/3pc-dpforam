#ifndef BIT_PERM_H_
#define BIT_PERM_H_

typedef __uint128_t t_bits;
typedef int t_int;
typedef unsigned t_uint;

#define ld_bits 7

const t_bits a_bfly_mask[] = {
    // 0..ld_bits
    // For butterfly ops
    // = all_bits / ((1 << (1 << i)) + 1)
    (((t_bits)(0x5555555555555555ULL)) << 64) + 0x5555555555555555ULL,   // 0
    (((t_bits)(0x3333333333333333ULL)) << 64) + 0x3333333333333333ULL,   // 1
    (((t_bits)(0x0f0f0f0f0f0f0f0fULL)) << 64) + 0x0f0f0f0f0f0f0f0fULL,   // 2
    (((t_bits)(0x00ff00ff00ff00ffULL)) << 64) + 0x00ff00ff00ff00ffULL,   // 3
    (((t_bits)(0x0000ffff0000ffffULL)) << 64) + 0x0000ffff0000ffffULL,   // 4
    (((t_bits)(0x00000000ffffffffULL)) << 64) + 0x00000000ffffffffULL,   // 5
    (((t_bits)(0x0000000000000000ULL)) << 64) + 0xffffffffffffffffULL,   // 6
    (((t_bits)(0xffffffffffffffffULL)) << 64) + 0xffffffffffffffffULL};  // 7

t_bits bit_permute_step_simple(t_bits x, t_bits m, t_uint shift) {
    return ((x & m) << shift) | ((x >> shift) & m);
}

t_bits general_reverse_bits(t_bits x, t_int k) {
    // Swap all subwords of given levels.
    // See Hacker's Delight, 7.1 "Generalized Bit Reversal"
    // k: set of t_subword, i.e. one bit per subword size.

    t_int i, j;
    t_bits m;

    for (i = 0; i <= ld_bits - 1; ++i) {  // UNROLL
        j = 1 << i;
        if ((k & j) != 0) {
            // x = bit_index_complement(x,j);
            m = a_bfly_mask[i];
            x = bit_permute_step_simple(x, m, j);
        }
    }
    return x;
}

#endif /* BIT_PERM_H_ */
