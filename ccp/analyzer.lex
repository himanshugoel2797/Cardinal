%{
   #include "y.tab.h"

#undef YY_INPUT
#define YY_INPUT(a, b, c) inCARD(a, &b, c)

#undef ECHO
#define ECHO

char *CARDINAL_CCP_globalInputText;
static int globalReadOffset = 0;

int inCARD(char *buffer, int *numBytesRead, int maxBytesToRead){
   
    int numBytesToRead = maxBytesToRead;
    int bytesRemaining = strlen(CARDINAL_CCP_globalInputText)-globalReadOffset;
    int i;
    if ( numBytesToRead > bytesRemaining ) { numBytesToRead = bytesRemaining; }
    for ( i = 0; i < numBytesToRead; i++ ) {
        buffer[i] = CARDINAL_CCP_globalInputText[globalReadOffset+i];
    }
    *numBytesRead = numBytesToRead;
    globalReadOffset += numBytesToRead;
    return 0;
}

%}
%%
%option never-interactive

":"     { return ASSIGN; }

"VER"  { yylval.a = yytext; return VER; }

"ERROR"|"AUTH"|"OPEN"|"CLOSE"|"READ"|"WRITE"|"MKDIR"|"RMDIR"|"REMOVE"|"ADDTAG"|"RMTAG"|"RDTAG"|"RDDIR"|"RESP" { yylval.a = strdup(yytext); return CMD; }

\"[^"\n]*["\n] { yylval.a = strdup(yytext + 1); yylval.a[yyleng - 2] = 0; return STRING_LITERAL; }

[0-9]+   { yylval.fn = atoi(yytext); return NUMBER; }

[A-Za-z]+   { yylval.a = strdup(yytext); return NAME; }

%%

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
        return -1;
} 