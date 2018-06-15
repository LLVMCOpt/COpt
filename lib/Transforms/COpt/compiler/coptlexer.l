%{
	#include "y.tab.h"
	int lineno = 1;	// to track line number
	void yyerror(char *);
%}

%%
"opt"				{	strcpy(yylval.keyword, yytext); return OPT;			}	//	basic keywords
"iterate"			{	strcpy(yylval.keyword, yytext); return ITER;		}
"repeat"			{	strcpy(yylval.keyword, yytext); return REP;			}
"precondition"		{	strcpy(yylval.keyword, yytext); return PRECONDITION;}
"dag"				{	strcpy(yylval.keyword, yytext); return DAG;			}
"availableexprs"	{	strcpy(yylval.keyword, yytext); return AVEX;		}
"reachingdefs"		{	strcpy(yylval.keyword, yytext); return RDEFS;		}
"reversepostorder"	{	strcpy(yylval.keyword, yytext); return RPO;			}

"runOn"							{	strcpy(yylval.keyword, yytext); return COMMON_FN1;	}	// common functions
"makeDAG"						{	strcpy(yylval.keyword, yytext); return COMMON_FN2;	}
"findAvailableExpressionsInfo"	{	strcpy(yylval.keyword, yytext); return COMMON_FN3;	}
"findReachingDefinitionsInfo"	{	strcpy(yylval.keyword, yytext); return COMMON_FN4;	}
"eraseUnusedInstructions"		{	strcpy(yylval.keyword, yytext); return COMMON_FN5;	}

"module"		{	strcpy(yylval.keyword, yytext); return MODULE;		}	// iterables
"cfg"			{	strcpy(yylval.keyword, yytext); return CFG;			}
"function"		{	strcpy(yylval.keyword, yytext); return FUNCTION;	}
"basicblock"	{	strcpy(yylval.keyword, yytext); return BASICBLOCK;	}
"loop"			{	strcpy(yylval.keyword, yytext); return LOOP;		}
"instruction"	{	strcpy(yylval.keyword, yytext); return INSTRUCTION;	}
"expression"	{	strcpy(yylval.keyword, yytext); return EXPRESSION;	}

"FunctionInlining"			{	strcpy(yylval.keyword, yytext); return OPT_FI;	}	// optimizations
"ConstantFolding"			{	strcpy(yylval.keyword, yytext); return OPT_CF;	}
"DeadCodeElimination"		{	strcpy(yylval.keyword, yytext); return OPT_DCE;	}
"LocalCSE"					{	strcpy(yylval.keyword, yytext); return OPT_LCSE;}
"GlobalCSE"					{	strcpy(yylval.keyword, yytext); return OPT_GCSE;}
"IndVarSimplify"			{	strcpy(yylval.keyword, yytext); return OPT_IVS;	}
"LoopInvariantCodeMotion"	{	strcpy(yylval.keyword, yytext); return OPT_LICM;}
"GlobalVN"					{	strcpy(yylval.keyword, yytext); return OPT_GVN;	}
"LoopUnrolling"				{	strcpy(yylval.keyword, yytext); return OPT_LU;	}
"TailCallElim"				{	strcpy(yylval.keyword, yytext); return OPT_TRE;	}

"makeInline"				{	strcpy(yylval.keyword, yytext); return FI_FN1;		}	// function inlining functions
"removeInlinedFunctions"	{	strcpy(yylval.keyword, yytext); return FI_FN2;		}
"noLines"					{	strcpy(yylval.keyword, yytext); return COMMON_THR1;	}	// function inlining thresholds
"noUses"					{	strcpy(yylval.keyword, yytext); return FI_THR1;		}

"foldpropagate"				{	strcpy(yylval.keyword, yytext); return CF_FN1;	}	// constant folding functions
"foldable"					{	strcpy(yylval.keyword, yytext); return CF_FN2;	}

"deadcodeelim"				{	strcpy(yylval.keyword, yytext); return DCE_FN1;	}	// dead code elimination functions

"lcse"						{	strcpy(yylval.keyword, yytext); return LCSE_FN1;}	// local common subexpression elimination functions
"noExprs"					{	strcpy(yylval.keyword, yytext); return CSE_THR1;}	// local common subexpression elimination thresholds

"indvarsimplify"			{	strcpy(yylval.keyword, yytext); return IVS_FN1;}	// induction variables simplify function

"gcse"						{	strcpy(yylval.keyword, yytext); return GCSE_FN1;}	// global common subexpression elimination functions

"loopinvstmts"					{	strcpy(yylval.keyword, yytext); return LICM_KW1;}	// loop invariant code motion keywords
"motionstmts"					{	strcpy(yylval.keyword, yytext); return LICM_KW2;}
"findLoopInvariantStatements"	{	strcpy(yylval.keyword, yytext); return LICM_FN1;}	// loop invariant code motion functions
"findMotionCandidates"			{	strcpy(yylval.keyword, yytext); return LICM_FN2;}
"applyCodeMotion"				{	strcpy(yylval.keyword, yytext); return LICM_FN3;}

"valuenumber"				{	strcpy(yylval.keyword, yytext); return GVN_FN1; }
"eliminateInstructions"		{	strcpy(yylval.keyword, yytext); return GVN_FN2; }

"unrollLoop"				{	strcpy(yylval.keyword, yytext); return LU_FN1; }

"tailcallelim"				{	strcpy(yylval.keyword, yytext); return TRE_FN1; }
"hasRecursiveTailCall"		{	strcpy(yylval.keyword, yytext); return TRE_FN2; }

"<="							{	strcpy(yylval.op, yytext); return LTE;		}
">="							{	strcpy(yylval.op, yytext); return GTE;		}
"<"								{	strcpy(yylval.op, yytext); return LT;		}
">"								{	strcpy(yylval.op, yytext); return GT;		}
"=="							{	strcpy(yylval.op, yytext); return EQ;		}
"!="							{	strcpy(yylval.op, yytext); return NEQ;		}
[0-9]+							{	yylval.iValue = atoi(yytext); return NUM;	}
"-"								{	strcpy(yylval.op, yytext); return MINUS;	}
"+"								{	strcpy(yylval.op, yytext); return PLUS;		}
")"								{	return ')';		}
"("								{	return '(';		}
"{"								{	return '{';		}
"}"								{	return '}';		}
","								{	return ',';		}
";"								{	return ';';		}
":"								{	return ':';		}
[\t ]							{	;				}	// remove whitespaces
\n								{	lineno++;		}	// track line number
[^\"]"//".*						{	;				}	// remove comments
"/*"((\")|[^*]|\*+[^*/])*\*+"/"	{	;				}	// remove comments
"$"								{	return END;		}
.								{ yyerror("Character not recognized.");	}
%%

int yywrap()
{
	return yylineno;
}
