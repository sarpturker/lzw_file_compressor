#ifndef AB1D5576_F986_4B13_AE08_ABCF50CDF907
#define AB1D5576_F986_4B13_AE08_ABCF50CDF907

typedef struct {
	unsigned int code;
	unsigned char c;
	signed int pref;
	unsigned int used;
	unsigned int shift;
}TableEntry;

TableEntry* encode(int p, int max_bit_length, int stage_param);

#endif /* AB1D5576_F986_4B13_AE08_ABCF50CDF907 */
