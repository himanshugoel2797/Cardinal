#define VER 257
#define ASSIGN 258
#define CMD 259
#define STRING_LITERAL 260
#define NUMBER 261
#define NAME 262
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
extern YYSTYPE yylval;
