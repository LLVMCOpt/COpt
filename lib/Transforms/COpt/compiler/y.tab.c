/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20140715

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "coptparser.y"
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#include "coptast.h"
	#include "coptsemantic.h"
	
	extern int lineno;	/* track line number*/
	int check = 0;		/* check for semantic error*/
	int optNo = 0;		/* indicates where to generate the file*/
	
	void yyerror(char *);
	int yylex();
	
	nodeType *opr(char oper[], int nops, ...);		/* prototypes*/
	nodeType *con(int val1, char val2[], int typ);
	void freeNode(nodeType *p);
#line 21 "coptparser.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union
{
	int iValue;                 /* integer value*/
	char keyword[25];			/* string keywords*/
	char op[3];					/* operators*/
	struct nodeTypeTag *nPtr;	/* node pointer*/
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 54 "y.tab.c"

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

#define NUM 257
#define OPT 258
#define ITER 259
#define REP 260
#define PRECONDITION 261
#define DAG 262
#define AVEX 263
#define RDEFS 264
#define RPO 265
#define OPT_FI 266
#define OPT_CF 267
#define OPT_DCE 268
#define OPT_LCSE 269
#define OPT_IVS 270
#define OPT_GCSE 271
#define OPT_LICM 272
#define OPT_GVN 273
#define OPT_LU 274
#define OPT_TRE 275
#define MODULE 276
#define CFG 277
#define FUNCTION 278
#define BASICBLOCK 279
#define LOOP 280
#define INSTRUCTION 281
#define EXPRESSION 282
#define COMMON_FN1 283
#define COMMON_FN2 284
#define COMMON_FN3 285
#define COMMON_FN4 286
#define COMMON_FN5 287
#define FI_FN1 288
#define FI_FN2 289
#define COMMON_THR1 290
#define FI_THR1 291
#define CF_FN1 292
#define CF_FN2 293
#define DCE_FN1 294
#define LCSE_FN1 295
#define CSE_THR1 296
#define IVS_FN1 297
#define GCSE_FN1 298
#define LICM_KW1 299
#define LICM_KW2 300
#define LICM_FN1 301
#define LICM_FN2 302
#define LICM_FN3 303
#define GVN_FN1 304
#define GVN_FN2 305
#define LU_FN1 306
#define TRE_FN1 307
#define TRE_FN2 308
#define LTE 309
#define GTE 310
#define LT 311
#define GT 312
#define EQ 313
#define NEQ 314
#define MINUS 315
#define PLUS 316
#define END 317
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,   47,   47,    1,    2,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    4,    4,    4,   16,   15,   15,
   19,   18,   18,   17,   17,    5,    5,    5,   21,   22,
   22,   20,    6,    6,    6,   24,   23,    7,    7,   25,
   25,   26,   27,   27,   28,   28,    8,    8,    8,   29,
   29,   30,   31,   32,   32,   33,   33,    9,    9,   34,
   10,   10,   10,   35,   35,   35,   35,   36,   37,   38,
   39,   11,   11,   11,   40,   41,   41,   41,   12,   12,
   42,   43,   43,   13,   13,   44,   45,   45,   46,   46,
   46,   46,   46,   46,    3,   14,   14,   14,   14,   14,
   14,
};
static const YYINT yylen[] = {                            2,
    1,    2,    0,    4,    4,    4,    4,    4,    4,    4,
    4,    4,    4,    4,    2,    2,    0,    6,    4,    5,
    2,    3,    0,    3,    3,    2,    2,    0,    6,    7,
    0,    6,    2,    2,    0,    4,    6,    2,    0,    5,
    7,    2,    3,    0,    3,    1,    2,    2,    0,    5,
    7,    6,    2,    3,    0,    3,    1,    2,    0,    5,
    2,    2,    0,    7,    7,    5,    7,    6,    3,    3,
    3,    2,    2,    0,    8,    5,    5,    4,    2,    0,
    6,    4,    0,    2,    0,    6,    7,    0,    1,    1,
    1,    1,    1,    1,    5,    1,    1,    1,    1,    1,
    1,
};
static const YYINT yydefred[] = {                         0,
    0,    0,    0,    1,    0,    2,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    4,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    5,    0,    0,    0,    0,    6,    0,
    0,    0,    0,    7,    0,    0,    0,    0,    8,    0,
    0,    9,    0,    0,    0,    0,   10,    0,    0,    0,
    0,    0,    0,    0,   11,    0,    0,    0,    0,    0,
    0,   12,    0,    0,    0,   13,    0,    0,   14,    0,
   96,   97,   98,   99,  100,  101,    0,    0,    0,    0,
   15,   16,    0,    0,   26,   27,    0,    0,   33,   34,
    0,    0,   38,    0,   58,    0,    0,    0,   47,   48,
    0,    0,    0,    0,    0,   61,   62,    0,    0,    0,
    0,   73,   72,    0,   79,    0,   84,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   95,    0,    0,   21,
    0,   19,    0,    0,    0,    0,   36,    0,    0,   42,
    0,    0,    0,    0,    0,   53,    0,    0,    0,    0,
    0,    0,    0,    0,   78,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   20,    0,    0,    0,    0,
    0,   46,    0,    0,   40,   60,    0,    0,   57,    0,
    0,   50,    0,    0,    0,    0,    0,    0,   71,   66,
    0,   76,   77,    0,    0,    0,    0,   18,   89,   90,
   91,   92,   93,   94,    0,    0,   22,   32,    0,   29,
   37,    0,    0,   43,   52,    0,    0,   54,   68,    0,
    0,    0,    0,    0,    0,    0,   81,    0,   86,   24,
   25,    0,   41,   45,   51,   56,   67,   69,   64,   70,
   65,    0,   82,    0,    0,   75,    0,    0,    0,   30,
   87,
};
static const YYINT yydgoto[] = {                          2,
    3,   17,   30,   44,   49,   54,   59,   67,   62,   75,
   82,   86,   89,   97,   45,   46,  205,  170,  141,   50,
   51,  175,   55,   56,   60,  149,  180,  214,   68,   69,
  154,  186,  221,   63,   76,   77,  226,  228,  160,   83,
   84,   87,  199,   90,  201,  245,    4,
};
static const YYINT yysindex[] = {                      -233,
  -27,    0, -233,    0, -172,    0,  -88,  -83,  -76,  -74,
  -73,  -71,  -70,  -67,  -66,  -61,  -62, -213, -213, -213,
 -213, -213, -213, -213, -213, -213, -213,    0,   26, -217,
 -255, -243, -234, -218, -244, -235, -246, -225, -224, -203,
   24,   44,   45,    0, -217, -217,   29,   48,    0, -255,
 -255,   31,   50,    0, -243, -243,   61,   80,    0, -234,
   81,    0, -218,   64,   65,   84,    0, -244, -244,   67,
   69,   70,   71,   91,    0, -235, -235,   76,   95,   96,
   97,    0, -246, -246,   98,    0, -225,   99,    0, -224,
    0,    0,    0,    0,    0,    0,  100, -203, -203,  101,
    0,    0, -203, -203,    0,    0, -117,  102,    0,    0,
 -140, -203,    0, -203,    0, -203, -139, -203,    0,    0,
 -203, -141, -154, -153, -203,    0,    0, -203,  107, -203,
 -203,    0,    0, -203,    0, -203,    0,   92,   27,  108,
  112,  103,   32,  110,   33,  104,  117,  114,  118,  119,
   38,  124,  121,  125,   46,  127,  128,  130,  129,  131,
  132,  115,  134,  136,  135,  137,    0, -217, -247,    0,
  123,    0, -255,  -90,  139, -116,    0, -203, -236,    0,
  126,  133, -244, -203, -229,    0,  138, -235, -203, -203,
 -203, -114,  140,  -82,    0,  141,  142, -102,  149,  -68,
  150,   73, -196, -196,  108,    0,   77,  145,  146,   79,
  153,    0, -196,  114,    0,    0,   82,  154,    0, -196,
  121,    0,   83,  155,  162,  168,  166,  170,    0,    0,
   89,    0,    0, -196,  156,  158,  159,    0,    0,    0,
    0,    0,    0,    0,  -44,  -43,    0,    0,  -72,    0,
    0,  160,  -40,    0,    0,  161,  -35,    0,    0,  164,
  -39,  165,  -69,  167, -246,  -30,    0,  -80,    0,    0,
    0,  189,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  106,    0,  192, -203,    0, -203,  193,  194,    0,
    0,
};
static const YYINT yyrindex[] = {                       233,
    0,    0,  233,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  111,
  -62,  113,  116,  120,  122,  143,  144,  147,  148,    0,
    0,    0,    0,    0,  111,  111,    0,    0,    0,  -62,
  -62,    0,    0,    0,  113,  113,    0,    0,    0,  116,
    0,    0,  120,    0,    0,    0,    0,  122,  122,    0,
    0,    0,    0,    0,    0,  143,  143,    0,    0,    0,
    0,    0,  144,  144,    0,    0,  147,    0,    0,  148,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  196,
    0,    0,    0,  198,    0,    0,    0,  199,    0,    0,
    0,    0,  201,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  202,  203,    0,  111,    0,    0,
    0,    0,  -62,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  122,    0,    0,    0,    0,  143,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  196,    0,    0,    0,    0,    0,
    0,    0,    0,  199,    0,    0,    0,    0,    0,    0,
  201,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  144,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
static const YYINT yygindex[] = {                         0,
    0,    0,   85,  -38,  -41,  -10,  186,  -57,  185,  -55,
  -81,  163,  169,  -98,    0,    0,    0,   47,    0,    0,
    0,    0,    0,   75,    0,    0,   35,    0,    0,    0,
    0,   34,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -165,  250,
};
#define YYTABLESIZE 273
static const YYINT yytable[] = {                        139,
  140,  132,  133,   47,  143,  144,  101,  102,  105,  106,
  119,  120,   78,  148,   64,  150,   52,  151,   65,  153,
  126,  127,  155,   70,    1,  212,  159,   57,   71,  161,
    5,  163,  164,  219,   18,  165,   48,  166,  246,   19,
   79,   41,  203,  204,  109,  110,   20,  253,   21,   22,
   53,   23,   24,   66,  257,   25,   26,   80,   81,  213,
   58,   27,   28,   72,   73,   40,  220,   74,  266,   29,
   42,   43,   91,   92,   93,   94,   95,   96,   61,  211,
   85,   98,   88,   99,  100,  218,  103,  104,  107,  108,
  224,  225,  227,    7,    8,    9,   10,   11,   12,   13,
   14,   15,   16,   31,   32,   33,   34,   35,   36,   37,
   38,   39,  239,  240,  241,  242,  243,  244,  111,  112,
  114,  116,  117,  118,  121,  217,  122,  123,  124,  202,
  125,  207,  223,  128,  129,  130,  131,  134,  136,  145,
  138,  142,  146,  147,  156,  152,  157,  162,  158,  168,
  167,  169,  171,  174,  173,  176,  178,  179,  181,  182,
  183,  172,  177,  184,  185,  187,  189,  190,  188,  191,
  208,  193,  192,  195,  196,  194,  197,   53,  198,  209,
  200,  206,  231,  282,  215,  229,  288,  234,  289,  235,
  237,  216,  236,  252,  256,  260,  222,  238,  230,  232,
  233,  248,  249,  251,  250,  261,  255,  259,  262,  263,
  264,  265,  270,  271,  267,  268,  274,  269,  273,  275,
  272,  276,  277,  279,  278,  281,  283,  284,  285,  280,
  286,  287,    3,  290,  291,   17,   23,   35,   31,   44,
   39,   55,   83,   88,   59,  113,   49,  115,  254,  135,
  210,  247,    6,    0,  258,    0,    0,    0,  137,    0,
    0,    0,    0,    0,    0,    0,    0,   63,   74,    0,
    0,   80,   85,
};
static const YYINT yycheck[] = {                         98,
   99,   83,   84,  259,  103,  104,   45,   46,   50,   51,
   68,   69,  259,  112,  259,  114,  260,  116,  263,  118,
   76,   77,  121,  259,  258,  262,  125,  262,  264,  128,
   58,  130,  131,  263,  123,  134,  292,  136,  204,  123,
  287,  259,  290,  291,   55,   56,  123,  213,  123,  123,
  294,  123,  123,  298,  220,  123,  123,  304,  305,  296,
  295,  123,  125,  299,  300,   40,  296,  303,  234,  283,
  288,  289,  276,  277,  278,  279,  280,  281,  297,  178,
  306,   58,  307,   40,   40,  184,   58,   40,   58,   40,
  189,  190,  191,  266,  267,  268,  269,  270,  271,  272,
  273,  274,  275,   19,   20,   21,   22,   23,   24,   25,
   26,   27,  309,  310,  311,  312,  313,  314,   58,   40,
   40,   58,   58,   40,   58,  183,   58,   58,   58,  168,
   40,  173,  188,   58,   40,   40,   40,   40,   40,  257,
   41,   41,   41,  284,  286,  285,  301,   41,  302,  123,
   59,   44,   41,   44,  123,  123,   40,   44,   41,   41,
  123,   59,   59,   40,   44,   41,   40,   40,  123,   40,
  261,   41,   44,   59,   41,   44,   41,  294,   44,   41,
   44,   59,  265,  265,   59,  300,  285,  290,  287,   41,
   41,   59,  261,   41,   41,   41,   59,  125,   59,   59,
   59,  125,   58,  125,   59,   44,  125,  125,   41,   44,
   41,  123,  257,  257,   59,   58,  257,   59,   59,   59,
  293,  257,   59,   59,  264,   59,  257,  308,   40,  299,
  125,   40,    0,   41,   41,  125,   41,  125,   41,   41,
  125,   41,   41,   41,  125,   60,  125,   63,  214,   87,
  176,  205,    3,   -1,  221,   -1,   -1,   -1,   90,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  125,  125,   -1,
   -1,  125,  125,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 317
#define YYUNDFTOKEN 367
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'","';'",0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"NUM",
"OPT","ITER","REP","PRECONDITION","DAG","AVEX","RDEFS","RPO","OPT_FI","OPT_CF",
"OPT_DCE","OPT_LCSE","OPT_IVS","OPT_GCSE","OPT_LICM","OPT_GVN","OPT_LU",
"OPT_TRE","MODULE","CFG","FUNCTION","BASICBLOCK","LOOP","INSTRUCTION",
"EXPRESSION","COMMON_FN1","COMMON_FN2","COMMON_FN3","COMMON_FN4","COMMON_FN5",
"FI_FN1","FI_FN2","COMMON_THR1","FI_THR1","CF_FN1","CF_FN2","DCE_FN1",
"LCSE_FN1","CSE_THR1","IVS_FN1","GCSE_FN1","LICM_KW1","LICM_KW2","LICM_FN1",
"LICM_FN2","LICM_FN3","GVN_FN1","GVN_FN2","LU_FN1","TRE_FN1","TRE_FN2","LTE",
"GTE","LT","GT","EQ","NEQ","MINUS","PLUS","END",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : programstart",
"programstart : program",
"program : opts program",
"program :",
"opts : OPT ':' opt '}'",
"opt : OPT_FI '{' runon fi_statements",
"opt : OPT_CF '{' runon cf_statements",
"opt : OPT_DCE '{' runon dce_statements",
"opt : OPT_LCSE '{' runon lcse_statements",
"opt : OPT_IVS '{' runon ivs_statements",
"opt : OPT_GCSE '{' runon gcse_statements",
"opt : OPT_LICM '{' runon licm_statements",
"opt : OPT_GVN '{' runon gvn_statements",
"opt : OPT_LU '{' runon lu_statements",
"opt : OPT_TRE '{' runon tre_statements",
"fi_statements : fi_command fi_statements",
"fi_statements : fi_iterate fi_statements",
"fi_statements :",
"fi_iterate : ITER ':' iterable '{' fi_statements '}'",
"fi_command : FI_FN2 '(' ')' ';'",
"fi_command : FI_FN1 '(' fi_params ')' ';'",
"fi_params : iterable fi_thresh",
"fi_thresh : ',' fi_threshval fi_thresh",
"fi_thresh :",
"fi_threshval : COMMON_THR1 comparop NUM",
"fi_threshval : FI_THR1 comparop NUM",
"cf_statements : cf_iterate cf_statements",
"cf_statements : cf_fncall cf_statements",
"cf_statements :",
"cf_fncall : CF_FN1 '(' iterable cf_precond ')' ';'",
"cf_precond : ',' PRECONDITION ':' CF_FN2 '(' iterable ')'",
"cf_precond :",
"cf_iterate : ITER ':' iterable '{' cf_statements '}'",
"dce_statements : dce_repeat dce_statements",
"dce_statements : dce_fncall dce_statements",
"dce_statements :",
"dce_fncall : DCE_FN1 '(' ')' ';'",
"dce_repeat : REP ':' NUM '{' dce_fncall '}'",
"lcse_statements : lcse_command lcse_statements",
"lcse_statements :",
"lcse_command : LCSE_FN1 '(' lcse_params ')' ';'",
"lcse_command : DAG ':' COMMON_FN2 '(' iterable ')' ';'",
"lcse_params : iterable lcse_thresh",
"lcse_thresh : ',' lcse_threshval lcse_thresh",
"lcse_thresh :",
"lcse_threshval : CSE_THR1 comparop NUM",
"lcse_threshval : DAG",
"gcse_statements : gcse_command gcse_statements",
"gcse_statements : gcse_iterate gcse_statements",
"gcse_statements :",
"gcse_command : GCSE_FN1 '(' gcse_params ')' ';'",
"gcse_command : AVEX ':' COMMON_FN3 '(' iterable ')' ';'",
"gcse_iterate : ITER ':' iterable '{' gcse_statements '}'",
"gcse_params : iterable gcse_thresh",
"gcse_thresh : ',' gcse_threshval gcse_thresh",
"gcse_thresh :",
"gcse_threshval : CSE_THR1 comparop NUM",
"gcse_threshval : AVEX",
"ivs_statements : ivs_command ivs_statements",
"ivs_statements :",
"ivs_command : IVS_FN1 '(' iterable ')' ';'",
"licm_statements : licm_command licm_statements",
"licm_statements : licm_iterate licm_statements",
"licm_statements :",
"licm_command : LICM_KW1 ':' LICM_FN1 '(' licmfn1_params ')' ';'",
"licm_command : LICM_KW2 ':' LICM_FN2 '(' licmfn2_params ')' ';'",
"licm_command : LICM_FN3 '(' licmfn3_params ')' ';'",
"licm_command : RDEFS ':' COMMON_FN4 '(' iterable ')' ';'",
"licm_iterate : ITER ':' iterable '{' licm_statements '}'",
"licmfn1_params : iterable ',' RDEFS",
"licmfn2_params : iterable ',' LICM_KW1",
"licmfn3_params : iterable ',' LICM_KW2",
"gvn_statements : gvn_command gvn_statements",
"gvn_statements : gvn_iterate gvn_statements",
"gvn_statements :",
"gvn_iterate : ITER ':' iterable ',' RPO '{' gvn_statements '}'",
"gvn_command : GVN_FN1 '(' iterable ')' ';'",
"gvn_command : GVN_FN2 '(' iterable ')' ';'",
"gvn_command : COMMON_FN5 '(' ')' ';'",
"lu_statements : lu_command lu_statements",
"lu_statements :",
"lu_command : LU_FN1 '(' iterable lu_params ')' ';'",
"lu_params : ',' COMMON_THR1 comparop NUM",
"lu_params :",
"tre_statements : tre_fncall tre_statements",
"tre_statements :",
"tre_fncall : TRE_FN1 '(' iterable tre_precond ')' ';'",
"tre_precond : ',' PRECONDITION ':' TRE_FN2 '(' iterable ')'",
"tre_precond :",
"comparop : LTE",
"comparop : GTE",
"comparop : LT",
"comparop : GT",
"comparop : EQ",
"comparop : NEQ",
"runon : COMMON_FN1 '(' iterable ')' ';'",
"iterable : MODULE",
"iterable : CFG",
"iterable : FUNCTION",
"iterable : BASICBLOCK",
"iterable : LOOP",
"iterable : INSTRUCTION",

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
#line 201 "coptparser.y"

nodeType *con(int val1, char val2[], int typ)
{
	nodeType *p;

	if ((p = malloc(sizeof(nodeType))) == NULL)		// allocate node
		yyerror("Memory full.");

	p->type = typeConst;	// copy information
	p->con.typ = typ;
	if (typ==1)
		p->con.num = val1;
	else if (typ==2)
		strcpy(p->con.alpha, val2);
				
	return p;
}

nodeType *opr(char oper[], int nops, ...)
{
	va_list ap;
	nodeType *p;
	int i;

	if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)		// allocate node
		yyerror("Memory full");

	p->type = typeOpr;		// copy information
	strcpy(p->opr.oper,oper);
	p->opr.nops = nops;
	
	va_start(ap, nops);
	for (i = 0; i < nops; i++)
		p->opr.op[i] = va_arg(ap, nodeType*);
	va_end(ap);
	return p;
}

void freeNode(nodeType *p)	// free memory
{
	int i;

	if (!p) return;
	if (p->type == typeOpr)
	{
		for (i = 0; i < p->opr.nops; i++)
			freeNode(p->opr.op[i]);
	}
	free(p);
}

void yyerror(char *s)
{
	printf("\nline %d: %s\n", lineno, s);
}

int main()
{
	//remove("Optimiser.cpp");
	//remove("OptimiserDataflow.h");
	//remove("OptimiserDataflow.cpp");
	//remove("OptimiserAvEx.cpp");
	//remove("OptimiserReachingDefs.cpp");
	yyparse();
	return 0;
}
#line 582 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
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
static void yyfreestack(YYSTACKDATA *data)
{
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
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
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
    if (yychar < 0)
    {
        if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            yys = yyname[YYTRANSLATE(yychar)];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
        {
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
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab;

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yystack.s_mark]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
                {
                    goto yyoverflow;
                }
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
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
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
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
    switch (yyn)
    {
case 1:
#line 37 "coptparser.y"
	{ printf("\nOptimizer parsed successfully.\n"); }
break;
case 2:
#line 39 "coptparser.y"
	{	}
break;
case 3:
#line 40 "coptparser.y"
	{	}
break;
case 4:
#line 42 "coptparser.y"
	{ 	yyval.nPtr = opr("optstart", 2, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr);
							printTree(yyval.nPtr, 0);
							semanticAnalysis(yyval.nPtr,optNo);
							if (check==1)	exit(0);
							freeNode(yyval.nPtr);
						}
break;
case 5:
#line 49 "coptparser.y"
	{ yyval.nPtr = opr("fi_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 1;	}
break;
case 6:
#line 50 "coptparser.y"
	{ yyval.nPtr = opr("cf_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 2;	}
break;
case 7:
#line 51 "coptparser.y"
	{ yyval.nPtr = opr("dce_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 3;	}
break;
case 8:
#line 52 "coptparser.y"
	{ yyval.nPtr = opr("lcse_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 4;}
break;
case 9:
#line 53 "coptparser.y"
	{ yyval.nPtr = opr("ivs_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 5;	}
break;
case 10:
#line 54 "coptparser.y"
	{ yyval.nPtr = opr("gcse_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 6;}
break;
case 11:
#line 55 "coptparser.y"
	{ yyval.nPtr = opr("licm_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 7;}
break;
case 12:
#line 56 "coptparser.y"
	{ yyval.nPtr = opr("gvn_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 8;	}
break;
case 13:
#line 57 "coptparser.y"
	{ yyval.nPtr = opr("lu_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 9;	}
break;
case 14:
#line 58 "coptparser.y"
	{ yyval.nPtr = opr("tre_start", 3, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); optNo = 10;}
break;
case 15:
#line 61 "coptparser.y"
	{ yyval.nPtr = opr("fi_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 16:
#line 62 "coptparser.y"
	{ yyval.nPtr = opr("fi_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 17:
#line 63 "coptparser.y"
	{	}
break;
case 18:
#line 65 "coptparser.y"
	{ yyval.nPtr = opr("fi_iteratestart", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-1].nPtr); }
break;
case 19:
#line 67 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[-3].keyword,2); }
break;
case 20:
#line 68 "coptparser.y"
	{ yyval.nPtr = opr("fifn2start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 21:
#line 70 "coptparser.y"
	{ yyval.nPtr = opr("fiparams", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr);}
break;
case 22:
#line 72 "coptparser.y"
	{ yyval.nPtr = opr("params", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 23:
#line 73 "coptparser.y"
	{	}
break;
case 24:
#line 75 "coptparser.y"
	{ yyval.nPtr = opr("fithr1start", 3, con(0,yystack.l_mark[-2].keyword,2), yystack.l_mark[-1].nPtr, con(yystack.l_mark[0].iValue,"",1)); }
break;
case 25:
#line 76 "coptparser.y"
	{ yyval.nPtr = opr("fithr2start", 3, con(0,yystack.l_mark[-2].keyword,2), yystack.l_mark[-1].nPtr, con(yystack.l_mark[0].iValue,"",1)); }
break;
case 26:
#line 79 "coptparser.y"
	{ yyval.nPtr = opr("cf_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 27:
#line 80 "coptparser.y"
	{ yyval.nPtr = opr("cf_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 28:
#line 81 "coptparser.y"
	{	}
break;
case 29:
#line 83 "coptparser.y"
	{ yyval.nPtr = opr("cf_fn1", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-2].nPtr); }
break;
case 30:
#line 84 "coptparser.y"
	{ yyval.nPtr = opr("cf_precond", 3, con(0,yystack.l_mark[-5].keyword,2), con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr); }
break;
case 31:
#line 85 "coptparser.y"
	{	}
break;
case 32:
#line 87 "coptparser.y"
	{ yyval.nPtr = opr("cf_iteratestart", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-1].nPtr); }
break;
case 33:
#line 90 "coptparser.y"
	{ yyval.nPtr = opr("dce_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 34:
#line 91 "coptparser.y"
	{ yyval.nPtr = opr("dce_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 35:
#line 92 "coptparser.y"
	{	}
break;
case 36:
#line 94 "coptparser.y"
	{ yyval.nPtr = opr("dcefn1", 1, con(0,yystack.l_mark[-3].keyword,2)); }
break;
case 37:
#line 96 "coptparser.y"
	{ yyval.nPtr = opr("dce_repeatstart", 3, con(0,yystack.l_mark[-5].keyword,2), con(yystack.l_mark[-3].iValue,"",1), yystack.l_mark[-1].nPtr); }
break;
case 38:
#line 99 "coptparser.y"
	{ yyval.nPtr = opr("lcse_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 39:
#line 100 "coptparser.y"
	{	}
break;
case 40:
#line 102 "coptparser.y"
	{ yyval.nPtr = opr("lcsefn1start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 41:
#line 103 "coptparser.y"
	{ yyval.nPtr = opr("lcsefn2", 3, con(0,yystack.l_mark[-6].keyword,2), con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 42:
#line 105 "coptparser.y"
	{ yyval.nPtr = opr("lcseparams", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr);}
break;
case 43:
#line 107 "coptparser.y"
	{ yyval.nPtr = opr("params", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 44:
#line 108 "coptparser.y"
	{	}
break;
case 45:
#line 110 "coptparser.y"
	{ yyval.nPtr = opr("lcsethr1", 3, con(0,yystack.l_mark[-2].keyword,2), yystack.l_mark[-1].nPtr, con(yystack.l_mark[0].iValue,"",1)); }
break;
case 46:
#line 111 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 47:
#line 114 "coptparser.y"
	{ yyval.nPtr = opr("gcse_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 48:
#line 115 "coptparser.y"
	{ yyval.nPtr = opr("gcse_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 49:
#line 116 "coptparser.y"
	{	}
break;
case 50:
#line 118 "coptparser.y"
	{ yyval.nPtr = opr("gcsefn1start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 51:
#line 119 "coptparser.y"
	{ yyval.nPtr = opr("gcsefn2", 3, con(0,yystack.l_mark[-6].keyword,2), con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 52:
#line 121 "coptparser.y"
	{ yyval.nPtr = opr("gcse_iteratestart", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-1].nPtr); }
break;
case 53:
#line 123 "coptparser.y"
	{ yyval.nPtr = opr("gcseparams", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr);}
break;
case 54:
#line 125 "coptparser.y"
	{ yyval.nPtr = opr("params", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 55:
#line 126 "coptparser.y"
	{	}
break;
case 56:
#line 128 "coptparser.y"
	{ yyval.nPtr = opr("gcsethr1", 3, con(0,yystack.l_mark[-2].keyword,2), yystack.l_mark[-1].nPtr, con(yystack.l_mark[0].iValue,"",1)); }
break;
case 57:
#line 129 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 58:
#line 132 "coptparser.y"
	{ yyval.nPtr = opr("ivs_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 59:
#line 133 "coptparser.y"
	{	}
break;
case 60:
#line 135 "coptparser.y"
	{ yyval.nPtr = opr("ivsfn1", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 61:
#line 138 "coptparser.y"
	{ yyval.nPtr = opr("licm_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 62:
#line 139 "coptparser.y"
	{ yyval.nPtr = opr("licm_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 63:
#line 140 "coptparser.y"
	{	}
break;
case 64:
#line 142 "coptparser.y"
	{ yyval.nPtr = opr("licmfn1start", 3, con(0,yystack.l_mark[-6].keyword,2), con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 65:
#line 143 "coptparser.y"
	{ yyval.nPtr = opr("licmfn2start", 3, con(0,yystack.l_mark[-6].keyword,2), con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 66:
#line 144 "coptparser.y"
	{ yyval.nPtr = opr("licmfn3start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr);	}
break;
case 67:
#line 145 "coptparser.y"
	{ yyval.nPtr = opr("licmrdfn", 3, con(0,yystack.l_mark[-6].keyword,2), con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 68:
#line 147 "coptparser.y"
	{ yyval.nPtr = opr("licm_iteratestart", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-1].nPtr); }
break;
case 69:
#line 149 "coptparser.y"
	{ yyval.nPtr = opr("licmfn1params", 2, yystack.l_mark[-2].nPtr, con(0,yystack.l_mark[0].keyword,2));}
break;
case 70:
#line 151 "coptparser.y"
	{ yyval.nPtr = opr("licmfn2params", 2, yystack.l_mark[-2].nPtr, con(0,yystack.l_mark[0].keyword,2));}
break;
case 71:
#line 153 "coptparser.y"
	{ yyval.nPtr = opr("licmfn3params", 2, yystack.l_mark[-2].nPtr, con(0,yystack.l_mark[0].keyword,2));}
break;
case 72:
#line 156 "coptparser.y"
	{ yyval.nPtr = opr("gvn_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 73:
#line 157 "coptparser.y"
	{ yyval.nPtr = opr("gvn_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 74:
#line 158 "coptparser.y"
	{	}
break;
case 75:
#line 160 "coptparser.y"
	{ yyval.nPtr = opr("gvn_iteratestart", 4, con(0,yystack.l_mark[-7].keyword,2), yystack.l_mark[-5].nPtr, con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr); }
break;
case 76:
#line 162 "coptparser.y"
	{ yyval.nPtr = opr("gvnfn1start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 77:
#line 163 "coptparser.y"
	{ yyval.nPtr = opr("gvnfn2start", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr);	}
break;
case 78:
#line 164 "coptparser.y"
	{ yyval.nPtr = opr("gvnfn5", 1, con(0,yystack.l_mark[-3].keyword,2)); }
break;
case 79:
#line 167 "coptparser.y"
	{ yyval.nPtr = opr("lu_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 80:
#line 168 "coptparser.y"
	{	}
break;
case 81:
#line 170 "coptparser.y"
	{ yyval.nPtr = opr("lufn1start", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-2].nPtr); }
break;
case 82:
#line 172 "coptparser.y"
	{ yyval.nPtr = opr("luparams", 3, con(0,yystack.l_mark[-2].keyword,2), yystack.l_mark[-1].nPtr, con(yystack.l_mark[0].iValue,"",1));}
break;
case 83:
#line 173 "coptparser.y"
	{	}
break;
case 84:
#line 176 "coptparser.y"
	{ yyval.nPtr = opr("tre_statements", 2, yystack.l_mark[-1].nPtr, yystack.l_mark[0].nPtr); }
break;
case 85:
#line 177 "coptparser.y"
	{	}
break;
case 86:
#line 179 "coptparser.y"
	{ yyval.nPtr = opr("tre_fn1", 3, con(0,yystack.l_mark[-5].keyword,2), yystack.l_mark[-3].nPtr, yystack.l_mark[-2].nPtr); }
break;
case 87:
#line 180 "coptparser.y"
	{ yyval.nPtr = opr("tre_precond", 3, con(0,yystack.l_mark[-5].keyword,2), con(0,yystack.l_mark[-3].keyword,2), yystack.l_mark[-1].nPtr); }
break;
case 88:
#line 181 "coptparser.y"
	{	}
break;
case 89:
#line 184 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 90:
#line 185 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 91:
#line 186 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 92:
#line 187 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 93:
#line 188 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 94:
#line 189 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].op,2); }
break;
case 95:
#line 191 "coptparser.y"
	{ yyval.nPtr = opr("runonstart", 2, con(0,yystack.l_mark[-4].keyword,2), yystack.l_mark[-2].nPtr); }
break;
case 96:
#line 193 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 97:
#line 194 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 98:
#line 195 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 99:
#line 196 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 100:
#line 197 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
case 101:
#line 198 "coptparser.y"
	{ yyval.nPtr = con(0,yystack.l_mark[0].keyword,2); }
break;
#line 1193 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            if ((yychar = YYLEX) < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
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
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM)
    {
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
