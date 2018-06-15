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
extern YYSTYPE yylval;
