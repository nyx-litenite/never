
%{
#include <stdio.h>
#include "utils.h"
#include "types.h"
#include "scanner.h"
#include "never.h"

int parse_result;
extern int line_no;

int yylex(token * tokp)
{
    return lex_scan(tokp);
}

int yyerror(never ** nev, char * str)
{
    parse_result = 1;
    print_error_msg(line_no, "%s\n", str);
    
    return 1;
}
%}

%token <val.str_value> TOK_ID
%token <val.float_value> TOK_NUM_FLOAT
%token <val.int_value> TOK_NUM_INT
%token <val.str_value> TOK_FLOAT
%token <val.str_value> TOK_INT
%token <val.str_value> TOK_FUNC
%token <val.str_value> TOK_RET /* -> */
%token <val.str_value> TOK_RETURN

%type <val.expr_value> expr
%type <val.expr_list_value> expr_list
%type <val.var_value> dim
%type <val.var_list_value> dim_list
%type <val.var_value> var
%type <val.var_list_value> var_list
%type <val.array_value> array;
%type <val.func_value> func
%type <val.func_list_value> func_list
%type <val.func_body_value> func_body
%type <val.never_value> never

%right <val.str_value> '?' ':'
%left TOK_OR
%left TOK_AND
%left TOK_EQ TOK_NEQ
%left <val.str_value> '<' '>' TOK_LTE TOK_GTE
%left <val.str_value> '+' '-'
%left <val.str_value> '*' '/' '%'
%right TOK_NOT /* %precedence NEG */
%left <val.str_value> '(' ')' '[' ']' ARR_DIM_BEG ARR_DIM_END

%start never

%destructor { free($$); } TOK_ID
%destructor { var_delete($$); } dim
%destructor { var_list_delete($$); } dim_list
%destructor { var_delete($$); } var
%destructor { var_list_delete($$); } var_list
%destructor { expr_delete($$); } expr
%destructor { expr_list_delete($$); } expr_list
%destructor { array_delete($$); } array
%destructor { func_delete($$); } func
%destructor { func_list_delete($$); } func_list
%destructor { func_body_delete($$); } func_body
%destructor {  } never

%pure-parser
%parse-param { never ** nev }

%%

expr: TOK_ID
{
    $$ = expr_new_id($1);
    $$->line_no = $<line_no>1;
};

expr: TOK_NUM_INT
{
    $$ = expr_new_int($1);
    $$->line_no = $<line_no>1;
};

expr: TOK_NUM_FLOAT
{
    $$ = expr_new_float($1);
    $$->line_no = $<line_no>1;
};

expr: '-' expr %prec TOK_NOT
{
    $$ = expr_new_one(EXPR_NEG, $2);
    $$->line_no = $<line_no>1;
};

expr: expr '+' expr
{
    $$ = expr_new_two(EXPR_ADD, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '-' expr
{
    $$ = expr_new_two(EXPR_SUB, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '*' expr
{
    $$ = expr_new_two(EXPR_MUL, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '/' expr
{
    $$ = expr_new_two(EXPR_DIV, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '%' expr
{
    $$ = expr_new_two(EXPR_MOD, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '<' expr
{
    $$ = expr_new_two(EXPR_LT, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_LTE expr
{
    $$ = expr_new_two(EXPR_LTE, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr '>' expr
{
    $$ = expr_new_two(EXPR_GT, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_GTE expr
{
    $$ = expr_new_two(EXPR_GTE, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_EQ expr
{
    $$ = expr_new_two(EXPR_EQ, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_NEQ expr
{
    $$ = expr_new_two(EXPR_NEQ, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_AND expr
{
    $$ = expr_new_two(EXPR_AND, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: expr TOK_OR expr
{
    $$ = expr_new_two(EXPR_OR, $1, $3);
    $$->line_no = $<line_no>2;
};

expr: TOK_NOT expr
{
    $$ = expr_new_one(EXPR_NOT, $2);
    $$->line_no = $<line_no>1;
};

expr: '(' expr ')'
{
    $$ = expr_new_one(EXPR_SUP, $2);
    $$->line_no = $<line_no>1;
};

expr: expr '?' expr ':' expr
{
    $$ = expr_new_three(EXPR_COND, $1, $3, $5);
    $$->line_no = $<line_no>2;
};

expr: array
{
    $$ = expr_new_array($1);
    $$->line_no = $1->line_no;
};

expr: expr '[' expr_list ']' /* array dereference */
{
    $$ = expr_new_array_deref($1, $3);
    $$->line_no = $<line_no>2;
};

array: ARR_DIM_BEG expr_list ARR_DIM_END TOK_RET var
{
    $$ = array_new_dims($2, $5);
    $$->line_no = $<line_no>1;
};

array: '{' expr_list '}' TOK_RET var
{
    $$ = array_new($2, $5);
    $$->line_no = $<line_no>1;
};

array: '{' expr_list '}'
{
    $$ = array_new_sub($2);
    $$->line_no = $<line_no>1;
};

expr: func
{
    $$ = expr_new_func($1);
};

expr: expr '(' ')'
{
    $$ = expr_new_call($1, NULL);
    $$->line_no = $1->line_no;
};

expr: expr '(' expr_list ')'
{
    $$ = expr_new_call($1, $3);
    $$->line_no = $1->line_no;
};

expr_list: expr
{
    $$ = expr_list_new();
    expr_list_add_end($$, $1);
};

expr_list: expr_list ',' expr
{
    expr_list_add_end($1, $3);
    $$ = $1;
};

var: TOK_INT
{
    $$ = var_new_int(NULL);
    $$->line_no = $<line_no>1;
};

var: TOK_ID TOK_RET TOK_INT
{
    $$ = var_new_int($1);
    $$->line_no = $<line_no>2;
};

var: TOK_FLOAT
{
    $$ = var_new_float(NULL);
    $$->line_no = $<line_no>1;
};

var: TOK_ID TOK_RET TOK_FLOAT 
{
    $$ = var_new_float($1);
    $$->line_no = $<line_no>2;
};

var: '[' dim_list ']' TOK_RET var
{
    $$ = var_new_array(NULL, $2, $5);
    $$->line_no = $<line_no>1;
};

var: TOK_ID '[' dim_list ']' TOK_RET var
{
    $$ = var_new_array($1, $3, $6);
    $$->line_no = $<line_no>1;
};

var: '(' ')' TOK_RET var
{
    $$ = var_new_func(NULL, NULL, $4);
    $$->line_no = $<line_no>4;
};

var: '(' var_list ')' TOK_RET var
{
    $$ = var_new_func(NULL, $2, $5);
    $$->line_no = $<line_no>5;
};

var: TOK_ID '(' ')' TOK_RET var
{
    $$ = var_new_func($1, NULL, $5);
    $$->line_no = $<line_no>5;
};

var: TOK_ID '(' var_list ')' TOK_RET var
{
    $$ = var_new_func($1, $3, $6);
    $$->line_no = $<line_no>6;
};

dim: TOK_ID
{
    $$ = var_new_dim($1);
    $$->line_no = $<line_no>1;
};

dim_list: dim
{
    $$ = var_list_new();
    var_list_add_end($$, $1);
};

dim_list: dim_list ',' dim
{
    var_list_add_end($1, $3);
    $$ = $1;
};

var_list: var
{
    $$ = var_list_new();
    var_list_add_end($$, $1);
};

var_list: var_list ',' var
{
    var_list_add_end($1, $3);
    $$ = $1;
};

func: TOK_FUNC TOK_ID '(' ')' TOK_RET var func_body
{
    $$ = func_new($2, NULL, $6, $7);
    $$->line_no = $<line_no>2;
};

func: TOK_FUNC TOK_ID '(' var_list ')' TOK_RET var func_body
{
    $$ = func_new($2, $4, $7, $8);
    $$->line_no = $<line_no>2;
};

func: TOK_FUNC TOK_ID error
{
    print_error_msg(line_no, "error in function %s defined at %d\n", $2, $<line_no>2);
    free($2);
    
    yyclearin;
    yyerrok;
    $$ = NULL;
};

func_body: '{' func_list TOK_RETURN expr ';' '}'
{
    $$ = func_body_new($2, $4);
};

func_body: '{' TOK_RETURN expr ';' '}'
{
    $$ = func_body_new(NULL, $3);
};

func_body: '{' '}'
{
    $$ = func_body_new(NULL, NULL);
};

func_list: func
{
    $$ = func_list_new();
    func_list_add_end($$, $1);
};

func_list: func_list func
{
    func_list_add_end($1, $2);
    $$ = $1;
};

never: func_list
{
    $$ = *nev = never_new($1);
};

%%

