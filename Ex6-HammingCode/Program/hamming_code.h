#ifndef hamming_code_h
#define hamming_code_h

#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>


char* pass_noise(char *encoded_msg, int msg_size, int *err_posn){
	char* noisy_msg = (char*)malloc(sizeof(char)*msg_size);
	memcpy(noisy_msg, encoded_msg, msg_size);
	srand(time(0));
	int posn = rand()%msg_size;
	// Add error, change bit
	if(*(noisy_msg+posn)=='1'){
		*(noisy_msg+posn) = '0';
	}
	else{
		*(noisy_msg+posn) = '1';
	}
	// Return reversed error posn and the noisy message
	*err_posn = msg_size - posn;
	return noisy_msg;
}

char* reverse_string(char *orig_string){
	int size = strlen(orig_string);
	char *rev_string = (char*)malloc(sizeof(char)*size);
	for(int i=0;i<size;i++){
		*(rev_string+i) = *(orig_string+size-i-1);
	}
	return rev_string;
}

int raise_to_power(int base, int exp){
	// Only for positive exp
	int result = 1;
	for(int i=0;i<exp;i++){
		result *= base;
	}
	return result;
}

char* decimal_to_binary(int num){
	// Result is returned in reverse order
	// Eg: 6 -> 011
	int binary_size = ((int)floor(log2(num))) + 1;
	char* binary = (char*)malloc(sizeof(char)*binary_size);
	for(int i=0;i<binary_size;i++){
		// Converting to ASCII character
		*(binary+i) = 48 + (num%2); 
		num /= 2;
	}
	return binary;
}

int binary_to_decimal(char *binary){
	binary = reverse_string(binary);
	int decimal_num = 0;
	int posn = 0;
	char *parser = binary;
	while(*parser!='\0'){
		decimal_num += raise_to_power(2, posn)*((int)((*parser)-48));
		posn++;
		parser++;
	}
	return decimal_num;
}

int find_r_value_from_rawmsg(int msg_size){
	// Deduce r from 2^r >= m+r+1
	for(int r=0;r<msg_size;r++){
		if(raise_to_power(2, r) >= msg_size+r+1){
			return r;
		}
	}
	return -1;  // Not found. Syntactic statement - never reached
}

int find_r_value_from_hammingmsg(int msg_size){
	// Deduce r from 2^r >= m+r+1
	int m;
	for(int r=0;r<msg_size;r++){
		m = msg_size - r;
		if(raise_to_power(2, r) >= m+r+1){
			return r;
		}
	}
	return -1;  // Not found. Syntactic statement - never reached
}

short find_even_parity(char *rev_merged_msg, int msg_size, int rbit_num, short exclude_rbit, short verbose){
	// Find's the even parity for posns relevant to rbit_num
	// msg_size is the entire size of merged message
	// assessed_bits returns the set of bits that were examined
	int start_at = (exclude_rbit ? raise_to_power(2, rbit_num) : raise_to_power(2, rbit_num)-1);
	// Exclude the r-bit itself if exclude is set to 1. Otherwise, start from rbit
	// Eg: for r2 (rbit_num=1), exclude upto position-2 (index-1 in the reversed msg)
	int count_ones = 0;
	for(int i=start_at;i<msg_size;i++){
		// i is the index. posn=i+1
		if(decimal_to_binary(i+1)[rbit_num]=='1'){
			if(*(rev_merged_msg+i)=='1'){
				count_ones++;
			}
			if(verbose){
				printf(" %d,", (i+1));
			}
		}
	}
	// Return 1 if odd number of 1s. 0, otherwise
	return count_ones%2;
}

char* position_redundant_bits(char* rev_raw_msg, int msg_size, int r_val){
	// raw_msg is given in reverse
	char *rev_merged_msg = malloc(sizeof(char)*(msg_size+r_val));
	// curr_r is the r in 2^r=posn (posn in reversed msg)
	for(int i=0, curr_r=0; i<(msg_size+r_val); i++){
		if(raise_to_power(2, curr_r)==(i+1)){  
			// This is a redundant bit
			*(rev_merged_msg+i) = '0';
			curr_r++;
		}
		else{
			*(rev_merged_msg+i) = *(rev_raw_msg+i-curr_r);
		}
	}
	return rev_merged_msg;
}

char* remove_redundant_bits(char* rev_merged_msg, int msg_size, int r_val, short verbose){
	// merged_msg is given in reverse
	char *rev_raw_msg = (char*)malloc(sizeof(char)*(msg_size-r_val));
	// curr_r is the r in 2^r=posn (posn in reversed msg)
	for(int i=0, curr_r=0; i<msg_size; i++){
		if(raise_to_power(2, curr_r)==(i+1)){  
			// This is a redundant bit
			// Exclude
			curr_r++;
		}
		else{
			*(rev_raw_msg+i-curr_r) = *(rev_merged_msg+i);
		}
	}
	return rev_raw_msg;
}


char* encode_hamming_message(char* raw_msg, int *enc_msg_size, short verbose){
	// r_value is used to return the r_value
	// msg_size is the size of raw message
	int msg_size = strlen(raw_msg);
	int r_val = find_r_value_from_rawmsg(msg_size);
	if(verbose){
		printf("         Size of original message (m) : %d", msg_size);
		printf("\nMin. r computed using `2^r >= (m+r+1)`: %d", r_val);
	}
	// Reverse the raw message for convenience
	char *rev_raw_msg = reverse_string(raw_msg);
	// Insert the redundant bit
	char *rev_merged_msg = position_redundant_bits(rev_raw_msg, msg_size, r_val);
	// Set parity values to the redundant bits
	char *redundant_bits = (char*)malloc(sizeof(char)*r_val);
	int parity_val;
	for(int r=0;r<r_val;r++){
		if(verbose){
			printf("\n\nFinding Parity Bit at R%d\nComputed at positions: ", (r+1));
		}
		parity_val = find_even_parity(rev_merged_msg, msg_size+r_val, r, 1, 1);
		*(rev_merged_msg+(raise_to_power(2, r)-1)) = 48 + parity_val;
		*(redundant_bits+r_val-r-1) = 48 + parity_val;
		if(verbose){
			printf("\nParity Bit Value: %d", parity_val);
		}
	}
	if(verbose){
		printf("\n\n Redundant bits: %s", redundant_bits);
	}
	// Return the full message size
	*enc_msg_size = (r_val + msg_size); 
	// Reverse-back the merged string
	return reverse_string(rev_merged_msg);
}

char* decode_hamming_message(char *merged_msg, short verbose){
	// msg_size is the size of merged message
	int msg_size = strlen(merged_msg);
	int r_val = find_r_value_from_hammingmsg(msg_size);
	if(verbose){
		printf("        Size of encoded message (m+r) : %d", msg_size);
		printf("\nMin. r computed using `2^r >= (m+r+1)`: %d", r_val);
	}
	// Reverse the hamming message
	char *rev_merged_msg = reverse_string(merged_msg);
	// Compute parities
	char *error_posn_binary = (char*)malloc(sizeof(char)*r_val);
	int parity_val;
	for(int r=0;r<r_val;r++){
		if(verbose){
			printf("\n\nFinding Parity Bit at R%d\nComputed at positions: ", (r+1));
		}
		parity_val = find_even_parity(rev_merged_msg, msg_size, r, 0, 1);
		*(error_posn_binary+r_val-r-1) = 48 + parity_val;
		if(verbose){
			printf("\nParity Bit Value: %d", parity_val);
		}
	}
	int correction_posn = binary_to_decimal(error_posn_binary);
	if(correction_posn!=0){
		if((*(rev_merged_msg+correction_posn-1))=='0'){
			*(rev_merged_msg+correction_posn-1) = '1';
		}
		else{
			*(rev_merged_msg+correction_posn-1) = '0';
		}
	}
	if(verbose){
		printf("\n\n Binary form of correction posn: %s", error_posn_binary);
		printf("\nDecimal form of correction posn: %d", correction_posn);
		printf("\nCorrected hamming-encoded message: %s", reverse_string(rev_merged_msg));
	}
	// Return the redundant bits
	return reverse_string(remove_redundant_bits(rev_merged_msg, msg_size, r_val, verbose));
}

#endif
