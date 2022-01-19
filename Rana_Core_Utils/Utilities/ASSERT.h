#ifndef ASSERT_H_
#define ASSERT_H_

//okay, this has been needed for a while, assert

//assert and print to console via cout
#define ASSERT_P(bool, message) \
if (!(bool)) {\
	cout << message << endl;\
}

//assert and print to console via cout and return current function
#define ASSERT_P_R(bool, message) \
if (!(bool)) {\
	cout << message << endl;\
	return false;\
}

//assert and log to elk
#define ASSERT_E(bool, message, ctx) \
if (!(bool)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
}

//assert and log to elk and return current function
#define ASSERT_E_R(bool, message, ctx) \
if (!(bool)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
	return false;\
}

//assert and log to elk and return current function
#define ASSERT_E_B(bool, message, ctx) \
if (!(bool)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
	return;\
}

#endif
