%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#include "coptast.h"
	#include "coptsemantic.h"
	
	extern int lineno;	// track line number
	int check = 0;		// check for semantic error
	int optNo = 0;		// indicates where to generate the file
	
	void yyerror(char *);
	int yylex();
	
	nodeType *opr(char oper[], int nops, ...);		// prototypes
	nodeType *con(int val1, char val2[], int typ);
	void freeNode(nodeType *p);
%}

%union
{
	int iValue;                 // integer value
	char keyword[25];			// string keywords
	char op[3];					// operators
	struct nodeTypeTag *nPtr;	// node pointer
};

%token <iValue> NUM
%token <keyword> OPT ITER REP PRECONDITION DAG AVEX RDEFS RPO OPT_FI OPT_CF OPT_DCE OPT_LCSE OPT_IVS OPT_GCSE OPT_LICM OPT_GVN OPT_LU OPT_TRE MODULE CFG FUNCTION BASICBLOCK LOOP INSTRUCTION EXPRESSION COMMON_FN1 COMMON_FN2 COMMON_FN3 COMMON_FN4 COMMON_FN5 FI_FN1 FI_FN2 COMMON_THR1 FI_THR1 CF_FN1 CF_FN2 DCE_FN1 LCSE_FN1 CSE_THR1 IVS_FN1 GCSE_FN1 LICM_KW1 LICM_KW2 LICM_FN1 LICM_FN2 LICM_FN3 GVN_FN1 GVN_FN2 LU_FN1 TRE_FN1 TRE_FN2
%token <op> LTE GTE LT GT EQ NEQ MINUS PLUS 
%token END
%left PLUS MINUS LT GT LTE GTE EQ NEQ
%type <nPtr> opts opt runon fi_statements cf_statements dce_statements lcse_statements gcse_statements ivs_statements licm_statements gvn_statements lu_statements tre_statements iterable fi_command fi_iterate fi_threshval fi_thresh fi_params cf_iterate cf_fncall cf_precond dce_repeat dce_fncall lcse_command lcse_params lcse_thresh lcse_threshval gcse_command gcse_iterate gcse_params gcse_thresh gcse_threshval ivs_command licm_command licm_iterate licmfn1_params licmfn2_params licmfn3_params gvn_iterate gvn_command lu_command lu_params tre_fncall tre_precond comparop

%%
programstart :	program	{ printf("\nOptimizer parsed successfully.\n"); }
				;
program :	opts program	{	}
			|				{	}
			;
opts :	OPT ':' opt '}'	{ 	$$ = opr("optstart", 2, con(0,$1,2), $3);
							printTree($$, 0);
							semanticAnalysis($$,optNo);
							if (check==1)	exit(0);
							freeNode($$);
						}
	;
opt : OPT_FI  '{' runon fi_statements		{ $$ = opr("fi_start", 3, con(0,$1,2), $3, $4); optNo = 1;	}
	| OPT_CF  '{' runon cf_statements		{ $$ = opr("cf_start", 3, con(0,$1,2), $3, $4); optNo = 2;	}
	| OPT_DCE '{' runon dce_statements		{ $$ = opr("dce_start", 3, con(0,$1,2), $3, $4); optNo = 3;	}
	| OPT_LCSE '{' runon lcse_statements	{ $$ = opr("lcse_start", 3, con(0,$1,2), $3, $4); optNo = 4;}
	| OPT_IVS '{' runon ivs_statements		{ $$ = opr("ivs_start", 3, con(0,$1,2), $3, $4); optNo = 5;	}
	| OPT_GCSE '{' runon gcse_statements	{ $$ = opr("gcse_start", 3, con(0,$1,2), $3, $4); optNo = 6;}
	| OPT_LICM '{' runon licm_statements	{ $$ = opr("licm_start", 3, con(0,$1,2), $3, $4); optNo = 7;}
	| OPT_GVN '{' runon gvn_statements		{ $$ = opr("gvn_start", 3, con(0,$1,2), $3, $4); optNo = 8;	}
	| OPT_LU '{' runon lu_statements		{ $$ = opr("lu_start", 3, con(0,$1,2), $3, $4); optNo = 9;	}
	| OPT_TRE '{' runon tre_statements		{ $$ = opr("tre_start", 3, con(0,$1,2), $3, $4); optNo = 10;}
	;

fi_statements :	fi_command fi_statements	{ $$ = opr("fi_statements", 2, $1, $2); }
				| fi_iterate fi_statements	{ $$ = opr("fi_statements", 2, $1, $2); }
				|							{	}
				;
fi_iterate :	ITER ':' iterable '{' fi_statements '}'	{ $$ = opr("fi_iteratestart", 3, con(0,$1,2), $3, $5); }
				;
fi_command :	FI_FN2 '(' ')' ';'				{ $$ = con(0,$1,2); }
				| FI_FN1 '(' fi_params ')' ';'	{ $$ = opr("fifn2start", 2, con(0,$1,2), $3); }
				;
fi_params :	iterable fi_thresh	{ $$ = opr("fiparams", 2, $1, $2);}
			;
fi_thresh :	',' fi_threshval fi_thresh	{ $$ = opr("params", 2, $2, $3); }
			|							{	}
			;
fi_threshval :	COMMON_THR1 comparop NUM	{ $$ = opr("fithr1start", 3, con(0,$1,2), $2, con($3,"",1)); }
				| FI_THR1 comparop NUM	{ $$ = opr("fithr2start", 3, con(0,$1,2), $2, con($3,"",1)); }
				;
				
cf_statements : cf_iterate cf_statements	{ $$ = opr("cf_statements", 2, $1, $2); }
				| cf_fncall cf_statements	{ $$ = opr("cf_statements", 2, $1, $2); }
				|							{	}
				;
cf_fncall :	CF_FN1 '(' iterable cf_precond ')' ';' 			{ $$ = opr("cf_fn1", 3, con(0,$1,2), $3, $4); };
cf_precond : ',' PRECONDITION ':' CF_FN2 '(' iterable ')'	{ $$ = opr("cf_precond", 3, con(0,$2,2), con(0,$4,2), $6); }
			| {	}
			;
cf_iterate :	ITER ':' iterable '{' cf_statements '}'	{ $$ = opr("cf_iteratestart", 3, con(0,$1,2), $3, $5); }
			;

dce_statements : dce_repeat dce_statements	{ $$ = opr("dce_statements", 2, $1, $2); }
				| dce_fncall dce_statements	{ $$ = opr("dce_statements", 2, $1, $2); }
				|							{	}
				;
dce_fncall :	DCE_FN1 '(' ')' ';' { $$ = opr("dcefn1", 1, con(0,$1,2)); }
			;
dce_repeat : 	REP ':' NUM '{' dce_fncall '}'	{ $$ = opr("dce_repeatstart", 3, con(0,$1,2), con($3,"",1), $5); }
			;

lcse_statements : lcse_command lcse_statements	{ $$ = opr("lcse_statements", 2, $1, $2); }
				|								{	}
				;
lcse_command : LCSE_FN1 '(' lcse_params ')' ';'	{ $$ = opr("lcsefn1start", 2, con(0,$1,2), $3); }
			| DAG ':' COMMON_FN2 '(' iterable ')' ';'	{ $$ = opr("lcsefn2", 3, con(0,$1,2), con(0,$3,2), $5); }
			;
lcse_params : 	iterable lcse_thresh	{ $$ = opr("lcseparams", 2, $1, $2);}
			;
lcse_thresh : ',' lcse_threshval lcse_thresh	{ $$ = opr("params", 2, $2, $3); }
			|						{	}
			;
lcse_threshval : CSE_THR1 comparop NUM	{ $$ = opr("lcsethr1", 3, con(0,$1,2), $2, con($3,"",1)); }
				| DAG					{ $$ = con(0,$1,2); }
				;
				
gcse_statements : gcse_command gcse_statements	{ $$ = opr("gcse_statements", 2, $1, $2); }
				| gcse_iterate gcse_statements	{ $$ = opr("gcse_statements", 2, $1, $2); }
				|								{	}
				;
gcse_command : GCSE_FN1 '(' gcse_params ')' ';'	{ $$ = opr("gcsefn1start", 2, con(0,$1,2), $3); }
			| AVEX ':' COMMON_FN3 '(' iterable ')' ';'	{ $$ = opr("gcsefn2", 3, con(0,$1,2), con(0,$3,2), $5); }
			;
gcse_iterate :	ITER ':' iterable '{' gcse_statements '}'	{ $$ = opr("gcse_iteratestart", 3, con(0,$1,2), $3, $5); }
			;
gcse_params : 	iterable gcse_thresh	{ $$ = opr("gcseparams", 2, $1, $2);}
			;
gcse_thresh : ',' gcse_threshval gcse_thresh	{ $$ = opr("params", 2, $2, $3); }
			|						{	}
			;
gcse_threshval : CSE_THR1 comparop NUM	{ $$ = opr("gcsethr1", 3, con(0,$1,2), $2, con($3,"",1)); }
				| AVEX					{ $$ = con(0,$1,2); }
				;

ivs_statements : ivs_command ivs_statements	{ $$ = opr("ivs_statements", 2, $1, $2); }
				|							{	}
				;
ivs_command : IVS_FN1 '(' iterable ')' ';' { $$ = opr("ivsfn1", 2, con(0,$1,2), $3); }
			;
			
licm_statements : licm_command licm_statements	{ $$ = opr("licm_statements", 2, $1, $2); }
				| licm_iterate licm_statements	{ $$ = opr("licm_statements", 2, $1, $2); }
				|								{	}
				;	
licm_command : LICM_KW1 ':' LICM_FN1 '(' licmfn1_params ')' ';'	{ $$ = opr("licmfn1start", 3, con(0,$1,2), con(0,$3,2), $5); }
			 | LICM_KW2 ':' LICM_FN2 '(' licmfn2_params ')' ';'	{ $$ = opr("licmfn2start", 3, con(0,$1,2), con(0,$3,2), $5); }
			 | LICM_FN3 '(' licmfn3_params ')' ';'				{ $$ = opr("licmfn3start", 2, con(0,$1,2), $3);	}
			 | RDEFS ':' COMMON_FN4 '(' iterable ')' ';'		{ $$ = opr("licmrdfn", 3, con(0,$1,2), con(0,$3,2), $5); }
			;
licm_iterate :	ITER ':' iterable '{' licm_statements '}'	{ $$ = opr("licm_iteratestart", 3, con(0,$1,2), $3, $5); }
			;
licmfn1_params : iterable ',' RDEFS	{ $$ = opr("licmfn1params", 2, $1, con(0,$3,2));}
			;			
licmfn2_params : iterable ',' LICM_KW1	{ $$ = opr("licmfn2params", 2, $1, con(0,$3,2));}
			;			
licmfn3_params : iterable ',' LICM_KW2	{ $$ = opr("licmfn3params", 2, $1, con(0,$3,2));}
			;			
			
gvn_statements : gvn_command gvn_statements		{ $$ = opr("gvn_statements", 2, $1, $2); }
				| gvn_iterate gvn_statements	{ $$ = opr("gvn_statements", 2, $1, $2); }
				|								{	}
				;
gvn_iterate : ITER ':' iterable ',' RPO  '{' gvn_statements '}'	{ $$ = opr("gvn_iteratestart", 4, con(0,$1,2), $3, con(0,$5,2), $7); }
			;
gvn_command : GVN_FN1 '(' iterable ')' ';'	{ $$ = opr("gvnfn1start", 2, con(0,$1,2), $3); }
			 | GVN_FN2 '(' iterable ')' ';'				{ $$ = opr("gvnfn2start", 2, con(0,$1,2), $3);	}
			 | COMMON_FN5 '(' ')' ';'		{ $$ = opr("gvnfn5", 1, con(0,$1,2)); }
			;
			
lu_statements : lu_command lu_statements	{ $$ = opr("lu_statements", 2, $1, $2); }
				|							{	}
				;
lu_command : LU_FN1 '(' iterable lu_params ')' ';'	{ $$ = opr("lufn1start", 3, con(0,$1,2), $3, $4); }
			;
lu_params : ',' COMMON_THR1 comparop NUM	{ $$ = opr("luparams", 3, con(0,$2,2), $3, con($4,"",1));}
			|								{	}
			;
			
tre_statements : tre_fncall tre_statements	{ $$ = opr("tre_statements", 2, $1, $2); }
				|							{	}
				;
tre_fncall :	TRE_FN1 '(' iterable tre_precond ')' ';' 	{ $$ = opr("tre_fn1", 3, con(0,$1,2), $3, $4); };
tre_precond : ',' PRECONDITION ':' TRE_FN2 '(' iterable ')'	{ $$ = opr("tre_precond", 3, con(0,$2,2), con(0,$4,2), $6); }
			| {	}
			;

comparop :	LTE		{ $$ = con(0,$1,2); }
			| GTE	{ $$ = con(0,$1,2); }
			| LT	{ $$ = con(0,$1,2); } 
			| GT	{ $$ = con(0,$1,2); } 
			| EQ	{ $$ = con(0,$1,2); }
			| NEQ	{ $$ = con(0,$1,2); }
			; 	
runon :	COMMON_FN1 '(' iterable ')' ';'	{ $$ = opr("runonstart", 2, con(0,$1,2), $3); }
		;	
iterable :	MODULE			{ $$ = con(0,$1,2); }
			| CFG			{ $$ = con(0,$1,2); } 
			| FUNCTION		{ $$ = con(0,$1,2); }
			| BASICBLOCK	{ $$ = con(0,$1,2); }
			| LOOP			{ $$ = con(0,$1,2); }
			| INSTRUCTION	{ $$ = con(0,$1,2); }
	;	
%%

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
