#include "encode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define EMPTY -1

void display_temp_table(TableEntry* table, int len, int start){
	for (int i=start;i<len;i++){
		printf("Code: %d, Pref Code: %d, Char: %c, Used: %d, Shift: %d\n", table[i].code, table[i].pref, table[i].c, table[i].used, table[i].shift);
	}
}

int get_first_available_index(TableEntry* table, int size){
	int i = 0;
	int flag = 0;
	while(table[i].pref != -2){
		if(i == size-1){
			flag = 1;
			break;
		}
		i++;	
	}

	if(!flag){
		return i;
	}
	return -1;
}
int add_to_table(TableEntry* table, int size, char cc, unsigned int P){

	int index = get_first_available_index(table, size);

	if (index!= -1){
	//insert in the table
	table[index].c = cc;
	table[index].pref = P;
	table[P].used = 1;

	return index;
	} else{
		return 0;
	}

}
TableEntry*  initialize_string_table(int size, TableEntry* table, int start_prune, unsigned int old_P,  unsigned char CC){
	TableEntry* new_table = (TableEntry*)malloc(size*sizeof(TableEntry));
	if (new_table == NULL) {
		printf("Memory allocation for string table failed\n");
		exit(1);
	}
	
	int p_new = old_P;
	if (table == NULL){
		for (int i=0;i<size;i++){
			if (i<256){
			      new_table[i].code = i;
			      new_table[i].c = (char)i;
			      new_table[i].pref = EMPTY;
			      new_table[i].used = 1;
			      new_table[i].shift = 0;
			}
			else{
			      new_table[i].code = i;
			      new_table[i].c = '\0';
			      new_table[i].pref = -2; 
			      new_table[i].used = 0;
			      new_table[i].shift = 0;
			}

		}
	}else{
		int count = 0;
		table[old_P].used = 1;
		for (int i=0; i < start_prune; i++){
			      new_table[i].code = i;
			      new_table[i].c = table[i].c;
			      new_table[i].pref = table[i].pref;
			      new_table[i].used = table[i].used;
			      new_table[i].shift = 0;
		}
		for(int i = start_prune; i<size; i++){
			if(table[i].used){
			      table[i].shift = count;
			      unsigned int current_pref = table[i].pref;
			      unsigned int new_pref;
			      if (current_pref>255){
				new_pref = current_pref - table[current_pref].shift;	
			      }else{
				new_pref = current_pref;
			      }
				
			      int new_index = i - count;

			      if(old_P == i){
			      	p_new = new_index;
			      }	
			      char cc = table[i].c;
			      new_table[new_index].code = new_index;
			      new_table[new_index].c = cc;
			      new_table[new_index].pref = new_pref; 
			      new_table[new_index].used = 1;
			      new_table[new_index].shift = 0;
			}else{
			      count++;
			}
		}
	
	
	for (int i = size - count; i<size;i++){
		new_table[i].code = i;
		new_table[i].c = '\0';
		new_table[i].pref = -2; 
		new_table[i].used = 0;
		new_table[i].shift = 0;
	}
	add_to_table(new_table, size, CC, p_new);	
	}
	return new_table;
}

int give_current_bit_size(int first_index){
	int val = first_index -1;
	int bits = 0;
	while (val > 0){
		val >>= 1;
		bits++;
	}
	return bits;
}


void display_table(TableEntry* table, int len, FILE* output_file){
	for (int i=0;i<len;i++){
		fprintf(output_file, "Code: %d, Pref Code: %d, Char: %c, Used: %d, Shift: %d\n", table[i].code, table[i].pref, table[i].c, table[i].used, table[i].shift);
	}
}


int parse_table(int P,  unsigned char CC, TableEntry* table, int size){
	for(int i=0;i<size;i++){
		if((table[i].pref == P) && (table[i].c == CC)){
			return i;
		}
		if(table[i].pref == -2){
			break;
		}
	}
	return -1;
	
}	

int set_P(TableEntry* table,  unsigned char CC){
	for(int i=0; i<256; i++){
		if(table[i].c == CC){
			return i;
		}
	}
	return -1;
}


void bit_conversion(int P, int* total, int* bp, int maxbits){
	int P_new = 0;
	int t = 0;
	int shift = 0;
	*total += maxbits;

	while (*total >= 8){
		t = *total - 8;
		P_new = P >> t;
		*bp |= P_new;
		putchar(*bp);
		*bp = 0;
		*total -= 8;	
	}
	
	if (*total>0){
		t = *total;
		shift = 8 - t;
		P <<= shift;
		*bp |= P;
	}

}


TableEntry* encode(int p, int max_bit_length, int stage_param) {
	int MAXSIZE = 1 << max_bit_length;
	unsigned char CC;
	int k;
	int P = EMPTY;
	TableEntry* table = initialize_string_table(MAXSIZE, NULL, 0, 0, 'c');
	signed int start_prune = 256;

	int prum_num = 0;
	int buffer = 0;
	int total = 0;
	int bits_before = 9;
	int current_bits = 9;

	while((k = getchar()) != EOF){
		CC = (unsigned char)k;
		int table_query = parse_table(P,CC, table, MAXSIZE); 
		//printf("checking for P: %d and c: %c\n", P, CC);
		if(table_query != -1){
			//printf("found it!\n");
			P = table_query;
		} else{
			//printf("not found :(\n");
			int check = add_to_table(table, MAXSIZE, CC, P); 

			if (stage_param){
				int first_index = get_first_available_index(table, MAXSIZE);
				if(first_index != -1){	
					current_bits = give_current_bit_size(first_index);
					int bit_change = 0;
						if (bits_before > current_bits){
							//printf("\nCHANGING BITS AMKK\n");
							bit_change = bits_before - current_bits;
							bit_conversion(0, &total, &buffer, bits_before);
							bit_conversion(bit_change, &total, &buffer, bits_before);
							bits_before = current_bits;
						}else if (bits_before < current_bits){
							bit_conversion(0, &total, &buffer, bits_before);
							bit_conversion(0, &total, &buffer, bits_before);
							bits_before = current_bits;
						}
				}
				//printf("will now do conversion for %d with current bits %d\n", P+3, current_bits);
				if((P+3) >= (1 << bits_before)){
					bit_conversion(0, &total, &buffer, bits_before);
					bit_conversion(0, &total, &buffer, bits_before);
					bit_conversion(P+3, &total, &buffer, (bits_before + 1));
					bit_conversion(0, &total, &buffer, (bits_before + 1));
					bit_conversion(1, &total, &buffer, (bits_before + 1));
				}else{
					bit_conversion(P+3, &total, &buffer, current_bits);
				}
				
			}else{
				printf("%d\n", P);
			}
			if (p){	
				if (!check){
					if (stage_param){
						prum_num++;
						bit_conversion(1, &total, &buffer, max_bit_length);

					}else{
						printf("1\n");	
					}
					//printf("Prune start: %d,P: %d, CC: %c\n", start_prune, P, CC);
					TableEntry* temp_table = initialize_string_table(MAXSIZE, table, start_prune, P, CC);
					free(table);
					table = temp_table;
					//display_temp_table(table, MAXSIZE, 256);
					start_prune = get_first_available_index(table, MAXSIZE)-1; 
					if (start_prune == -2){
						p = 0;
					}
				}
			}
			P = set_P(table, CC);
		}
		//printf("\n\n\n");
                //display_temp_table(table, MAXSIZE, 256);
	}

	if (P != EMPTY){
		if (stage_param){
			bit_conversion(P+3, &total, &buffer, current_bits);
		}else{
			printf("%d\n", P);
		}
	}	
	
	bit_conversion(3, &total, &buffer, current_bits);
	putchar(buffer);
	return table;
}

int main(int argc, char* argv[]) {

	//get maxbits and prune	
	int MAXBITS = 12;
	int p = 0;


	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-m") == 0) { 
			if (i + 1 < argc) {      	
				int potential_max_bit = atoi(argv[i + 1]);
				if ((potential_max_bit > 8) && (potential_max_bit <= 20)){ 
					MAXBITS = potential_max_bit;
				}
               			i++;                       
            		}
        	}else if (strcmp(argv[i], "-p") == 0) {
            		p = 1;                  
        	}
	}

	int stage_param = 0;	
	char* stage = getenv("STAGE");
	if (stage != NULL){
		int stage_val = atoi(stage);
		if ((stage_val == 1) || (stage_val == 2)){
			printf("%d\n", MAXBITS);
			printf("%d\n", p);	
		}else{
			putchar(MAXBITS);
			putchar(p);	
			stage_param = 1;
		}
	}else{	
		putchar(MAXBITS);
		putchar(p);
		stage_param = 1;
	}

	TableEntry* string_table = encode(p, MAXBITS, stage_param);
	
	int size = 1 << MAXBITS;

	if (getenv("DBG")) {
        	FILE* file = fopen("DBG.encode", "w");
        	if (file == NULL) {
            		fprintf(stderr, "Error opening file\n");
			free(string_table);
            		exit(1);
        	}
		//size = 267;
        	display_table(string_table, size, file);
        	fclose(file);
    	}

	free(string_table);


	exit(0);
}

