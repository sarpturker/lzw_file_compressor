#ifndef B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65
#define B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65

typedef struct {
	unsigned int code;
	char c;
	signed int pref;
	unsigned int used;
        unsigned int shift;
	signed int shift_other;
}TableEntry;

TableEntry* decode();


#endif /* B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65 */

