#ifndef ASSERT_H_
#define ASSERT_H_

//okay, this has been needed for a while, assert

//assert and print to console via cout
#define ASSERT_P(boolean, message) \
if (!(boolean)) {\
	cout << message << endl;\
}

//assert and print to console via cout and return current function
#define ASSERT_P_R(boolean, message) \
if (!(boolean)) {\
	cout << message << endl;\
	return false;\
}

//assert and log to elk
#define ASSERT_E(boolean, message, ctx) \
if (!(boolean)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
}

//assert and log to elk and return current function
#define ASSERT_E_R(boolean, message, ctx) \
if (!(boolean)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
	return false;\
}

//assert and log to elk and return current function
#define ASSERT_E_B(boolean, message, ctx) \
if (!(boolean)) {\
	LOG_ERROR_CTX((ctx)) {\
		ADD_STR_LOG("message", message);\
	}\
	return;\
}

#endif
