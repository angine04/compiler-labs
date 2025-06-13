///
/// @file RecursiveDescentFlex.cpp
/// @brief 词法分析的手动实现源文件
/// @author zenglj (zenglj@live.com)
/// @version 1.1
/// @date 2024-11-23
///
/// @copyright Copyright (c) 2024
///
/// @par 修改日志:
/// <table>
/// <tr><th>Date       <th>Version <th>Author  <th>Description
/// <tr><td>2024-11-21 <td>1.0     <td>zenglj  <td>新做
/// <tr><td>2024-11-23 <td>1.1     <td>zenglj  <td>表达式版增强
/// </table>
///
#include <cctype>
#include <cstdio>
#include <cstring>
#include <string>

#include "RecursiveDescentParser.h"
#include "Common.h"

/// @brief 词法分析的行号信息
int64_t rd_line_no = 1;

/// @brief 词法分析的token对应的字符识别
std::string tokenValue;

/// @brief 输入源文件指针
FILE * rd_filein;

/// @brief 关键字与Token类别的数据结构
struct KeywordToken {
    std::string name;
    enum RDTokenType type;
};

/// @brief  关键字与Token对应表
static KeywordToken allKeywords[] = {
    {"int", RDTokenType::T_INT},
    {"void", RDTokenType::T_VOID},
    {"return", RDTokenType::T_RETURN},
    {"if", RDTokenType::T_IF},
    {"else", RDTokenType::T_ELSE},
    {"while", RDTokenType::T_WHILE},
    {"break", RDTokenType::T_BREAK},
    {"continue", RDTokenType::T_CONTINUE},
};

/// @brief 在标识符中检查是否时关键字，若是关键字则返回对应关键字的Token，否则返回T_ID
/// @param id 标识符
/// @return Token
static RDTokenType getKeywordToken(std::string id)
{
    //如果在allkeywords中找到，则说明为关键字
    for (auto & keyword: allKeywords) {
        if (keyword.name == id) {
            return keyword.type;
        }
    }
    // 如果不再allkeywords中，说明是标识符
    return RDTokenType::T_ID;
}

/// @brief 词法文法，获取下一个Token
/// @return  Token，值保存在rd_lval中
int rd_flex()
{
    int c;                              // 扫描的字符
    int tokenKind = RDTokenType::T_ERR; // Initialize to error, ensure it gets set.
    tokenValue = "";                    // Clear previous token value text for debugging/logging

    // 忽略空白符号，主要有空格，TAB键和换行符
    while (true) {
        c = fgetc(rd_filein);
        if (c == ' ' || c == '\t') {
            continue;
        } else if (c == '\n') {
            rd_line_no++;
            continue;
        } else if (c == '\r') {
            rd_line_no++;
            int next_c = fgetc(rd_filein);
            if (next_c != '\n') {
                ungetc(next_c, rd_filein);
            }
            continue;
        }
        break; // Not a whitespace character
    }

    // 文件结束符
    if (c == EOF) {
        return RDTokenType::T_EOF;
    }

    // TODO: Implement comment skipping (single-line // and multi-line /* */)
    // For now, we assume no comments or they will cause errors.

    rd_lval.integer_num.lineno = rd_line_no; // Set line number for literals/IDs early
    rd_lval.var_id.lineno = rd_line_no;      // (though specific literal parsing will re-confirm)

    // 处理数字 (Hex, Octal, Decimal)
    if (isdigit(c)) {
        std::string num_str;
        num_str += (char) c;

        if (c == '0') {
            int next_c = fgetc(rd_filein);
            if (next_c == 'x' || next_c == 'X') { // Hexadecimal
                num_str += (char) next_c;
                tokenKind = RDTokenType::T_HEX_LITERAL;
                while (isxdigit(c = fgetc(rd_filein))) {
                    num_str += (char) c;
                }
                ungetc(c, rd_filein);        // Put back non-hex digit
                if (num_str.length() <= 2) { // e.g., "0x" without digits
                    tokenKind = RDTokenType::T_ERR;
                    fprintf(stderr,
                            "Line(%lld): Malformed hexadecimal literal %s\n",
                            (long long) rd_line_no,
                            num_str.c_str());
                } else {
                    rd_lval.integer_num.val = static_cast<uint32_t>(std::stoul(num_str.substr(2), nullptr, 16));
                }
            } else if (next_c >= '0' && next_c <= '7') { // Octal (must have at least one octal digit after 0)
                ungetc(next_c, rd_filein);               // Put back the first octal digit to be read by the loop
                tokenKind = RDTokenType::T_OCT_LITERAL;
                // num_str already contains '0'
                while (true) {
                    c = fgetc(rd_filein);
                    if (c >= '0' && c <= '7') {
                        num_str += (char) c;
                    } else {
                        ungetc(c, rd_filein); // Put back non-octal digit
                        break;
                    }
                }
                // num_str will be like "0", "01", "012" etc.
                // stoul with base 8 handles this. If num_str is just "0", it's 0.
                rd_lval.integer_num.val = static_cast<uint32_t>(std::stoul(num_str, nullptr, 8));
            } else {                       // Decimal '0'
                ungetc(next_c, rd_filein); // Put back whatever char it was
                tokenKind = RDTokenType::T_DEC_LITERAL;
                rd_lval.integer_num.val = 0;
                // num_str is just "0"
            }
        } else { // Decimal (starts with 1-9)
            tokenKind = RDTokenType::T_DEC_LITERAL;
            while (isdigit(c = fgetc(rd_filein))) {
                num_str += (char) c;
            }
            ungetc(c, rd_filein); // Put back non-digit
            rd_lval.integer_num.val = static_cast<uint32_t>(std::stoul(num_str, nullptr, 10));
        }
        tokenValue = num_str; // Store the original string for debugging
    } else if (c == '(') {
        tokenKind = RDTokenType::T_L_PAREN;
        tokenValue = "(";
    } else if (c == ')') {
        tokenKind = RDTokenType::T_R_PAREN;
        tokenValue = ")";
    } else if (c == '{') {
        tokenKind = RDTokenType::T_L_BRACE;
        tokenValue = "{";
    } else if (c == '}') {
        tokenKind = RDTokenType::T_R_BRACE;
        tokenValue = "}";
    } else if (c == '[') {
        tokenKind = RDTokenType::T_L_BRACKET;
        tokenValue = "[";
    } else if (c == ']') {
        tokenKind = RDTokenType::T_R_BRACKET;
        tokenValue = "]";
    } else if (c == ';') {
        tokenKind = RDTokenType::T_SEMICOLON;
        tokenValue = ";";
    } else if (c == '+') {
        tokenKind = RDTokenType::T_ADD;
        tokenValue = "+";
    } else if (c == '-') {
        tokenKind = RDTokenType::T_SUB;
        tokenValue = "-";
    } else if (c == '*') {
        tokenKind = RDTokenType::T_MUL;
        tokenValue = "*";
    } else if (c == '/') {
        // 检查是否为注释
        int next_c = fgetc(rd_filein);
        if (next_c == '/') {
            // 单行注释，读到行末
            tokenValue = "//";
            while ((c = fgetc(rd_filein)) != '\n' && c != EOF) {
                tokenValue += (char)c;
            }
            if (c == '\n') {
                rd_line_no++;
            }
            ungetc(c, rd_filein);
            // 跳过注释，重新获取下一个token
            return rd_flex();
        } else if (next_c == '*') {
            // 多行注释
            tokenValue = "/*";
            bool found_end = false;
            while (!found_end) {
                c = fgetc(rd_filein);
                if (c == EOF) {
                    fprintf(stderr, "Line(%lld): Unterminated comment\n", (long long)rd_line_no);
                    tokenKind = RDTokenType::T_ERR;
                    return tokenKind;
                }
                tokenValue += (char)c;
                if (c == '\n') {
                    rd_line_no++;
                } else if (c == '*') {
                    int after_star = fgetc(rd_filein);
                    if (after_star == '/') {
                        tokenValue += '/';
                        found_end = true;
                    } else {
                        ungetc(after_star, rd_filein);
                    }
                }
            }
            // 跳过注释，重新获取下一个token
            return rd_flex();
        } else {
            ungetc(next_c, rd_filein);
            tokenKind = RDTokenType::T_DIV;
            tokenValue = "/";
        }
    } else if (c == '%') {
        tokenKind = RDTokenType::T_MOD;
        tokenValue = "%";
    } else if (c == '=') {
        // 检查是否为 ==
        int next_c = fgetc(rd_filein);
        if (next_c == '=') {
            tokenKind = RDTokenType::T_EQ;
            tokenValue = "==";
        } else {
            ungetc(next_c, rd_filein);
            tokenKind = RDTokenType::T_ASSIGN;
            tokenValue = "=";
        }
    } else if (c == '<') {
        // 检查是否为 <=
        int next_c = fgetc(rd_filein);
        if (next_c == '=') {
            tokenKind = RDTokenType::T_LE;
            tokenValue = "<=";
        } else {
            ungetc(next_c, rd_filein);
            tokenKind = RDTokenType::T_LT;
            tokenValue = "<";
        }
    } else if (c == '>') {
        // 检查是否为 >=
        int next_c = fgetc(rd_filein);
        if (next_c == '=') {
            tokenKind = RDTokenType::T_GE;
            tokenValue = ">=";
        } else {
            ungetc(next_c, rd_filein);
            tokenKind = RDTokenType::T_GT;
            tokenValue = ">";
        }
    } else if (c == '!') {
        // 检查是否为 !=
        int next_c = fgetc(rd_filein);
        if (next_c == '=') {
            tokenKind = RDTokenType::T_NE;
            tokenValue = "!=";
        } else {
            ungetc(next_c, rd_filein);
            tokenKind = RDTokenType::T_LOGICAL_NOT;
            tokenValue = "!";
        }
    } else if (c == '&') {
        // 检查是否为 &&
        int next_c = fgetc(rd_filein);
        if (next_c == '&') {
            tokenKind = RDTokenType::T_LOGICAL_AND;
            tokenValue = "&&";
        } else {
            ungetc(next_c, rd_filein);
            fprintf(stderr, "Line(%lld): Invalid character '&'\n", (long long)rd_line_no);
            tokenKind = RDTokenType::T_ERR;
        }
    } else if (c == '|') {
        // 检查是否为 ||
        int next_c = fgetc(rd_filein);
        if (next_c == '|') {
            tokenKind = RDTokenType::T_LOGICAL_OR;
            tokenValue = "||";
        } else {
            ungetc(next_c, rd_filein);
            fprintf(stderr, "Line(%lld): Invalid character '|'\n", (long long)rd_line_no);
            tokenKind = RDTokenType::T_ERR;
        }
    } else if (c == ',') {
        tokenKind = RDTokenType::T_COMMA;
        tokenValue = ",";
    } else if (isLetterUnderLine(c)) { // Assuming isLetterUnderLine and isLetterDigitalUnderLine are defined elsewhere
        std::string name_str;
        do {
            name_str += (char) c;
            c = fgetc(rd_filein);
        } while (isLetterDigitalUnderLine(c));
        ungetc(c, rd_filein);

        tokenValue = name_str;
        tokenKind = getKeywordToken(name_str);

        if (tokenKind == RDTokenType::T_ID) {
            rd_lval.var_id.id = strdup(name_str.c_str());
            // rd_lval.var_id.lineno = rd_line_no; // Already set at the beginning of the function for all potential
            // tokens
        } else if (tokenKind == RDTokenType::T_INT || tokenKind == RDTokenType::T_RETURN) {
            // For keywords, rd_lval might not be used by parser, or could store type info if needed
            // e.g., if T_INT needed to pass type info via rd_lval.type
            rd_lval.type.lineno = rd_line_no;
            if (tokenKind == RDTokenType::T_INT)
                rd_lval.type.type = BasicType::TYPE_INT;
            // else for T_RETURN, rd_lval.type might not be relevant or could be set to TYPE_NONE
        }
    } else {
        fprintf(stderr, "Line(%lld): Invalid character '%c'\n", (long long) rd_line_no, (char) c);
        tokenKind = RDTokenType::T_ERR;
    }
    return tokenKind;
}
