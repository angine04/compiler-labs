%{
#include <cstdio>
#include <cstring>

// 词法分析头文件
#include "FlexLexer.h"

// bison生成的头文件
#include "BisonParser.h"

// 抽象语法树函数定义原型头文件
#include "AST.h"

#include "IntegerType.h"

// LR分析失败时所调用函数的原型声明
void yyerror(char * msg);

%}

// 联合体声明，用于后续终结符和非终结符号属性指定使用
%union {
    class ast_node * node;

    struct digit_int_attr integer_num;
    struct digit_real_attr float_num;
    struct var_id_attr var_id;
    struct type_attr type;
    int op_class;
};

// 文法的开始符号
%start  CompileUnit

// 指定文法的终结符号，<>可指定文法属性
// 对于单个字符的算符或者分隔符，在词法分析时可直返返回对应的ASCII码值，bison预留了255以内的值
// %token开始的符号称之为终结符，需要词法分析工具如flex识别后返回
// %type开始的符号称之为非终结符，需要通过文法产生式来定义
// %token或%type之后的<>括住的内容成为文法符号的属性，定义在前面的%union中的成员名字。
%token <integer_num> T_DIGIT
%token <var_id> T_ID
%token <type> T_INT T_VOID

// 关键或保留字 一词一类 不需要赋予语义属性
%token T_RETURN

// 分隔符 一词一类 不需要赋予语义属性
%token T_SEMICOLON T_L_PAREN T_R_PAREN T_L_BRACE T_R_BRACE
%token T_L_BRACKET T_R_BRACKET
%token T_COMMA

// 运算符
%token T_ASSIGN T_SUB T_ADD
%token T_MUL T_DIV T_MOD

// New Tokens for control flow, relational and logical operators
%token T_IF T_ELSE T_WHILE
%token T_LT T_LE T_GT T_GE T_EQ T_NE
%token T_LOGICAL_AND T_LOGICAL_OR T_LOGICAL_NOT
// End of New Tokens

// Tokens for break and continue
%token T_BREAK T_CONTINUE

// Virtual token for dangling else resolution
%token T_IF_WITHOUT_ELSE

%left T_LOGICAL_OR
%left T_LOGICAL_AND
%left T_EQ T_NE
%left T_LT T_LE T_GT T_GE
%left T_ADD T_SUB
%left T_MUL T_DIV T_MOD
%right T_UMINUS /* 虚拟Token，用于单目负号的优先级 */
%right T_LOGICAL_NOT /* Logical NOT is unary and right-associative */

// Dangling else resolution: T_ELSE (shift action) should have higher precedence
// than an 'if' without an 'else' (reduce action).
// So, T_IF_WITHOUT_ELSE should have lower precedence.
%right T_IF_WITHOUT_ELSE // Lower precedence for the 'if' rule without an else
%right T_ELSE             // Higher precedence for the T_ELSE token itself

// 非终结符
// %type指定文法的非终结符号，<>可指定文法属性
%type <node> CompileUnit
%type <node> FuncDef
%type <node> Block
%type <node> BlockItemList
%type <node> BlockItem
%type <node> Statement
%type <node> Expr
%type <node> LVal
%type <node> VarDecl VarDeclExpr VarDef
%type <node> AddExp UnaryExp PrimaryExp
%type <node> MulExp /* 新增非终结符 MulExp */
%type <node> RealParamList
%type <node> FormalParamList FormalParam
%type <node> ArrayDimensions ArrayDimension
%type <type> BasicType

// New non-terminals for control flow and new expression types
%type <node> IfStmt
%type <node> WhileStmt
%type <node> RelationalExp
%type <node> EqualityExp
%type <node> LogicalAndExp
%type <node> LogicalOrExp
%type <node> RealParamListOpt // Optional real parameter list for function calls

// New non-terminals for break and continue statements
%type <node> BreakStmt
%type <node> ContinueStmt
%%

// 编译单元可包含若干个函数与全局变量定义。要在语义分析时检查main函数存在
// compileUnit: (funcDef | varDecl)* EOF;
// bison不支持闭包运算，为便于追加修改成左递归方式
// compileUnit: funcDef | varDecl | compileUnit funcDef | compileUnit varDecl
CompileUnit : FuncDef {

		// 创建一个编译单元的节点AST_OP_COMPILE_UNIT
		$$ = create_contain_node(ast_operator_type::AST_OP_COMPILE_UNIT, $1);

		// 设置到全局变量中
		ast_root = $$;
	}
	| VarDecl {

		// 创建一个编译单元的节点AST_OP_COMPILE_UNIT
		$$ = create_contain_node(ast_operator_type::AST_OP_COMPILE_UNIT, $1);
		ast_root = $$;
	}
	| CompileUnit FuncDef {

		// 把函数定义的节点作为编译单元的孩子
		$$ = $1->insert_son_node($2);
	}
	| CompileUnit VarDecl {
		// 把变量定义的节点作为编译单元的孩子
		$$ = $1->insert_son_node($2);
	}
	;

// 函数定义，目前支持整数返回类型，不支持形参
FuncDef : BasicType T_ID T_L_PAREN T_R_PAREN Block  {

		// 函数返回类型
		type_attr funcReturnType = $1;

		// 函数名
		var_id_attr funcId = $2;

		// 函数体节点即Block，即$5
		ast_node * blockNode = $5;

		// 形参结点没有，设置为空指针
		ast_node * formalParamsNode = nullptr;

		// 创建函数定义的节点，孩子有类型，函数名，语句块和形参(实际上无)
		// create_func_def函数内会释放funcId中指向的标识符空间，切记，之后不要再释放，之前一定要是通过strdup函数或者malloc分配的空间
		$$ = create_func_def(funcReturnType, funcId, blockNode, formalParamsNode);
	}
	| BasicType T_ID T_L_PAREN FormalParamList T_R_PAREN Block  {

		// 函数返回类型
		type_attr funcReturnType = $1;

		// 函数名
		var_id_attr funcId = $2;

		// 函数体节点即Block，即$6
		ast_node * blockNode = $6;

		// 形参结点
		ast_node * formalParamsNode = $4;

		// 创建函数定义的节点，孩子有类型，函数名，语句块和形参
		// create_func_def函数内会释放funcId中指向的标识符空间，切记，之后不要再释放，之前一定要是通过strdup函数或者malloc分配的空间
		$$ = create_func_def(funcReturnType, funcId, blockNode, formalParamsNode);
	}
	;

// 语句块的文法Block ： T_L_BRACE BlockItemList? T_R_BRACE
// 其中?代表可有可无，在bison中不支持，需要拆分成两个产生式
// Block ： T_L_BRACE T_R_BRACE | T_L_BRACE BlockItemList T_R_BRACE
Block : T_L_BRACE T_R_BRACE {
		// 语句块没有语句

		// 为了方便创建一个空的Block节点
		$$ = create_contain_node(ast_operator_type::AST_OP_BLOCK);
	}
	| T_L_BRACE BlockItemList T_R_BRACE {
		// 语句块含有语句

		// BlockItemList归约时内部创建Block节点，并把语句加入，这里不创建Block节点
		$$ = $2;
	}
	;

// 语句块内语句列表的文法：BlockItemList : BlockItem+
// Bison不支持正闭包，需修改成左递归形式，便于属性的传递与孩子节点的追加
// 左递归形式的文法为：BlockItemList : BlockItem | BlockItemList BlockItem
BlockItemList : BlockItem {
		// 第一个左侧的孩子节点归约成Block节点，后续语句可持续作为孩子追加到Block节点中
		// 创建一个AST_OP_BLOCK类型的中间节点，孩子为Statement($1)
		$$ = create_contain_node(ast_operator_type::AST_OP_BLOCK, $1);
	}
	| BlockItemList BlockItem {
		// 把BlockItem归约的节点加入到BlockItemList的节点中
		$$ = $1->insert_son_node($2);
	}
	;


// 语句块中子项的文法：BlockItem : Statement
// 目前只支持语句,后续可增加支持变量定义
BlockItem : Statement  {
		// 语句节点传递给归约后的节点上，综合属性
		$$ = $1;
	}
	| VarDecl {
		// 变量声明节点传递给归约后的节点上，综合属性
		$$ = $1;
	}
	;

// 变量声明语句
// 语法：varDecl: basicType varDef (T_COMMA varDef)* T_SEMICOLON
// 因Bison不支持闭包运算符，因此需要修改成左递归，修改后的文法为：
// VarDecl : VarDeclExpr T_SEMICOLON
// VarDeclExpr: BasicType VarDef | VarDeclExpr T_COMMA varDef
VarDecl : VarDeclExpr T_SEMICOLON {
		$$ = $1;
	}
	;

// 变量声明表达式，可支持逗号分隔定义多个
VarDeclExpr: BasicType VarDef {

		// 创建类型节点
		ast_node * type_node = create_type_node($1);

		// 检查VarDef是否是初始化节点或数组声明节点
		ast_node * decl_node;
		if ($2->node_type == ast_operator_type::AST_OP_VAR_INIT) {
			// 如果是初始化节点，直接使用
			decl_node = $2;
			decl_node->type = type_node->type;
		} else if ($2->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
			// 如果是数组声明节点，直接使用
			decl_node = $2;
			decl_node->type = type_node->type;
		} else {
			// 普通变量声明
			decl_node = create_contain_node(ast_operator_type::AST_OP_VAR_DECL, type_node, $2);
			decl_node->type = type_node->type;
		}

		// 创建变量声明语句，并加入第一个变量
		$$ = create_var_decl_stmt_node(decl_node);
	}
	| VarDeclExpr T_COMMA VarDef {

		// 创建类型节点，这里从VarDeclExpr获取类型，前面已经设置
		ast_node * type_node = ast_node::New($1->type);

		// 检查VarDef是否是初始化节点或数组声明节点
		ast_node * decl_node;
		if ($3->node_type == ast_operator_type::AST_OP_VAR_INIT) {
			// 如果是初始化节点，需要正确设置类型
			decl_node = $3;
			// 设置变量类型（从初值表达式获取）
		} else if ($3->node_type == ast_operator_type::AST_OP_ARRAY_DECL) {
			// 如果是数组声明节点，直接使用
			decl_node = $3;
			decl_node->type = type_node->type;
		} else {
			// 普通变量声明
			decl_node = create_contain_node(ast_operator_type::AST_OP_VAR_DECL, type_node, $3);
		}

		// 插入到变量声明语句
		$$ = $1->insert_son_node(decl_node);
	}
	;

// 变量定义包含变量名，支持初值
VarDef : T_ID {
		// 变量ID（无初值）

		$$ = ast_node::New(var_id_attr{$1.id, $1.lineno});

		// 对于字符型字面量的字符串空间需要释放，因词法用到了strdup进行了字符串复制
		free($1.id);
	}
	| T_ID T_ASSIGN Expr {
		// 变量ID（有初值）
		
		// 创建变量ID节点
		ast_node * id_node = ast_node::New(var_id_attr{$1.id, $1.lineno});
		
		// 释放字符串空间
		free($1.id);
		
		// 创建初始化节点，包含变量ID和初值表达式
		$$ = create_contain_node(ast_operator_type::AST_OP_VAR_INIT, id_node, $3);
	}
	| T_ID ArrayDimensions {
		// 数组声明（无初值）
		
		// 创建变量ID节点
		ast_node * id_node = ast_node::New(var_id_attr{$1.id, $1.lineno});
		
		// 释放字符串空间
		free($1.id);
		
		// 创建数组声明节点，包含变量ID和维度信息
		$$ = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, id_node, $2);
	}
	;

// 基本类型，支持整型和void类型
BasicType: T_INT {
		$$ = $1;
	}
	| T_VOID {
		$$ = $1;
	}
	;

// 语句文法：statement:T_RETURN expr T_SEMICOLON | lVal T_ASSIGN expr T_SEMICOLON
// | block | expr? T_SEMICOLON
// 支持返回语句、赋值语句、语句块、表达式语句
// 其中表达式语句可支持空语句，由于bison不支持?，修改成两条
Statement : T_RETURN Expr T_SEMICOLON {
		// 返回语句

		// 创建返回节点AST_OP_RETURN，其孩子为Expr，即$2
		$$ = create_contain_node(ast_operator_type::AST_OP_RETURN, $2);
	}
	| LVal T_ASSIGN Expr T_SEMICOLON {
		// 赋值语句

		// 创建一个AST_OP_ASSIGN类型的中间节点，孩子为LVal($1)和Expr($3)
		$$ = create_contain_node(ast_operator_type::AST_OP_ASSIGN, $1, $3);
	}
	| Block {
		// 语句块

		// 内部已创建block节点，直接传递给Statement
		$$ = $1;
	}
	| Expr T_SEMICOLON {
		// 表达式语句

		// 内部已创建表达式，直接传递给Statement
		$$ = $1;
	}
	| T_SEMICOLON {
		// 空语句
		$$ = nullptr; // Or a specific AST node for empty statement if needed
	}
	| IfStmt {
		$$ = $1;
	}
	| WhileStmt {
		$$ = $1;
	}
	| BreakStmt {
		$$ = $1;
	}
	| ContinueStmt {
		$$ = $1;
	}
	;

// 表达式文法 expr : AddExp
// 表达式目前只支持加法与减法运算，现在扩展到逻辑和关系运算
Expr : LogicalOrExp {
        $$ = $1;
    }
    ;

LogicalOrExp : LogicalAndExp {
        $$ = $1;
    }
    | LogicalOrExp T_LOGICAL_OR LogicalAndExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_LOGICAL_OR, $1, $3);
    }
    ;

LogicalAndExp : EqualityExp {
        $$ = $1;
    }
    | LogicalAndExp T_LOGICAL_AND EqualityExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_LOGICAL_AND, $1, $3);
    }
    ;

EqualityExp : RelationalExp {
        $$ = $1;
    }
    | EqualityExp T_EQ RelationalExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_EQ, $1, $3);
    }
    | EqualityExp T_NE RelationalExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_NE, $1, $3);
    }
    ;

RelationalExp : AddExp {
        $$ = $1;
    }
    | RelationalExp T_LT AddExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_LT, $1, $3);
    }
    | RelationalExp T_LE AddExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_LE, $1, $3);
    }
    | RelationalExp T_GT AddExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_GT, $1, $3);
    }
    | RelationalExp T_GE AddExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_GE, $1, $3);
    }
    ;

// AddExp and MulExp rules that were missing
AddExp : MulExp {
        $$ = $1;
    }
    | AddExp T_ADD MulExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_ADD, $1, $3);
    }
    | AddExp T_SUB MulExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_SUB, $1, $3);
    }
    ;

MulExp : UnaryExp {
        $$ = $1;
    }
    | MulExp T_MUL UnaryExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_MUL, $1, $3);
    }
    | MulExp T_DIV UnaryExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_DIV, $1, $3);
    }
    | MulExp T_MOD UnaryExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_MOD, $1, $3);
    }
    ;

// 目前一元表达式可以为基本表达式、函数调用，其中函数调用的实参可有可无
// 其文法为：unaryExp: primaryExp | T_ID T_L_PAREN realParamList? T_R_PAREN
// 由于bison不支持？表达，因此变更后的文法为：
// unaryExp: primaryExp | T_ID T_L_PAREN T_R_PAREN | T_ID T_L_PAREN realParamList T_R_PAREN
// Update UnaryExp for T_LOGICAL_NOT and use RealParamListOpt
UnaryExp : PrimaryExp {
        $$ = $1;
    }
    | T_SUB UnaryExp %prec T_UMINUS {
        $$ = create_contain_node(ast_operator_type::AST_OP_NEG, $2);
    }
    | T_LOGICAL_NOT UnaryExp {
        $$ = create_contain_node(ast_operator_type::AST_OP_LOGICAL_NOT, $2);
    }
    | T_ID T_L_PAREN RealParamListOpt T_R_PAREN {
        // Function call
        // $1 is var_id_attr (due to %token <var_id> T_ID)
        // $3 is RealParamListOpt (ast_node* which is a list of expressions, or nullptr)
        ast_node * id_node = ast_node::New($1); // Create AST node for the function ID from var_id_attr $1
        free($1.id);
        $$ = create_func_call(id_node, $3);
    }
    ;

// Optional Real Parameter List for function calls
RealParamListOpt : /* empty */ {
        $$ = nullptr; // No parameters
    }
    | RealParamList {
        $$ = $1;
    }
    ;

// 基本表达式支持无符号整型字面量、带括号的表达式、具有左值属性的表达式
// 其文法为：primaryExp: T_L_PAREN expr T_R_PAREN | T_DIGIT | lVal
PrimaryExp :  T_L_PAREN Expr T_R_PAREN {
		// 带有括号的表达式
		$$ = $2;
	}
	| T_DIGIT {
        	// 无符号整型字面量

		// 创建一个无符号整型的终结符节点
		$$ = ast_node::New($1);
	}
	| LVal  {
		// 具有左值的表达式

		// 直接传递到归约后的非终结符号PrimaryExp
		$$ = $1;
	}
	;

// 实参表达式支持逗号分隔的若干个表达式
// 其文法为：realParamList: expr (T_COMMA expr)*
// 由于Bison不支持闭包运算符表达，修改成左递归形式的文法
// 左递归文法为：RealParamList : Expr | 左递归文法为：RealParamList T_COMMA expr
RealParamList : Expr {
		// 创建实参列表节点，并把当前的Expr节点加入
		$$ = create_contain_node(ast_operator_type::AST_OP_FUNC_REAL_PARAMS, $1);
	}
	| RealParamList T_COMMA Expr {
		// 左递归增加实参表达式
		$$ = $1->insert_son_node($3);
	}
	;

// 左值表达式，支持变量名和数组元素访问
LVal : T_ID {
		// 变量名终结符

		// 创建变量名终结符节点
		$$ = ast_node::New($1);

		// 对于字符型字面量的字符串空间需要释放，因词法用到了strdup进行了字符串复制
		free($1.id);
	}
	| T_ID ArrayDimensions {
		// 数组元素访问
		
		// 创建变量名节点
		ast_node * id_node = ast_node::New($1);
		
		// 释放字符串空间
		free($1.id);
		
		// 创建数组访问节点，包含变量名和下标表达式
		$$ = create_contain_node(ast_operator_type::AST_OP_ARRAY_REF, id_node, $2);
	}
	;

// Definition for IfStmt
IfStmt : T_IF T_L_PAREN Expr T_R_PAREN Statement %prec T_IF_WITHOUT_ELSE
           { 
             // If statement without else
             // $3 is Expr (condition), $5 is Statement (then_branch)
             $$ = create_contain_node(ast_operator_type::AST_OP_IF, $3, $5);
           }
       | T_IF T_L_PAREN Expr T_R_PAREN Statement T_ELSE Statement
           { 
             // If statement with else
             // $3 is Expr (condition), $5 is Statement (then_branch), $7 is Statement (else_branch)
             $$ = create_contain_node(ast_operator_type::AST_OP_IF, $3, $5, $7);
           }
       ;

// Definition for WhileStmt
WhileStmt : T_WHILE T_L_PAREN Expr T_R_PAREN Statement
              { 
                // While statement
                // $3 is Expr (condition), $5 is Statement (body)
                $$ = create_contain_node(ast_operator_type::AST_OP_WHILE, $3, $5);
              }
            ;

// Definitions for BreakStmt and ContinueStmt
BreakStmt : T_BREAK T_SEMICOLON
            { 
              $$ = create_contain_node(ast_operator_type::AST_OP_BREAK);
            }
            ;

ContinueStmt : T_CONTINUE T_SEMICOLON
               { 
                 $$ = create_contain_node(ast_operator_type::AST_OP_CONTINUE);
               }
               ;

// 形参列表
FormalParamList: FormalParam {
		// 创建形参列表节点
		$$ = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAMS, $1);
	}
	| FormalParamList T_COMMA FormalParam {
		// 添加形参到列表
		$$ = $1->insert_son_node($3);
	}
	;

// 单个形参
FormalParam: BasicType T_ID {
		// 创建类型节点
		ast_node * type_node = create_type_node($1);
		
		// 创建变量ID节点
		ast_node * id_node = ast_node::New($2);
		
		// 释放字符串空间
		free($2.id);
		
		// 创建形参节点
		$$ = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAM, type_node, id_node);
	}
	| BasicType T_ID ArrayDimensions {
		// 数组形参
		
		// 创建类型节点
		ast_node * type_node = create_type_node($1);
		
		// 创建变量ID节点
		ast_node * id_node = ast_node::New($2);
		
		// 释放字符串空间
		free($2.id);
		
		// 创建数组声明节点
		ast_node * array_decl = create_contain_node(ast_operator_type::AST_OP_ARRAY_DECL, id_node, $3);
		
		// 创建形参节点
		$$ = create_contain_node(ast_operator_type::AST_OP_FUNC_FORMAL_PARAM, type_node, array_decl);
	}
	;

// 数组维度列表
ArrayDimensions: ArrayDimension {
		// 创建数组维度列表节点
		$$ = create_contain_node(ast_operator_type::AST_OP_ARRAY_DIM, $1);
	}
	| ArrayDimensions ArrayDimension {
		// 添加维度到列表
		$$ = $1->insert_son_node($2);
	}
	;

// 单个数组维度
ArrayDimension: T_L_BRACKET Expr T_R_BRACKET {
		// 数组维度，包含表达式
		$$ = $2;
	}
	;

%%

// 语法识别错误要调用函数的定义
void yyerror(char * msg)
{
    printf("Line %d: %s\n", yylineno, msg);
}
