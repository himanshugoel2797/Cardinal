%{

#include <stdio.h>
#include <string.h>

#include "ccp_parser.h"

#define CARDINAL_CCP_ARGC_MAX 8

static int CARDINAL_CCP_major_ver;
static int CARDINAL_CCP_minor_ver;
static int CARDINAL_CCP_cmd_num;
static int CARDINAL_CCP_argc;

static CARDINAL_CCP_Arg CARDINAL_CCP_args[CARDINAL_CCP_ARGC_MAX];
extern char *CARDINAL_CCP_globalInputText;


void
CardinalCCP_ParseString(char *str)
{
		CARDINAL_CCP_globalInputText = str;
        yyparse();

        CardinalCCP_Translate(CARDINAL_CCP_major_ver,
        					  CARDINAL_CCP_minor_ver,
        					  CARDINAL_CCP_cmd_num,
        					  CARDINAL_CCP_args,
        					  CARDINAL_CCP_argc);

/*
        printf("VER %u %u, CMD: %u, Argc: %u\n", CARDINAL_CCP_major_ver, CARDINAL_CCP_minor_ver, CARDINAL_CCP_cmd_num, CARDINAL_CCP_argc);

        for(int i = 0; i < CARDINAL_CCP_argc; i++){
        	if(CARDINAL_CCP_args[i].val != NULL)
        		printf("%s : %s\n", CARDINAL_CCP_args[i].name, CARDINAL_CCP_args[i].val);
        	else
        		printf("%s : %d\n", CARDINAL_CCP_args[i].name, CARDINAL_CCP_args[i].val_len);
        }
        */
} 
	

%}

%union {

  char *a;
  unsigned long long int fn;
}

%token VER
%token ASSIGN
%token CMD
%token STRING_LITERAL
%token NUMBER
%token NAME

%%

commands:
|ver command args
	;

ver:
VER NUMBER NUMBER
	{
		CARDINAL_CCP_major_ver = $2.fn;
		CARDINAL_CCP_minor_ver = $3.fn;
	}
	;

command:
CMD
	{
		char *str = $1.a;

		if(strcmp(str, "ERROR")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_ERROR;
		}else if(strcmp(str, "AUTH")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_AUTH;
		}else if(strcmp(str, "OPEN")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_OPEN;
		}else if(strcmp(str, "CLOSE")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_CLOSE;
		}else if(strcmp(str, "READ")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_READ;
		}else if(strcmp(str, "WRITE")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_WRITE;
		}else if(strcmp(str, "MKDIR")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_MKDIR;
		}else if(strcmp(str, "RMDIR")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_RMDIR;
		}else if(strcmp(str, "REMOVE")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_REMOVE;
		}else if(strcmp(str, "ADDTAG")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_ADDTAG;
		}else if(strcmp(str, "RMTAG")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_RMTAG;
		}else if(strcmp(str, "RDTAG")){
			CARDINAL_CCP_cmd_num = CARDINAL_CCP_RDTAG;
		}

		CARDINAL_CCP_argc = 0;
	}
	;

args:
|arg_single args
	;

arg_single:
NAME ASSIGN STRING_LITERAL
	{
		if(CARDINAL_CCP_argc + 1 >= CARDINAL_CCP_ARGC_MAX){
			yyerror(-1);
		}

		char *name = $1.a;
		char *val = $3.a;
		int val_len = strlen(val);

		CARDINAL_CCP_args[CARDINAL_CCP_argc].name[7] = 0;
		strncpy(CARDINAL_CCP_args[CARDINAL_CCP_argc].name, name, 7);

		CARDINAL_CCP_args[CARDINAL_CCP_argc].val = strdup(val);
		CARDINAL_CCP_args[CARDINAL_CCP_argc].val_len = val_len;

		CARDINAL_CCP_argc++;
	}
|NAME ASSIGN NUMBER
	{
		if(CARDINAL_CCP_argc + 1 >= CARDINAL_CCP_ARGC_MAX){
			yyerror(-1);
		}

		char *name = $1.a;
		unsigned long long int val = $3.fn;

		CARDINAL_CCP_args[CARDINAL_CCP_argc].name[7] = 0;
		strncpy(CARDINAL_CCP_args[CARDINAL_CCP_argc].name, name, 7);

		CARDINAL_CCP_args[CARDINAL_CCP_argc].val = NULL;
		CARDINAL_CCP_args[CARDINAL_CCP_argc].val_len = val;

		CARDINAL_CCP_argc++;
	}
	;

%%