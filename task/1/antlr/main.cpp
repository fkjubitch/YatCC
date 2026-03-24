#include "SYsULexer.h" // 确保这里的头文件名与您生成的词法分析器匹配
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_map>

// 全局定义
int curr_line = 1; // 当前行号
int curr_col = 1; // 当前列号
std::string filename; // 文件名
bool startOfLine = true;
bool leadingSpace = false;

// 保存最后一个有效token之后的位置（用于EOF）
int last_token_end_line = 1;
int last_token_end_col = 1;

// 映射定义，将ANTLR的tokenTypeName映射到clang的格式
std::unordered_map<std::string, std::string> tokenTypeMapping = {
  { "Int", "int" },
  { "Identifier", "identifier" },
  { "LeftParen", "l_paren" },
  { "RightParen", "r_paren" },
  { "RightBrace", "r_brace" },
  { "LeftBrace", "l_brace" },
  { "LeftBracket", "l_square" },
  { "RightBracket", "r_square" },
  { "Constant", "numeric_constant" },
  { "Return", "return" },
  { "Semi", "semi" },
  { "EOF", "eof" },
  { "Equal", "equal" },
  { "Plus", "plus" },
  { "Comma", "comma" },

  // 在这里继续添加其他映射
  { "Minus", "minus" },
  { "Const", "const" },
  { "Star", "star" },
  { "Slash", "slash" },
  { "Percent", "percent" },
  { "If", "if" },
  { "Else", "else" },
  { "While", "while" },
  { "For", "for" },
  { "Void", "void" }, 
  { "Break", "break" },
  { "Continue", "continue" },
  { "Greater", "greater" },
  { "Less", "less" },
  { "Ampamp", "ampamp" },
  { "Pipepipe", "pipepipe" },
  { "Equalequal", "equalequal" },
  { "Lessequal", "lessequal" },
  { "Greaterequal", "greaterequal" },
  { "Exclaimequal", "exclaimequal" },
  { "Exclaim", "exclaim" },
};


// 提取开始行号与文件名, 形如 # 1 "./basic/000_main.sysu.c"
void
fetch_info(const std::string info_str)
{
  /* flag:0 #号部分
     flag:1 行号部分
     flag:2 文件名部分
     flag:-1 尾端不需要的部分
  */
  int flag = 0; 
  std::string line_str;
  std::string curr_filename;
  for (const char ch : info_str)
  {
    switch (flag)
    {
      case 0: {
        if (ch != '#' && !std::isspace(ch)) {line_str += ch; flag = 1;}
        break;
      }
      case 1: {
        if (std::isdigit(ch)) {line_str += ch;}
        else if (!std::isdigit(ch) && !std::isspace(ch)) {curr_line = std::stoi(line_str); flag = 2;} // 掠过引号
        break;
      }
      case 2: {
        if (ch != '"') {curr_filename += ch;}
        else {flag = -1;}
        break;
      }
    }
  }
  filename = curr_filename;
}

void
print_token(const antlr4::Token* token,
            const antlr4::CommonTokenStream& tokens,
            std::ofstream& outFile,
            const antlr4::Lexer& lexer)
{
  auto& vocabulary = lexer.getVocabulary();

  auto tokenTypeName =
    std::string(vocabulary.getSymbolicName(token->getType()));

  if (tokenTypeName.empty())
    tokenTypeName = "<UNKNOWN>"; // 处理可能的空字符串情况
  else if (tokenTypeName == "FileMetaData")
  {
    fetch_info(token->getText());
    return;
  }
  else if (tokenTypeName == "Newline")
  {
    curr_line ++;
    curr_col = 1;
    startOfLine = true;
    leadingSpace = false;
    return;
  }
  else if (tokenTypeName == "Whitespace")
  {
    leadingSpace = true;
    curr_col += token->getText().length();
    return;
  }
  else if (tokenTypeName == "MultiLineComment")
  {
    // 计算多行注释中的换行符数量
    std::string text = token->getText();
    for (char c : text) {
      if (c == '\n') {
        curr_line++;
        curr_col = 1;
      } else {
        curr_col++;
      }
    }
    return;
  }

  if (tokenTypeMapping.find(tokenTypeName) != tokenTypeMapping.end()) {
    tokenTypeName = tokenTypeMapping[tokenTypeName];
  }

  if (tokenTypeName != "eof")
    outFile << tokenTypeName << " '" << token->getText() << "'" << "\t";
  else
    outFile << tokenTypeName << " '" << "'" << "\t";

  
  if (tokenTypeName != "eof") {
    if (startOfLine)
    {
      outFile << " [StartOfLine]";
      startOfLine = false;
    }
    if (leadingSpace)
    {
      outFile << " [LeadingSpace]";
      leadingSpace = false;
    }
  }

  std::string locInfo;
  if (tokenTypeName == "eof") {
    // 对于EOF，使用最后一个有效token之后的位置
    locInfo = "\tLoc=<" + filename + ":" + std::to_string(last_token_end_line) + ":" + std::to_string(last_token_end_col) + ">";
  } else {
    locInfo = "\tLoc=<" + filename + ":" + std::to_string(curr_line) + ":" + std::to_string(curr_col) + ">";
    // 更新最后一个有效token之后的位置
    last_token_end_line = curr_line;
    last_token_end_col = curr_col + token->getText().length();
  }
  outFile << locInfo << std::endl;

  if (tokenTypeName != "eof") {
    curr_col += token->getText().length();
  }
}

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <input> <output>\n";
    return -1;
  }

  std::ifstream inFile(argv[1]);
  if (!inFile) {
    std::cout << "Error: unable to open input file: " << argv[1] << '\n';
    return -2;
  }

  std::ofstream outFile(argv[2]);
  if (!outFile) {
    std::cout << "Error: unable to open output file: " << argv[2] << '\n';
    return -3;
  }

  std::cout << "程序 '" << argv[0] << std::endl;
  std::cout << "输入 '" << argv[1] << std::endl;
  std::cout << "输出 '" << argv[2] << std::endl;

  antlr4::ANTLRInputStream input(inFile);
  SYsULexer lexer(&input);

  antlr4::CommonTokenStream tokens(&lexer);
  tokens.fill();

  for (auto&& token : tokens.getTokens()) {
    print_token(token, tokens, outFile, lexer);
  }
}
