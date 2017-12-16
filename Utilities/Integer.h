#ifndef INTEGER_H_
#define INTEGER_H_

union Integer {
	int  data;
	char bytes[4];
};

#endif /* INTEGER_H_ */