/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20150711

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "parser.yacc"

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
CardinalCCP_ParseString(char *str) {
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


#line 46 "parser.yacc"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {

    char *a;
    unsigned long long int fn;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 77 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define VER 257
#define ASSIGN 258
#define CMD 259
#define STRING_LITERAL 260
#define NUMBER 261
#define NAME 262
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
                                                         0,    0,    1,    2,    3,    3,    4,    4,
                             };
static const YYINT yylen[] = {                            2,
                                                          0,    3,    3,    1,    0,    2,    3,    3,
                             };
static const YYINT yydefred[] = {                         0,
                                                          0,    0,    0,    0,    4,    0,    3,    0,    2,    0,
                                                          0,    6,    7,    8,
                                };
static const YYINT yydgoto[] = {                          2,
                                                          3,    6,    9,   10,
                               };
static const YYINT yysindex[] = {                      -255,
                                                       -258,    0, -254, -257,    0, -256,    0, -251,    0, -256,
                                                       -260,    0,    0,    0,
                                };
static const YYINT yyrindex[] = {                         8,
                                                          0,    0,    0,    0,    0,    9,    0,    0,    0,    9,
                                                          0,    0,    0,    0,
                                };
static const YYINT yygindex[] = {                         0,
                                                          0,    0,    1,    0,
                                };
#define YYTABLESIZE 11
static const YYINT yytable[] = {                         13,
                                                         14,    1,    4,    7,    5,    8,   11,    1,    5,    0,
                                                         12,
                               };
static const YYINT yycheck[] = {                        260,
                                                        261,  257,  261,  261,  259,  262,  258,    0,    0,   -1,
                                                        10,
                               };
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 262
#define YYUNDFTOKEN 269
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

    "end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"VER","ASSIGN","CMD",
    "STRING_LITERAL","NUMBER","NAME",0,0,0,0,0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
    "$accept : commands",
    "commands :",
    "commands : ver command args",
    "ver : VER NUMBER NUMBER",
    "command : CMD",
    "args :",
    "args : arg_single args",
    "arg_single : NAME ASSIGN STRING_LITERAL",
    "arg_single : NAME ASSIGN NUMBER",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data) {
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data) {
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL() {
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0) {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0) {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug) {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                   YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar) {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                   YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) {
            goto yyoverflow;
        }
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar) {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3) {
        yyerrflag = 3;
        for (;;) {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE) {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            } else {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                           YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    } else {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug) {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                   YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
               YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn) {
    case 3:
#line 67 "parser.yacc"
        {
            CARDINAL_CCP_major_ver = yystack.l_mark[-1].fn;
            CARDINAL_CCP_minor_ver = yystack.l_mark[0].fn;
        }
        break;
    case 4:
#line 75 "parser.yacc"
        {
            char *str = yystack.l_mark[0].a;

            if(strcmp(str, "ERROR")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_ERROR;
            } else if(strcmp(str, "AUTH")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_AUTH;
            } else if(strcmp(str, "OPEN")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_OPEN;
            } else if(strcmp(str, "CLOSE")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_CLOSE;
            } else if(strcmp(str, "READ")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_READ;
            } else if(strcmp(str, "WRITE")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_WRITE;
            } else if(strcmp(str, "MKDIR")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_MKDIR;
            } else if(strcmp(str, "RMDIR")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_RMDIR;
            } else if(strcmp(str, "REMOVE")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_REMOVE;
            } else if(strcmp(str, "ADDTAG")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_ADDTAG;
            } else if(strcmp(str, "RMTAG")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_RMTAG;
            } else if(strcmp(str, "RDTAG")) {
                CARDINAL_CCP_cmd_num = CARDINAL_CCP_RDTAG;
            }

            CARDINAL_CCP_argc = 0;
        }
        break;
    case 7:
#line 114 "parser.yacc"
        {
            if(CARDINAL_CCP_argc + 1 >= CARDINAL_CCP_ARGC_MAX) {
                yyerror(-1);
            }

            char *name = yystack.l_mark[-2].a;
            char *val = yystack.l_mark[0].a;
            int val_len = strlen(val);

            CARDINAL_CCP_args[CARDINAL_CCP_argc].name[7] = 0;
            strncpy(CARDINAL_CCP_args[CARDINAL_CCP_argc].name, name, 7);

            CARDINAL_CCP_args[CARDINAL_CCP_argc].val = strdup(val);
            CARDINAL_CCP_args[CARDINAL_CCP_argc].val_len = val_len;

            CARDINAL_CCP_argc++;
        }
        break;
    case 8:
#line 132 "parser.yacc"
        {
            if(CARDINAL_CCP_argc + 1 >= CARDINAL_CCP_ARGC_MAX) {
                yyerror(-1);
            }

            char *name = yystack.l_mark[-2].a;
            unsigned long long int val = yystack.l_mark[0].fn;

            CARDINAL_CCP_args[CARDINAL_CCP_argc].name[7] = 0;
            strncpy(CARDINAL_CCP_args[CARDINAL_CCP_argc].name, name, 7);

            CARDINAL_CCP_args[CARDINAL_CCP_argc].val = NULL;
            CARDINAL_CCP_args[CARDINAL_CCP_argc].val_len = val;

            CARDINAL_CCP_argc++;
        }
        break;
#line 498 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0) {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0) {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug) {
                yys = yyname[YYTRANSLATE(yychar)];
                printf("%sdebug: state %d, reading %d (%s)\n",
                       YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) {
        goto yyoverflow;
    }
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
