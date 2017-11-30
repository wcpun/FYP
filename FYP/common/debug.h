#ifndef _DEBUG_H__
#define _DEBUG_H__

#define DEBUG(var, type) DEBUG_##type(var)
#define DEBUG_int(var) printf("In %s:%d: " #var "'s value: %d\n", __FILE__, __LINE__, var)
#define DEBUG_str(var) printf("In %s:%d: " #var "'s value: %d\n", __FILE__, __LINE__, var)
#define DEBUG_uint(var) printf("In %s:%d: " #var "'s value: %u\n", __FILE__, __LINE__, var)

void error(const char* msg);

#endif