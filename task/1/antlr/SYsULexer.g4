lexer grammar SYsULexer;

Int : 'int';
Return : 'return';
Const : 'const';
If : 'if';
Else : 'else';
While : 'while';
For: 'for';
Void: 'void';
Break: 'break';
Continue: 'continue';

LeftParen : '(';
RightParen : ')';
LeftBracket : '[';
RightBracket : ']';
LeftBrace : '{';
RightBrace : '}';

Plus : '+';
Minus : '-';
Star : '*';
Slash : '/';
Percent : '%';
Greater : '>';
Less : '<';
Ampamp : '&&';
Pipepipe : '||';
Equalequal : '==';
Lessequal: '<=';
Greaterequal: '>=';
Exclaimequal: '!=';
Exclaim: '!';

Semi : ';';
Comma : ',';

Equal : '=';

Identifier
    :   IdentifierNondigit
        (   IdentifierNondigit
        |   Digit
        )*
    ;

fragment
IdentifierNondigit
    :   Nondigit
    ;

fragment
Nondigit
    :   [a-zA-Z_]
    ;

fragment
Digit
    :   [0-9]
    ;

Constant
    :   IntegerConstant
    ;

fragment
IntegerConstant
    :   DecimalConstant
    |   OctalConstant
    |   HexConstant
    ;

fragment
DecimalConstant
    :   NonzeroDigit Digit*
    ;

fragment
OctalConstant
    :   '0' OctalDigit*
    ;

fragment
HexConstant
    :   ('0x' | '0X') HexDigit*
    ;

fragment
NonzeroDigit
    :   [1-9]
    ;

fragment
OctalDigit
    :   [0-7]
    ;

fragment
HexDigit
    :   [0-9a-fA-F]
    ;


// 预处理信息处理，可以从预处理信息中获得文件名以及行号
// 预处理信息中的第一个数字即为行号
FileMetaData
    : '#' ~[\r\n]* ('.c"' | '.h"') ~[\r\n]* Newline
    ;

LineAfterPreprocessing
    :   '#' Whitespace* ~[\r\n]* Newline
        -> skip
    ;

Whitespace
    :   [ \t]+
    ;

// 换行符号，可以利用这个信息来更新行号
Newline
    :   (   '\r' '\n'?
        |   '\n'
        )
    ;

// 注释
SingleLineComment
    :   '//' ~[\r\n]*
        -> skip
    ;

MultiLineComment
    :   '/*' .*? '*/'
    ;
