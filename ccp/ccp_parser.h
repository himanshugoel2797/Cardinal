#ifndef _CARDINAL_CCP_PARSER_H_
#define _CARDINAL_CCP_PARSER_H_

typedef struct {
	char name[8];
	unsigned long long int val_len;
	char *val;
} CARDINAL_CCP_Arg;

typedef enum {
	CARDINAL_CCP_OPEN,
	CARDINAL_CCP_AUTH,
	CARDINAL_CCP_ERROR,
	CARDINAL_CCP_CLOSE,
	CARDINAL_CCP_READ,
	CARDINAL_CCP_WRITE,
	CARDINAL_CCP_MKDIR,
	CARDINAL_CCP_RMDIR,
	CARDINAL_CCP_REMOVE,
	CARDINAL_CCP_ADDTAG,
	CARDINAL_CCP_RMTAG,
	CARDINAL_CCP_RDTAG, 
	CARDINAL_CCP_RDDIR
} CARDINAL_CCP_COMMAND;

typedef struct {
    int major;
    int minor;
    int (*open)(const char * /*file*/, int /*flags*/, uint64_t /*unused*/, UID /*src pid*/, void* /*auth data*/, int /*auth data len*/, uint64_t* /*fd return*/);
} CCP_VersionHandlers;

void 
CardinalCCP_SetVersionHandlers(CCP_VersionHandlers *handlers, 
							   int cnt);

void 
CardinalCCP_Translate(int major, 
					  int minor,
					 CARDINAL_CCP_COMMAND cmd,
					 CARDINAL_CCP_Arg *args,
					 int argc);

void 
CardinalCCP_ParseString(char *str);

#endif