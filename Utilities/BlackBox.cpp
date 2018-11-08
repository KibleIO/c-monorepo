#include "BlackBox.h"

int* count_num_diff(char** diff, int* buff_size, int num_buff) {
	int* arr = new int[num_buff];
	for (int i = 0; i < num_buff; i++) {
		arr[i] = 0;
		for (int j = 0; j < buff_size[i];) {
			j += ((int*)(diff[i]))[(j + 4) / 4] + 8;
			arr[i]++;
		}
	}
	return arr;
}

int find_smallest(char** diff, int* len_diff, int num_buff) {
    int smallest = 0;

    while (len_diff[smallest] <= 0) smallest++;

    for (int i = smallest + 1; i < num_buff; i++) {
        if (len_diff[i] > 0) {
            if (((int*)diff[i])[0] < ((int*)diff[smallest])[0]) {
                smallest = i;
            } else if (((int*)diff[i])[0] == ((int*)diff[smallest])[0]) {
                smallest = (smallest > i) ? i : smallest;
            }
        }
    }
    return smallest;
}

int get_color(int index, char** back_buffers, int num_buff, int *i) {
    switch(back_buffers[*i][index + 3]) {
        case -1: //opaque
            return ((int*)back_buffers[*i])[index / 4];
        case 0: //transparent
        	*i = *i - 1;
        	return get_color(index, back_buffers, num_buff, i); //this won't error because bottom buffer will always be fully opaque
        default: //slightly transparent
        	*i = *i - 1;
            return ((int*)back_buffers[*i])[index / 4] + get_color(index, back_buffers, num_buff, i); //wrong but that doesn't matter
    }
}

int handle_section(char** diff_, int* len_diff_, int num_buff, int addr, int size_of_diff, int depth, char* final_buff, int final_buff_i, char* data, int parse_len, char** back_buffers) {
    int depth_temp, buff_size, over_step, final_buff_temp, temp_int, temp_int2, col, index_temp;
    char* temp;

    if (size_of_diff <= 0) return 0;

    depth_temp = find_smallest(diff_, len_diff_, num_buff);
    final_buff_temp = final_buff_i;
    start_here:

    //do we need that last check?
    if (depth_temp < num_buff && (addr + size_of_diff) >= ((int*)diff_[depth_temp])[0] && ((int*)diff_[depth_temp])[0] >= addr) {
        //there is overlap so lets pop that guy off!
        len_diff_[depth_temp]--;
        temp = diff_[depth_temp];
        diff_[depth_temp] += (((int*)diff_[depth_temp])[1] + 8);

        over_step = ((int*)temp)[0] - addr;
		
		for (int i = parse_len; i < parse_len + over_step; i += 4) {
        	index_temp = num_buff - 1;
    		col = get_color(((int*)data)[0] + i - 8, back_buffers, num_buff, &index_temp);
    		if (depth >= index_temp) {
                switch(data[i + 3]) {
                    case -1: //opaque
                        ((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4];
                        break;
                    case 0: //transparent
                    	((int*)final_buff)[final_buff_i / 4] = col;
                    	break;
                    default: //slightly transparent
                        ((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4] + col;
                }
    		} else {
    			((int*)final_buff)[final_buff_i / 4] = col;
    		}
    		final_buff_i += 4;
        }
        
        parse_len += over_step;

        if (over_step == size_of_diff) { //are we done?
			final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8, back_buffers);

            return final_buff_i - final_buff_temp;
        } else if (depth_temp < depth) { //checking for higher precedence
        
        	/*
        	over_step += ((int*)temp)[1];
            if (over_step < size_of_diff) {
                    len_diff_[depth]++;
                    diff_[depth] -= 8 + (size_of_diff - over_step);
                    ((int*)diff_[depth])[0] = addr + over_step;
                    ((int*)diff_[depth])[1] = size_of_diff - over_step;
            }
            final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8);
        	*/
           	temp_int = handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8, back_buffers);

            final_buff_i += temp_int;

			final_buff_i += handle_section(diff_, len_diff_, num_buff, addr + temp_int + over_step, (addr + size_of_diff) - (addr + temp_int + over_step), depth, final_buff, final_buff_i, data, parse_len + temp_int, back_buffers);

            return final_buff_i - final_buff_temp;
        } else if ((addr + size_of_diff) < ((int*)temp)[0] + ((int*)temp)[1]) { //checking for overlap
            temp_int2 = ((int*)temp)[1];

            len_diff_[depth_temp]++;
            diff_[depth_temp] -= 8 + (((int*)temp)[0] + temp_int2) - (addr + size_of_diff);

            ((int*)diff_[depth_temp])[0] = addr + size_of_diff;

            ((int*)diff_[depth_temp])[1] = (((int*)temp)[0] + temp_int2) - (addr + size_of_diff);

            temp = diff_[depth_temp];
            
            final_buff_i += handle_section(diff_, len_diff_, num_buff, addr + over_step, size_of_diff - over_step, depth, final_buff, final_buff_i, data, parse_len, back_buffers);

            return final_buff_i - final_buff_temp;
        } else {
            addr += over_step;
            size_of_diff -= over_step;

            depth_temp = find_smallest(diff_, len_diff_, num_buff);

            goto start_here;
        }
    } else {
        //write all the way through
        for (int i = parse_len; i < parse_len + size_of_diff; i += 4) {
        	index_temp = num_buff - 1;
    		col = get_color(((int*)data)[0] + i - 8, back_buffers, num_buff, &index_temp);
    		if (depth >= index_temp) {
                switch(data[i + 3]) {
                    case -1: //opaque
                        ((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4];
                        break;
                    case 0: //transparent
                    	((int*)final_buff)[final_buff_i / 4] = col;
                    	break;
                    default: //slightly transparent
                        ((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4] + col;
                }
    		} else {
    			((int*)final_buff)[final_buff_i / 4] = col;
    		}
            final_buff_i += 4;
        }

        return final_buff_i - final_buff_temp;
    }
}

/*So this basically just takes in a few difference buffers and combines them into one difference buffer
 * diff         => array of difference arrays in order of priority (first array is highest priority)
 * len_diff     => number of difference blocks in array 'diff'
 * back_buffers => array of history buffers, or what the buffers used to look like prior to the render cycle
 * num_buff     => count of the number of difference buffers
 * final_buff   => output buffer
 * ALGORITH PREC's: difference buffers must be output in pairs of 4; No transparencies. only 0xff or 0x00
 * for alpha channel. First buffer in diff must be fully 0xff
 */

int black_box(char** diff, int* len_diff, char** back_buffers, int num_buff, char* final_buff) {
    char** diff_        = new char*[num_buff];
    int*   len_diff_    = new int   [num_buff];
    char*  temp;
    int    final_buff_i = 0;
    int    smallest;
    int    final_buff_temp;

    memcpy(diff_, diff, num_buff * sizeof(char*));
    memcpy(len_diff_, len_diff, num_buff * 4);

    smallest = find_smallest(diff_, len_diff_, num_buff);
    while (smallest < num_buff) {
        len_diff_[smallest]--;
        temp = diff_[smallest];
        diff_[smallest] += (((int*)diff_[smallest])[1] + 8);

        ((int*)final_buff)[final_buff_i / 4] = ((int*)temp)[0];

        final_buff_i += 4;

        //saving address of size parameter
        final_buff_temp = final_buff_i;
        final_buff_i += 4;
        
		final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], smallest, final_buff, final_buff_i, temp, 8, back_buffers);

        ((int*)final_buff)[final_buff_temp / 4] = final_buff_i - (final_buff_temp + 4);

        smallest = find_smallest(diff_, len_diff_, num_buff);
    }
    return final_buff_i;
}

int handle_section(char** diff_, int* len_diff_, int num_buff, int addr, int size_of_diff, int depth, char* final_buff, int final_buff_i, char* data, int parse_len) {
    int depth_temp, buff_size, over_step, final_buff_temp, temp_int, temp_int2;
    char* temp;

    if (size_of_diff <= 0) return 0;

    depth_temp = find_smallest(diff_, len_diff_, num_buff);
    final_buff_temp = final_buff_i;
    start_here:

    //do we need that last check?
    if (depth_temp < num_buff && (addr + size_of_diff) >= ((int*)diff_[depth_temp])[0] && ((int*)diff_[depth_temp])[0] >= addr) {
        //there is overlap so lets pop that guy off!
        len_diff_[depth_temp]--;
        temp = diff_[depth_temp];
        diff_[depth_temp] += (((int*)diff_[depth_temp])[1] + 8);

        over_step = ((int*)temp)[0] - addr;
		
		for (int i = parse_len; i < parse_len + over_step; i += 4) {
			((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4];
			
    		final_buff_i += 4;
        }
        
        parse_len += over_step;

        if (over_step == size_of_diff) { //are we done?
			final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8);

            return final_buff_i - final_buff_temp;
        } else if (depth_temp < depth) { //checking for higher precedence
        	/*
        	temp_int = (size_of_diff - addr) - (addr - ((int*)temp)[0]);
        	
        	for (int i = 0; i < temp_int; i += 4) {
        		if (temp[i + 3] == 0) {
        			((int*)temp)[i / 4] = ((int*)data)[(parse_len + i) / 4];
                }
        	}
        	*/
        	over_step += ((int*)temp)[1];
            if (over_step < size_of_diff) {
                    len_diff_[depth]++;
                    diff_[depth] -= 8 + (size_of_diff - over_step);
                    ((int*)diff_[depth])[0] = addr + over_step;
                    ((int*)diff_[depth])[1] = size_of_diff - over_step;
            }
            final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8);
           	//temp_int = handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], depth_temp, final_buff, final_buff_i, temp, 8);

            //final_buff_i += temp_int;

			//final_buff_i += handle_section(diff_, len_diff_, num_buff, addr + temp_int + over_step, (addr + size_of_diff) - (addr + temp_int + over_step), depth, final_buff, final_buff_i, data, parse_len + temp_int);

            return final_buff_i - final_buff_temp;
        } else if ((addr + size_of_diff) < ((int*)temp)[0] + ((int*)temp)[1]) { //checking for overlap
        	/*
        	for (int i = 0; i < size_of_diff - parse_len; i += 4) {
        		if (data[parse_len + i + 3] == 0) {
        			((int*)data)[(parse_len + i) / 4] = ((int*)temp)[2 + (i / 4)];
                }
        	}
        	*/
        
        
            temp_int2 = ((int*)temp)[1];

            len_diff_[depth_temp]++;
            diff_[depth_temp] -= 8 + (((int*)temp)[0] + temp_int2) - (addr + size_of_diff);

            ((int*)diff_[depth_temp])[0] = addr + size_of_diff;

            ((int*)diff_[depth_temp])[1] = (((int*)temp)[0] + temp_int2) - (addr + size_of_diff);

            temp = diff_[depth_temp];
            
            final_buff_i += handle_section(diff_, len_diff_, num_buff, addr + over_step, size_of_diff - over_step, depth, final_buff, final_buff_i, data, parse_len);

            return final_buff_i - final_buff_temp;
        } else {
            addr += over_step;
            size_of_diff -= over_step;

            depth_temp = find_smallest(diff_, len_diff_, num_buff);

            goto start_here;
        }
    } else {
        //write all the way through
        for (int i = parse_len; i < parse_len + size_of_diff; i += 4) {
        	((int*)final_buff)[final_buff_i / 4] = ((int*)data)[i / 4];
        	
            final_buff_i += 4;
        }

        return final_buff_i - final_buff_temp;
    }
}

/*So this basically just takes in a few difference buffers and combines them into one difference buffer
 * diff         => array of difference arrays in order of priority (first array is highest priority)
 * len_diff     => number of difference blocks in array 'diff'
 * num_buff     => count of the number of difference buffers
 * final_buff   => output buffer
 * ALGORITH PREC's: difference buffers must be output in pairs of 4; No transparencies. only 0xff or 0x00
 * for alpha channel. First buffer in diff must be fully 0xff
 */

int black_box(char** diff, int* len_diff, int num_buff, char* final_buff) {
    char** diff_        = new char*[num_buff];
    int*   len_diff_    = new int   [num_buff];
    char*  temp;
    int    final_buff_i = 0;
    int    smallest;
    int    final_buff_temp;

    memcpy(diff_, diff, num_buff * sizeof(char*));
    memcpy(len_diff_, len_diff, num_buff * 4);

    smallest = find_smallest(diff_, len_diff_, num_buff);
    while (smallest < num_buff) {
        len_diff_[smallest]--;
        temp = diff_[smallest];
        diff_[smallest] += (((int*)diff_[smallest])[1] + 8);

        ((int*)final_buff)[final_buff_i / 4] = ((int*)temp)[0];

        final_buff_i += 4;

        //saving address of size parameter
        final_buff_temp = final_buff_i;
        final_buff_i += 4;
        
		final_buff_i += handle_section(diff_, len_diff_, num_buff, ((int*)temp)[0], ((int*)temp)[1], smallest, final_buff, final_buff_i, temp, 8);

        ((int*)final_buff)[final_buff_temp / 4] = final_buff_i - (final_buff_temp + 4);

        smallest = find_smallest(diff_, len_diff_, num_buff);
    }
    return final_buff_i;
}