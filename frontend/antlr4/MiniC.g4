grammar MiniC;

// 词法规则名总是以大写字母开头

// 语法规则名总是以小写字母开头

// 每个非终结符尽量多包含闭包、正闭包或可选符等的EBNF范式描述

// 若非终结符由多个产生式组成，则建议在每个产生式的尾部追加# 名称来区分，详细可查看非终结符statement的描述

// 语法规则描述：EBNF范式

// 源文件编译单元定义
compileUnit: (funcDef | varDecl)* EOF;

// 函数定义，支持形参和void返回类型
funcDef: (T_INT | T_VOID) T_ID T_L_PAREN formalParamList? T_R_PAREN block;

// 形参列表
formalParamList: formalParam (T_COMMA formalParam)*;

// 形参定义
formalParam: basicType T_ID;

// 语句块看用作函数体，这里允许多个语句，并且不含任何语句
block: T_L_BRACE blockItemList? T_R_BRACE;

// 每个ItemList可包含至少一个Item
blockItemList: blockItem+;

// 每个Item可以是一个语句，或者变量声明语句
blockItem: statement | varDecl;

// 变量声明，支持数组声明和初始化
varDecl: basicType varDef (T_COMMA varDef)* T_SEMICOLON;

// 基本类型
basicType: T_INT;

// 变量定义，支持初始化和数组
varDef: T_ID (T_L_BRACKET T_DEC_LITERAL T_R_BRACKET)* (T_ASSIGN expr)?;

// 目前语句支持return、赋值、控制流语句
statement:
	T_RETURN expr T_SEMICOLON			# returnStatement
	| lVal T_ASSIGN expr T_SEMICOLON	# assignStatement
	| block								# blockStatement
	| expr? T_SEMICOLON					# expressionStatement
	| T_IF T_L_PAREN expr T_R_PAREN statement (T_ELSE statement)?  # ifStatement
	| T_WHILE T_L_PAREN expr T_R_PAREN statement					# whileStatement
	| T_BREAK T_SEMICOLON											# breakStatement
	| T_CONTINUE T_SEMICOLON										# continueStatement;

// 表达式文法 - 按优先级从低到高
expr:
	logicalOrExpr; 

logicalOrExpr:
	logicalAndExpr								# passToLogicalAndExpr
	| logicalOrExpr T_LOGICAL_OR logicalAndExpr	# logicalOrExpr;

logicalAndExpr:
	equalityExpr								# passToEqualityExpr
	| logicalAndExpr T_LOGICAL_AND equalityExpr	# logicalAndExpr;

equalityExpr:
	relationalExpr								# passToRelationalExpr
	| equalityExpr (T_EQ | T_NE) relationalExpr # equalityOpExpr;

relationalExpr:
	addExpr										# passToAddExpr
	| relationalExpr (T_LT | T_LE | T_GT | T_GE) addExpr	# relationalOpExpr;

addExpr:
	mulExpr								# passToMulExpr
	| addExpr (T_ADD | T_SUB) mulExpr	# addSubExpr;

mulExpr:
	unaryExpr									# passToUnaryExpr
	| mulExpr (T_MUL | T_DIV | T_MOD) unaryExpr	# mulDivModExpr;

unaryExpr:
	T_SUB unaryExpr		# negationExpr
	| T_LOGICAL_NOT unaryExpr	# logicalNotExpr
	| primaryExpr		# passToPrimaryExpr;

primaryExpr:
	T_L_PAREN expr T_R_PAREN					# parenthesizedExpr
	| integerLiteral							# integerAtom
	| lVal										# lValAtom
	| T_ID T_L_PAREN realParamList? T_R_PAREN	# functionCallAtom;

integerLiteral:
	T_HEX_LITERAL
	| T_OCT_LITERAL
	| T_DEC_LITERAL;

// 实参列表
realParamList: expr (T_COMMA expr)*;

// 左值表达式，支持数组访问
lVal: T_ID (T_L_BRACKET expr T_R_BRACKET)*;

// 用正规式来进行词法规则的描述

T_L_PAREN: '(';
T_R_PAREN: ')';
T_SEMICOLON: ';';
T_L_BRACE: '{';
T_R_BRACE: '}';
T_L_BRACKET: '[';
T_R_BRACKET: ']';

T_ASSIGN: '=';
T_COMMA: ',';

T_ADD: '+';
T_SUB: '-';
T_MUL: '*';
T_DIV: '/';
T_MOD: '%';

// 比较运算符
T_LT: '<';
T_LE: '<=';
T_GT: '>';
T_GE: '>=';
T_EQ: '==';
T_NE: '!=';

// 逻辑运算符
T_LOGICAL_AND: '&&';
T_LOGICAL_OR: '||';
T_LOGICAL_NOT: '!';

// 控制流关键字
T_IF: 'if';
T_ELSE: 'else';
T_WHILE: 'while';
T_BREAK: 'break';
T_CONTINUE: 'continue';

// 要注意关键字同样也属于T_ID，因此必须放在T_ID的前面，否则会识别成T_ID
T_RETURN: 'return';
T_INT: 'int';
T_VOID: 'void';

T_ID: [a-zA-Z_][a-zA-Z0-9_]*;

// Integer Literals Order is important: Hex, then Octal, then Decimal to avoid ambiguity e.g., "0"
// should be Decimal, not a prefix of an Octal. "012" is Octal, "12" is Decimal, "0x12" is Hex.
T_HEX_LITERAL: '0' [xX] [0-9a-fA-F]+;
T_OCT_LITERAL:
	'0' [0-7]+; // Ensures it's at least '0' followed by one octal digit
T_DEC_LITERAL:
	[1-9] [0-9]*
	| '0'; // Decimal numbers, or a single '0'

// 注释支持
SINGLE_LINE_COMMENT: '//' ~[\r\n]* -> skip;
MULTI_LINE_COMMENT: '/*' .*? '*/' -> skip;

/* 空白符丢弃 */
WS: [ \r\n\t]+ -> skip;