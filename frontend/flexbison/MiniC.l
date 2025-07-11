%{
/* 这里声明语义动作符程序所需要的函数原型或者变量原型或定义等 */
/* 主要包含头文件，extern的全局变量，定义的全局变量等 */

#include <cstdio>
#include <cstdint>
#include <cstring>

// 此文件定义了文法中终结符的类别
#include "BisonParser.h"

// 对于整数或浮点数，词法识别无符号数，对于负数，识别为求负运算符与无符号数，请注意。
%}

/* 使它不要添加默认的规则,这样输入无法被给定的规则完全匹配时，词法分析器可以报告一个错误 */
%option nodefault

/* 产生yywrap函数 */
%option noyywrap

/* flex 生成的扫描器用全局变量yylineno 维护着输入文件的当前行编号 */
%option yylineno

/* 区分大小写 */
%option case-sensitive

/* yytext的类型为指针类型，即char * */
%option pointer

/* 生成可重用的扫描器API，这些API用于多线程环境 */
/* %option reentrant */

/* 不进行命令行交互，只能分析文件 */
%option never-interactive

/* 辅助定义式或者宏，后面使用时带上大括号 */

/* 正规式定义 */
%%

"//".*        { /* Eat up single-line comments, including Chinese characters */ }
"/*"([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+"/" { /* Eat up multi-line comments, including Chinese characters */ }

"("         { return T_L_PAREN; }
")"         { return T_R_PAREN; }
"{"         { return T_L_BRACE; }
"}"         { return T_R_BRACE; }
"["         { return T_L_BRACKET; }
"]"         { return T_R_BRACKET; }

";"         { return T_SEMICOLON; }
","         { return T_COMMA; }

"="         { return T_ASSIGN; }
"+"         { return T_ADD; }
"-"         { return T_SUB; }
"*"         { return T_MUL; }
"/"         { return T_DIV; }
"%"         { return T_MOD; }

"if"        { return T_IF; }
"else"      { return T_ELSE; }
"while"     { return T_WHILE; }

"break"     { return T_BREAK; }
"continue"  { return T_CONTINUE; }

"<"         { return T_LT; }
"<="        { return T_LE; }
">"         { return T_GT; }
">="        { return T_GE; }
"=="        { return T_EQ; }
"!="        { return T_NE; }

"&&"        { return T_LOGICAL_AND; }
"||"        { return T_LOGICAL_OR; }
"!"         { return T_LOGICAL_NOT; }

"0"[xX][0-9a-fA-F]+ {
                yylval.integer_num.val = (uint32_t)strtol(yytext, (char **)NULL, 16);
                yylval.integer_num.lineno = yylineno;
                return T_DIGIT;
            }
"0"[1-7][0-7]*    {
                yylval.integer_num.val = (uint32_t)strtol(yytext, (char **)NULL, 8);
                yylval.integer_num.lineno = yylineno;
                return T_DIGIT;
            }
"0"                {
                yylval.integer_num.val = 0;
                yylval.integer_num.lineno = yylineno;
                return T_DIGIT;
            }
[1-9][0-9]*        {
                yylval.integer_num.val = (uint32_t)strtol(yytext, (char **)NULL, 10);
                yylval.integer_num.lineno = yylineno;
                return T_DIGIT;
            }

"int"       {
                // int类型关键字 关键字的识别要在标识符识别的前边，这是因为关键字也是标识符，不过是保留的
                yylval.type.type = BasicType::TYPE_INT;
                yylval.type.lineno = yylineno;
                return T_INT;
            }

"void"      {
                // void类型关键字
                yylval.type.type = BasicType::TYPE_VOID;
                yylval.type.lineno = yylineno;
                return T_VOID;
            }

"return"    {
                // return关键字 关键字的识别要在标识符识别的前边，，这是因为关键字也是标识符，不过是保留的
                return T_RETURN;
            }

[a-zA-Z_]+[0-9a-zA-Z_]* {
                // strdup 分配的空间需要在使用完毕后使用free手动释放，否则会造成内存泄漏
                yylval.var_id.id = strdup(yytext);
                yylval.var_id.lineno = yylineno;
                return T_ID;
            }


[\t\040]+   {
                /* \040代表8进制的32的识别，也就是空格字符 */
                // 空白符号忽略
                ;
            }

[\r\n]+     {
                // 空白行忽略
                ;
            }

.           {
                printf("Line %d: Invalid char %s\n", yylineno, yytext);
                // 词法识别错误
                return 257;
            }
%%
