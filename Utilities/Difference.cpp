#include "Difference.h"

unsigned int Encode_Difference_File(unsigned char *difference, unsigned char *old_buffer, unsigned char *new_buffer, unsigned int length) {
        unsigned int hi;
        unsigned int di = 0;
        unsigned int bi = 0;

        while(bi < length){
                if(*((unsigned long*)&old_buffer[bi])
                != *((unsigned long*)&new_buffer[bi])){
                        hi = di;
                        if(*((unsigned int*)&old_buffer[bi]) != *((unsigned int*)&new_buffer[bi])){
                                if(*((unsigned short*)&old_buffer[bi]) != *((unsigned short*)&new_buffer[bi])){
                                        if(*((unsigned char*)&old_buffer[bi]) == *((unsigned char*)&new_buffer[bi])){
                                                 bi += sizeof(unsigned char);
                                        }
                                } else {
										bi += sizeof(unsigned short);
										if(*((unsigned char*)&old_buffer[bi]) == *((unsigned char*)&new_buffer[bi])){
												bi += sizeof(unsigned char);
										}
                                }
                        } else {
                                bi += sizeof(unsigned int);
                                if(*((unsigned short*)&old_buffer[bi]) != *((unsigned short*)&new_buffer[bi])){
                                        if(*((unsigned char*)&old_buffer[bi]) == *((unsigned char*)&new_buffer[bi])){
                                                 bi += sizeof(unsigned char);
                                        }
                                } else {
                                        bi += sizeof(unsigned short);
                                        if(*((unsigned char*)&old_buffer[bi]) == *((unsigned char*)&new_buffer[bi])){
                                                bi += sizeof(unsigned char);
                                        }
                                }
                        }
                        *((unsigned int*)&difference[hi]) = bi;
                        di += sizeof(unsigned long);
                        do {
                                *((unsigned long*)&difference[di]) = *((unsigned long*)&new_buffer[bi]);
                                di += sizeof(unsigned long);
                                bi += sizeof(unsigned long);
                        } while(*((unsigned long*)&old_buffer[bi]) != *((unsigned long*)&new_buffer[bi]));
                        bi -= sizeof(unsigned int);
                        if(*((unsigned int*)&old_buffer[bi])
                        != *((unsigned int*)&new_buffer[bi])){
                                bi += sizeof(unsigned short);
                                if(*((unsigned short*)&old_buffer[bi])
                                != *((unsigned short*)&new_buffer[bi])){
                                        bi += sizeof(unsigned char);
                                        if(*((unsigned char*)&old_buffer[bi])
                                        != *((unsigned char*)&new_buffer[bi])){
                                                bi += sizeof(unsigned char);
                                        }
                                } else {
                                        bi -= sizeof(unsigned char);
                                        if(*((unsigned char*)&old_buffer[bi])
                                        != *((unsigned char*)&new_buffer[bi])){
                                                        bi += sizeof(unsigned char);
                                                }
                                        }
                        } else {
                                bi -= sizeof(unsigned short);
                                if(*((unsigned short*)&old_buffer[bi])
                                != *((unsigned short*)&new_buffer[bi])){
                                        bi += sizeof(unsigned char);
                                        if(*((unsigned char*)&old_buffer[bi])
                                        != *((unsigned char*)&new_buffer[bi])){
                                                bi += sizeof(unsigned char);
                                        }
                                } else {
                                        bi -= sizeof(unsigned char);
                                        if(*((unsigned char*)&old_buffer[bi])
                                        != *((unsigned char*)&new_buffer[bi])){
                                                bi += sizeof(unsigned char);
                                        }
                                }
                        }
                        di = *((unsigned int*)&difference[hi + sizeof(unsigned int)]) = bi - *((unsigned int*)&difference[hi]);
                        di += hi + sizeof(unsigned long);
                        //end block
                }
                bi += sizeof(unsigned long);
        }
        return di;
}

unsigned int Encode_Difference_File_spec(unsigned char *difference, unsigned char *old_buffer, unsigned char *new_buffer, unsigned int length, int max_diff) {
	
        unsigned int hi;
        unsigned int di = 0;
        unsigned int bi = 0;
        
        while(bi < length) {
                if(*((unsigned long*)&old_buffer[bi])
                != *((unsigned long*)&new_buffer[bi])){
                        hi = di;
                        //if(*((unsigned int*)&old_buffer[bi]) == *((unsigned int*)&new_buffer[bi])){
                        //        bi += sizeof(unsigned int);
                        //}
                        *((unsigned int*)&difference[hi]) = bi;
                        do {
                                bi += sizeof(unsigned long);
                                di += sizeof(unsigned long);
                                if (di > max_diff) {
                                	di = *((unsigned int*)&difference[hi + sizeof(unsigned int)]) = length - *((unsigned int*)&difference[hi]);
			                    	copy((unsigned int*)&new_buffer[length - di], (unsigned int*)&new_buffer[length], ((unsigned int*)&difference[hi + sizeof(unsigned long)]));
			                    	return di + hi + sizeof(unsigned long);
                                }
                        } while(*((unsigned long*)&old_buffer[bi]) != *((unsigned long*)&new_buffer[bi]));
						//if(*((unsigned int*)&old_buffer[bi]) != *((unsigned int*)&new_buffer[bi])){
                        //        bi += sizeof(unsigned int);
                        //}
                    	di = *((unsigned int*)&difference[hi + sizeof(unsigned int)]) = bi - *((unsigned int*)&difference[hi]);
                    	copy((unsigned int*)&new_buffer[bi - di], (unsigned int*)&new_buffer[bi], ((unsigned int*)&difference[hi + sizeof(unsigned long)]));
                    	di += hi + sizeof(unsigned long);
                }
                bi += sizeof(unsigned long);
        }
        
        return di;
}

unsigned int Encode_Difference_File_spec(unsigned char *difference, unsigned char *old_buffer, unsigned char *new_buffer, unsigned int length) {
        unsigned int hi;
        unsigned int di = 0;
        unsigned int bi = 0;

        while(bi < length){
                if(*((unsigned long*)&old_buffer[bi])
                != *((unsigned long*)&new_buffer[bi])){
                        hi = di;
                        if(*((unsigned int*)&old_buffer[bi]) == *((unsigned int*)&new_buffer[bi])){
                                bi += sizeof(unsigned int);
                        }
                        *((unsigned int*)&difference[hi]) = bi;
                        di += sizeof(unsigned long);
                        do {
                                *((unsigned long*)&difference[di]) = *((unsigned long*)&new_buffer[bi]);
                                di += sizeof(unsigned long);
                                bi += sizeof(unsigned long);
                        } while(*((unsigned long*)&old_buffer[bi]) != *((unsigned long*)&new_buffer[bi]));
						bi -= sizeof(unsigned int);
						if (*((unsigned int*)&old_buffer[bi]) != *((unsigned int*)&new_buffer[bi])){
                                bi += sizeof(unsigned int);
                        }
                        di = *((unsigned int*)&difference[hi + sizeof(unsigned int)]) = bi - *((unsigned int*)&difference[hi]);
                        di += hi + sizeof(unsigned long);
                        //end block
                }
                bi += sizeof(unsigned long);
        }
        
        return di;
}

void Decode_Difference_File(unsigned char *difference_file, unsigned int difference_file_length, unsigned char *buffer_to_update) {
    unsigned int di = 0;
    unsigned int bi;
    unsigned int bl;
    while (di < difference_file_length) {
        bi  = *((unsigned int*)&difference_file[di]);
        di += sizeof(unsigned int);
        bl  = *((unsigned int*)&difference_file[di]);
        di += sizeof(unsigned int);
        std::copy(difference_file + di, difference_file + di + bl, buffer_to_update + bi);
        di += bl;
    }
}

void Decode_Difference_File_spec(unsigned char *difference_file, unsigned int difference_file_length, unsigned char *buffer_to_update, unsigned char **mains, unsigned int num_mains, unsigned int num_curr) {
    unsigned int di = 0;
    unsigned int bi;
    unsigned int bl;
	unsigned int iterator;
	unsigned int index;
    unsigned char *dummy;
    while (di < difference_file_length) {
        bi  = *((unsigned int*)&difference_file[di]);
        di += sizeof(unsigned int);
        bl  = *((unsigned int*)&difference_file[di]);
        di += sizeof(unsigned int);
        dummy = difference_file + di;
        for (int i = 0; i < bl; i += 4) {
			if (dummy[i + 3] == 0) {
				iterator = 1;
				index = (num_curr == 0) ? 1 : 0;
				while (iterator < num_mains) {
					if (mains[index][bi + i + 3] <= mains[iterator][bi + i + 3] && iterator != num_curr) index = iterator;
					iterator++;
				}
				*((unsigned int*)&buffer_to_update[bi + i]) = *((unsigned int*)&mains[index][bi + i]);
            } else if (buffer_to_update[bi + i + 3] <= dummy[i + 3]) {
                *((unsigned int*)&buffer_to_update[bi + i]) = *((unsigned int*)&dummy[i]);
            }
        }
        di += bl;
    }
}
