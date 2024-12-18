#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"

#define EMPTY -1

void display_temp_table(TableEntry* table, int len, int start){
        for (int i=start;i<len;i++){
                printf("Code: %d, Pref Code: %d, Char: %c, Used: %d, Shift: %d, Shift Other: %d\n", table[i].code, table[i].pref, table[i].c, table[i].used, table[i].shift, table[i].shift_other);
        }
}

TableEntry*  initialize_string_table(int size, TableEntry* table, int start_prune, unsigned int old_P){
        TableEntry* new_table = (TableEntry*)malloc(size*sizeof(TableEntry));
        if (new_table == NULL) {
                printf("Memory allocation for string table failed\n");
                exit(1);
        }
        if (table == NULL){
                for (int i=0;i<size;i++){
                        if (i<256){
                              new_table[i].code = i;
                              new_table[i].c = (char)i;
                              new_table[i].pref = EMPTY;
                              new_table[i].used = 1;
                              new_table[i].shift = 0;
			      new_table[i].shift_other = -1;
                        }
                        else{
                              new_table[i].code = i;
                              new_table[i].c = '\0';
                              new_table[i].pref = -2;
                              new_table[i].used = 0;
                              new_table[i].shift = 0;
			      new_table[i].shift_other = -1;
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
			      new_table[i].shift_other = -1;
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
                              char cc = table[i].c;
                              new_table[new_index].code = new_index;
                              new_table[new_index].c = cc;
                              new_table[new_index].pref = new_pref;
                              new_table[new_index].used = 1;
			      new_table[new_index].shift = 0;	
			      if(old_P == i){
			              new_table[new_index].shift_other = i;
                              }else{
			      	      new_table[new_index].shift_other = -1;
			      }
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
        }
        return new_table;
}


void display_table(TableEntry* table, int len, FILE* output_file){
        for (int i=0;i<len;i++){
                fprintf(output_file, "Code: %d, Pref Code: %d, Char: %c, Used: %d, Shift: %d, Shift Other: %d\n", table[i].code, table[i].pref, table[i].c, table[i].used, table[i].shift, table[i].shift_other);
        }
}

int parse_table(int P, char CC, TableEntry* table, int size){
        for(int i=0;i<size;i++){
                if((table[i].pref == P) && (table[i].c == CC)){
                        return i;
                }
                if(table[i].pref == -2){
                        break;
                }
        }
        return 0;

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
        return 0;
}
int set_P(TableEntry* table, char CC){
        for(int i=0; i<256; i++){
                if(table[i].c == CC){
                        return i;
                }
        }
        return -1;
}

char table_output_query(TableEntry* table, int code, int start_prune){
	int index = code;
	int prefix = table[index].pref;
	for(int i = 256; i<start_prune; i++){
                if (table[i].shift_other == index){
                prefix = table[i].pref;
		index = i;
                }
	}	
	if(prefix != -1){
		char first_S = table_output_query(table, prefix, start_prune);
		char print = table[index].c;
		//printf("%d", (int)(print));
		putchar(print);
		
		return first_S;
	} else {
		char print = table[index].c;
		putchar(print);
		return print;
	}
		
}
int check_code(TableEntry* table, int index){
	int prefix = table[index].pref;	
	if(prefix == -2){
                return 0;
        }else{
		return 1;
	}		
	
}

int get_size(int stage_param){
	int input;
	int MAXBITS;
	
	if (stage_param){
		input = getchar();
		MAXBITS = input;
	}else{
		if(scanf("%d", &input) == 1){
			MAXBITS = input;
		}
	}
	int size = 1 << MAXBITS;
	return size;
}

int add_to_table(TableEntry* table, int size, char cc, unsigned int P, int start_prune, int edge){

        int index = get_first_available_index(table, size);

        if (index){
        //insert in the table
        table[index].c = cc;
        table[index].pref = P;
	for(int i = 256; i<start_prune; i++){
		if (table[i].shift_other == P){
        	table[index].pref = i;
		table[i].shift_other = -1;
		}
		
	}
        table[table[index].pref].used = 1;

        return index;
        } else{
                return 0;
        }

}

int get_mask(int val){

	int mask = 0;
	int i = val;

	while (i){
		mask |= 1;
		if (i != 1){
			mask<<= 1;
		}
		i--;
	}
	return mask;
}

int bit_reader(int* total, unsigned int* buffer, int* maxb, int bit_change){
	unsigned int temp_buffer = 0;
	unsigned int shift = 0;
	unsigned int mask = 0;

	while(*total < *maxb){
		int k = getchar();
		//printf("I got: %d\n", k);
		//printf("total: %d\n", *total);
		*buffer <<= 8;
		*buffer |= k;
		*total += 8;
	}

	//printf("processing.../n");
	//printf("buffer is %d\n", *buffer);
	shift = *total - *maxb;
	//printf("shift is %d\n", shift);
	temp_buffer = *buffer >> shift;
	mask = get_mask(shift);
	*buffer &= mask;
	*total -= *maxb;
	
	//printf("temp buffer is: %d\n", temp_buffer);
	if (bit_change){
		if (temp_buffer ==0){
			*maxb = *maxb + 1;
			//printf("\n\n\n\nbits increased by encoder by 1, current bits: %d\n\n\n\n", *maxb);
			return bit_reader(total, buffer, maxb, 0);
		}else{
			*maxb = *maxb - temp_buffer;
			//printf("\n\n\n\nbits decreased by encoder by: %d, current bits: %d\n\n\n\n", temp_buffer, *maxb);
			return bit_reader(total, buffer, maxb, 0);
		}
	}else{
		if (temp_buffer ==0){
			//printf("\n\n\ninitiating bit change, current bits: %d\n\n\n", *maxb);
			return bit_reader(total, buffer, maxb, 1);
		}else if (temp_buffer == 1){
			//printf("\n\n\nreceived prune, current bits: %d\n\n\n", *maxb);
			return 1500000;
		}else if(temp_buffer == 3){
			//printf("bitik herif\n");
			return -1;
		}else {
			//printf("now returning: %d\n", temp_buffer);
			return temp_buffer - 3;
		}
	}

}

TableEntry* decode(int size, int stage_param) {
        char CC;
        int old;
	int new;
	int prune;
	int input;
        int i = 0;
	int start_prune = 256;

	int current_bit = 9;	
	int total = 0;
	unsigned int buffer = 0;	
	
	
	TableEntry* table = initialize_string_table(size, NULL, 0, 0);
	if (stage_param){
		prune = getchar();
		input = bit_reader(&total, &buffer, &current_bit, 0);
		old = input;
	}else{
		if(scanf("%d", &input) == 1){
			prune = input;
		}else{
			//error
			exit(0);
		}
		if(scanf("%d", &input) == 1){
			old = input-2;
		}else{
			//error
			exit(0);
		}
	}	
	char first_char = table[old].c;
	putchar(first_char);
	CC = first_char;
	int num_of_pruns = 0;
        while((input = bit_reader(&total, &buffer, &current_bit, 0)) != -1){
                new = input;
		//printf("\nnew code: %d, old code: %d\n",new, old);
		int edge = 0;
		if (new == 1500000){
			num_of_pruns++;
			//printf("\n\n\n\nNUMBER OF PRUNES IS NOW %d\n\n\n\n", num_of_pruns);
			TableEntry* temp_table = initialize_string_table(size, table, start_prune, old);
			free(table);
			table = temp_table;
			//display_temp_table(table, size, 256);
			//printf("\n\n\n");
			start_prune = get_first_available_index(table, size);
			continue;
		}	
		//printf("new inp: %d, old inp: %d\n",new, old);		
		int check_presence = check_code(table, new);
                int code_query;
		
		if(!check_presence){
			code_query = table_output_query(table, old, start_prune);
			putchar(CC);
			edge = 1;
		}else{
			code_query = table_output_query(table, new, start_prune);
		}
		
		CC = code_query; 
		int check = add_to_table(table, size, CC, old, start_prune, edge); 
		
		//display_temp_table(table, size, 256);
		//printf("\n\n\n");

		//set old
		old = new;
		}

	//printf("number of prunes decode did: %d\n", num_of_pruns);
        return table;
}


int main(){
	
	int stage_param = 0;
	char* stage = getenv("STAGE");

	if (stage == NULL) {
	    stage_param = 1;
	} else {
	    int stage_val = atoi(stage);
	    if (stage_val == 3) {
		stage_param = 1;
	    }
	}


	int size = get_size(stage_param);
	TableEntry* string_table = decode(size, stage_param);

	if (getenv("DBG")) {
		FILE* file = fopen("DBG.decode", "w");
		if (file == NULL) {
			fprintf(stderr, "Error opening file\n");
			free(string_table);
			exit(1);
		}
		display_table(string_table, size, file);
		fclose(file);
	}	


        free(string_table);	
	exit(0);
}
