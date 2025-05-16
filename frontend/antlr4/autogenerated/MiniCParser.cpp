
// Generated from /Users/zheyu/Repositories/exp04-minic-expr/frontend/antlr4/MiniC.g4 by ANTLR 4.12.0


#include "MiniCVisitor.h"

#include "MiniCParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct MiniCParserStaticData final {
  MiniCParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MiniCParserStaticData(const MiniCParserStaticData&) = delete;
  MiniCParserStaticData(MiniCParserStaticData&&) = delete;
  MiniCParserStaticData& operator=(const MiniCParserStaticData&) = delete;
  MiniCParserStaticData& operator=(MiniCParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag minicParserOnceFlag;
MiniCParserStaticData *minicParserStaticData = nullptr;

void minicParserInitialize() {
  assert(minicParserStaticData == nullptr);
  auto staticData = std::make_unique<MiniCParserStaticData>(
    std::vector<std::string>{
      "compileUnit", "funcDef", "block", "blockItemList", "blockItem", "varDecl", 
      "basicType", "varDef", "statement", "expr", "addExpr", "mulExpr", 
      "unaryExpr", "primaryExpr", "integerLiteral", "realParamList", "lVal"
    },
    std::vector<std::string>{
      "", "'('", "')'", "';'", "'{'", "'}'", "'='", "','", "'+'", "'-'", 
      "'*'", "'/'", "'%'", "'return'", "'int'"
    },
    std::vector<std::string>{
      "", "T_L_PAREN", "T_R_PAREN", "T_SEMICOLON", "T_L_BRACE", "T_R_BRACE", 
      "T_ASSIGN", "T_COMMA", "T_ADD", "T_SUB", "T_MUL", "T_DIV", "T_MOD", 
      "T_RETURN", "T_INT", "T_ID", "T_HEX_LITERAL", "T_OCT_LITERAL", "T_DEC_LITERAL", 
      "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,19,151,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,1,0,1,0,5,0,37,8,0,10,0,12,0,40,9,0,1,0,1,0,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,3,2,52,8,2,1,2,1,2,1,3,4,3,57,8,3,11,3,
  	12,3,58,1,4,1,4,3,4,63,8,4,1,5,1,5,1,5,1,5,5,5,69,8,5,10,5,12,5,72,9,
  	5,1,5,1,5,1,6,1,6,1,7,1,7,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,1,8,
  	3,8,91,8,8,1,8,3,8,94,8,8,1,9,1,9,1,10,1,10,1,10,1,10,1,10,1,10,5,10,
  	104,8,10,10,10,12,10,107,9,10,1,11,1,11,1,11,1,11,1,11,1,11,5,11,115,
  	8,11,10,11,12,11,118,9,11,1,12,1,12,1,12,3,12,123,8,12,1,13,1,13,1,13,
  	1,13,1,13,1,13,1,13,1,13,1,13,3,13,134,8,13,1,13,3,13,137,8,13,1,14,1,
  	14,1,15,1,15,1,15,5,15,144,8,15,10,15,12,15,147,9,15,1,16,1,16,1,16,0,
  	2,20,22,17,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,0,3,1,0,8,9,
  	1,0,10,12,1,0,16,18,151,0,38,1,0,0,0,2,43,1,0,0,0,4,49,1,0,0,0,6,56,1,
  	0,0,0,8,62,1,0,0,0,10,64,1,0,0,0,12,75,1,0,0,0,14,77,1,0,0,0,16,93,1,
  	0,0,0,18,95,1,0,0,0,20,97,1,0,0,0,22,108,1,0,0,0,24,122,1,0,0,0,26,136,
  	1,0,0,0,28,138,1,0,0,0,30,140,1,0,0,0,32,148,1,0,0,0,34,37,3,2,1,0,35,
  	37,3,10,5,0,36,34,1,0,0,0,36,35,1,0,0,0,37,40,1,0,0,0,38,36,1,0,0,0,38,
  	39,1,0,0,0,39,41,1,0,0,0,40,38,1,0,0,0,41,42,5,0,0,1,42,1,1,0,0,0,43,
  	44,5,14,0,0,44,45,5,15,0,0,45,46,5,1,0,0,46,47,5,2,0,0,47,48,3,4,2,0,
  	48,3,1,0,0,0,49,51,5,4,0,0,50,52,3,6,3,0,51,50,1,0,0,0,51,52,1,0,0,0,
  	52,53,1,0,0,0,53,54,5,5,0,0,54,5,1,0,0,0,55,57,3,8,4,0,56,55,1,0,0,0,
  	57,58,1,0,0,0,58,56,1,0,0,0,58,59,1,0,0,0,59,7,1,0,0,0,60,63,3,16,8,0,
  	61,63,3,10,5,0,62,60,1,0,0,0,62,61,1,0,0,0,63,9,1,0,0,0,64,65,3,12,6,
  	0,65,70,3,14,7,0,66,67,5,7,0,0,67,69,3,14,7,0,68,66,1,0,0,0,69,72,1,0,
  	0,0,70,68,1,0,0,0,70,71,1,0,0,0,71,73,1,0,0,0,72,70,1,0,0,0,73,74,5,3,
  	0,0,74,11,1,0,0,0,75,76,5,14,0,0,76,13,1,0,0,0,77,78,5,15,0,0,78,15,1,
  	0,0,0,79,80,5,13,0,0,80,81,3,18,9,0,81,82,5,3,0,0,82,94,1,0,0,0,83,84,
  	3,32,16,0,84,85,5,6,0,0,85,86,3,18,9,0,86,87,5,3,0,0,87,94,1,0,0,0,88,
  	94,3,4,2,0,89,91,3,18,9,0,90,89,1,0,0,0,90,91,1,0,0,0,91,92,1,0,0,0,92,
  	94,5,3,0,0,93,79,1,0,0,0,93,83,1,0,0,0,93,88,1,0,0,0,93,90,1,0,0,0,94,
  	17,1,0,0,0,95,96,3,20,10,0,96,19,1,0,0,0,97,98,6,10,-1,0,98,99,3,22,11,
  	0,99,105,1,0,0,0,100,101,10,1,0,0,101,102,7,0,0,0,102,104,3,22,11,0,103,
  	100,1,0,0,0,104,107,1,0,0,0,105,103,1,0,0,0,105,106,1,0,0,0,106,21,1,
  	0,0,0,107,105,1,0,0,0,108,109,6,11,-1,0,109,110,3,24,12,0,110,116,1,0,
  	0,0,111,112,10,1,0,0,112,113,7,1,0,0,113,115,3,24,12,0,114,111,1,0,0,
  	0,115,118,1,0,0,0,116,114,1,0,0,0,116,117,1,0,0,0,117,23,1,0,0,0,118,
  	116,1,0,0,0,119,120,5,9,0,0,120,123,3,24,12,0,121,123,3,26,13,0,122,119,
  	1,0,0,0,122,121,1,0,0,0,123,25,1,0,0,0,124,125,5,1,0,0,125,126,3,18,9,
  	0,126,127,5,2,0,0,127,137,1,0,0,0,128,137,3,28,14,0,129,137,3,32,16,0,
  	130,131,5,15,0,0,131,133,5,1,0,0,132,134,3,30,15,0,133,132,1,0,0,0,133,
  	134,1,0,0,0,134,135,1,0,0,0,135,137,5,2,0,0,136,124,1,0,0,0,136,128,1,
  	0,0,0,136,129,1,0,0,0,136,130,1,0,0,0,137,27,1,0,0,0,138,139,7,2,0,0,
  	139,29,1,0,0,0,140,145,3,18,9,0,141,142,5,7,0,0,142,144,3,18,9,0,143,
  	141,1,0,0,0,144,147,1,0,0,0,145,143,1,0,0,0,145,146,1,0,0,0,146,31,1,
  	0,0,0,147,145,1,0,0,0,148,149,5,15,0,0,149,33,1,0,0,0,14,36,38,51,58,
  	62,70,90,93,105,116,122,133,136,145
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  minicParserStaticData = staticData.release();
}

}

MiniCParser::MiniCParser(TokenStream *input) : MiniCParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

MiniCParser::MiniCParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  MiniCParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *minicParserStaticData->atn, minicParserStaticData->decisionToDFA, minicParserStaticData->sharedContextCache, options);
}

MiniCParser::~MiniCParser() {
  delete _interpreter;
}

const atn::ATN& MiniCParser::getATN() const {
  return *minicParserStaticData->atn;
}

std::string MiniCParser::getGrammarFileName() const {
  return "MiniC.g4";
}

const std::vector<std::string>& MiniCParser::getRuleNames() const {
  return minicParserStaticData->ruleNames;
}

const dfa::Vocabulary& MiniCParser::getVocabulary() const {
  return minicParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MiniCParser::getSerializedATN() const {
  return minicParserStaticData->serializedATN;
}


//----------------- CompileUnitContext ------------------------------------------------------------------

MiniCParser::CompileUnitContext::CompileUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::CompileUnitContext::EOF() {
  return getToken(MiniCParser::EOF, 0);
}

std::vector<MiniCParser::FuncDefContext *> MiniCParser::CompileUnitContext::funcDef() {
  return getRuleContexts<MiniCParser::FuncDefContext>();
}

MiniCParser::FuncDefContext* MiniCParser::CompileUnitContext::funcDef(size_t i) {
  return getRuleContext<MiniCParser::FuncDefContext>(i);
}

std::vector<MiniCParser::VarDeclContext *> MiniCParser::CompileUnitContext::varDecl() {
  return getRuleContexts<MiniCParser::VarDeclContext>();
}

MiniCParser::VarDeclContext* MiniCParser::CompileUnitContext::varDecl(size_t i) {
  return getRuleContext<MiniCParser::VarDeclContext>(i);
}


size_t MiniCParser::CompileUnitContext::getRuleIndex() const {
  return MiniCParser::RuleCompileUnit;
}


std::any MiniCParser::CompileUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitCompileUnit(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::CompileUnitContext* MiniCParser::compileUnit() {
  CompileUnitContext *_localctx = _tracker.createInstance<CompileUnitContext>(_ctx, getState());
  enterRule(_localctx, 0, MiniCParser::RuleCompileUnit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(38);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MiniCParser::T_INT) {
      setState(36);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(34);
        funcDef();
        break;
      }

      case 2: {
        setState(35);
        varDecl();
        break;
      }

      default:
        break;
      }
      setState(40);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(41);
    match(MiniCParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

MiniCParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::FuncDefContext::T_INT() {
  return getToken(MiniCParser::T_INT, 0);
}

tree::TerminalNode* MiniCParser::FuncDefContext::T_ID() {
  return getToken(MiniCParser::T_ID, 0);
}

tree::TerminalNode* MiniCParser::FuncDefContext::T_L_PAREN() {
  return getToken(MiniCParser::T_L_PAREN, 0);
}

tree::TerminalNode* MiniCParser::FuncDefContext::T_R_PAREN() {
  return getToken(MiniCParser::T_R_PAREN, 0);
}

MiniCParser::BlockContext* MiniCParser::FuncDefContext::block() {
  return getRuleContext<MiniCParser::BlockContext>(0);
}


size_t MiniCParser::FuncDefContext::getRuleIndex() const {
  return MiniCParser::RuleFuncDef;
}


std::any MiniCParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::FuncDefContext* MiniCParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 2, MiniCParser::RuleFuncDef);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(43);
    match(MiniCParser::T_INT);
    setState(44);
    match(MiniCParser::T_ID);
    setState(45);
    match(MiniCParser::T_L_PAREN);
    setState(46);
    match(MiniCParser::T_R_PAREN);
    setState(47);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

MiniCParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::BlockContext::T_L_BRACE() {
  return getToken(MiniCParser::T_L_BRACE, 0);
}

tree::TerminalNode* MiniCParser::BlockContext::T_R_BRACE() {
  return getToken(MiniCParser::T_R_BRACE, 0);
}

MiniCParser::BlockItemListContext* MiniCParser::BlockContext::blockItemList() {
  return getRuleContext<MiniCParser::BlockItemListContext>(0);
}


size_t MiniCParser::BlockContext::getRuleIndex() const {
  return MiniCParser::RuleBlock;
}


std::any MiniCParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::BlockContext* MiniCParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 4, MiniCParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(49);
    match(MiniCParser::T_L_BRACE);
    setState(51);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 516634) != 0)) {
      setState(50);
      blockItemList();
    }
    setState(53);
    match(MiniCParser::T_R_BRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemListContext ------------------------------------------------------------------

MiniCParser::BlockItemListContext::BlockItemListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MiniCParser::BlockItemContext *> MiniCParser::BlockItemListContext::blockItem() {
  return getRuleContexts<MiniCParser::BlockItemContext>();
}

MiniCParser::BlockItemContext* MiniCParser::BlockItemListContext::blockItem(size_t i) {
  return getRuleContext<MiniCParser::BlockItemContext>(i);
}


size_t MiniCParser::BlockItemListContext::getRuleIndex() const {
  return MiniCParser::RuleBlockItemList;
}


std::any MiniCParser::BlockItemListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitBlockItemList(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::BlockItemListContext* MiniCParser::blockItemList() {
  BlockItemListContext *_localctx = _tracker.createInstance<BlockItemListContext>(_ctx, getState());
  enterRule(_localctx, 6, MiniCParser::RuleBlockItemList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(55);
      blockItem();
      setState(58); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 516634) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemContext ------------------------------------------------------------------

MiniCParser::BlockItemContext::BlockItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MiniCParser::StatementContext* MiniCParser::BlockItemContext::statement() {
  return getRuleContext<MiniCParser::StatementContext>(0);
}

MiniCParser::VarDeclContext* MiniCParser::BlockItemContext::varDecl() {
  return getRuleContext<MiniCParser::VarDeclContext>(0);
}


size_t MiniCParser::BlockItemContext::getRuleIndex() const {
  return MiniCParser::RuleBlockItem;
}


std::any MiniCParser::BlockItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitBlockItem(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::BlockItemContext* MiniCParser::blockItem() {
  BlockItemContext *_localctx = _tracker.createInstance<BlockItemContext>(_ctx, getState());
  enterRule(_localctx, 8, MiniCParser::RuleBlockItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(62);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MiniCParser::T_L_PAREN:
      case MiniCParser::T_SEMICOLON:
      case MiniCParser::T_L_BRACE:
      case MiniCParser::T_SUB:
      case MiniCParser::T_RETURN:
      case MiniCParser::T_ID:
      case MiniCParser::T_HEX_LITERAL:
      case MiniCParser::T_OCT_LITERAL:
      case MiniCParser::T_DEC_LITERAL: {
        enterOuterAlt(_localctx, 1);
        setState(60);
        statement();
        break;
      }

      case MiniCParser::T_INT: {
        enterOuterAlt(_localctx, 2);
        setState(61);
        varDecl();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

MiniCParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MiniCParser::BasicTypeContext* MiniCParser::VarDeclContext::basicType() {
  return getRuleContext<MiniCParser::BasicTypeContext>(0);
}

std::vector<MiniCParser::VarDefContext *> MiniCParser::VarDeclContext::varDef() {
  return getRuleContexts<MiniCParser::VarDefContext>();
}

MiniCParser::VarDefContext* MiniCParser::VarDeclContext::varDef(size_t i) {
  return getRuleContext<MiniCParser::VarDefContext>(i);
}

tree::TerminalNode* MiniCParser::VarDeclContext::T_SEMICOLON() {
  return getToken(MiniCParser::T_SEMICOLON, 0);
}

std::vector<tree::TerminalNode *> MiniCParser::VarDeclContext::T_COMMA() {
  return getTokens(MiniCParser::T_COMMA);
}

tree::TerminalNode* MiniCParser::VarDeclContext::T_COMMA(size_t i) {
  return getToken(MiniCParser::T_COMMA, i);
}


size_t MiniCParser::VarDeclContext::getRuleIndex() const {
  return MiniCParser::RuleVarDecl;
}


std::any MiniCParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::VarDeclContext* MiniCParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 10, MiniCParser::RuleVarDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(64);
    basicType();
    setState(65);
    varDef();
    setState(70);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MiniCParser::T_COMMA) {
      setState(66);
      match(MiniCParser::T_COMMA);
      setState(67);
      varDef();
      setState(72);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(73);
    match(MiniCParser::T_SEMICOLON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BasicTypeContext ------------------------------------------------------------------

MiniCParser::BasicTypeContext::BasicTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::BasicTypeContext::T_INT() {
  return getToken(MiniCParser::T_INT, 0);
}


size_t MiniCParser::BasicTypeContext::getRuleIndex() const {
  return MiniCParser::RuleBasicType;
}


std::any MiniCParser::BasicTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitBasicType(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::BasicTypeContext* MiniCParser::basicType() {
  BasicTypeContext *_localctx = _tracker.createInstance<BasicTypeContext>(_ctx, getState());
  enterRule(_localctx, 12, MiniCParser::RuleBasicType);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    match(MiniCParser::T_INT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

MiniCParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::VarDefContext::T_ID() {
  return getToken(MiniCParser::T_ID, 0);
}


size_t MiniCParser::VarDefContext::getRuleIndex() const {
  return MiniCParser::RuleVarDef;
}


std::any MiniCParser::VarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitVarDef(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::VarDefContext* MiniCParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 14, MiniCParser::RuleVarDef);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(77);
    match(MiniCParser::T_ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

MiniCParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t MiniCParser::StatementContext::getRuleIndex() const {
  return MiniCParser::RuleStatement;
}

void MiniCParser::StatementContext::copyFrom(StatementContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- BlockStatementContext ------------------------------------------------------------------

MiniCParser::BlockContext* MiniCParser::BlockStatementContext::block() {
  return getRuleContext<MiniCParser::BlockContext>(0);
}

MiniCParser::BlockStatementContext::BlockStatementContext(StatementContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::BlockStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitBlockStatement(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AssignStatementContext ------------------------------------------------------------------

MiniCParser::LValContext* MiniCParser::AssignStatementContext::lVal() {
  return getRuleContext<MiniCParser::LValContext>(0);
}

tree::TerminalNode* MiniCParser::AssignStatementContext::T_ASSIGN() {
  return getToken(MiniCParser::T_ASSIGN, 0);
}

MiniCParser::ExprContext* MiniCParser::AssignStatementContext::expr() {
  return getRuleContext<MiniCParser::ExprContext>(0);
}

tree::TerminalNode* MiniCParser::AssignStatementContext::T_SEMICOLON() {
  return getToken(MiniCParser::T_SEMICOLON, 0);
}

MiniCParser::AssignStatementContext::AssignStatementContext(StatementContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::AssignStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitAssignStatement(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ExpressionStatementContext ------------------------------------------------------------------

tree::TerminalNode* MiniCParser::ExpressionStatementContext::T_SEMICOLON() {
  return getToken(MiniCParser::T_SEMICOLON, 0);
}

MiniCParser::ExprContext* MiniCParser::ExpressionStatementContext::expr() {
  return getRuleContext<MiniCParser::ExprContext>(0);
}

MiniCParser::ExpressionStatementContext::ExpressionStatementContext(StatementContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::ExpressionStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitExpressionStatement(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ReturnStatementContext ------------------------------------------------------------------

tree::TerminalNode* MiniCParser::ReturnStatementContext::T_RETURN() {
  return getToken(MiniCParser::T_RETURN, 0);
}

MiniCParser::ExprContext* MiniCParser::ReturnStatementContext::expr() {
  return getRuleContext<MiniCParser::ExprContext>(0);
}

tree::TerminalNode* MiniCParser::ReturnStatementContext::T_SEMICOLON() {
  return getToken(MiniCParser::T_SEMICOLON, 0);
}

MiniCParser::ReturnStatementContext::ReturnStatementContext(StatementContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::ReturnStatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitReturnStatement(this);
  else
    return visitor->visitChildren(this);
}
MiniCParser::StatementContext* MiniCParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 16, MiniCParser::RuleStatement);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(93);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<MiniCParser::ReturnStatementContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(79);
      match(MiniCParser::T_RETURN);
      setState(80);
      expr();
      setState(81);
      match(MiniCParser::T_SEMICOLON);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<MiniCParser::AssignStatementContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(83);
      lVal();
      setState(84);
      match(MiniCParser::T_ASSIGN);
      setState(85);
      expr();
      setState(86);
      match(MiniCParser::T_SEMICOLON);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<MiniCParser::BlockStatementContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(88);
      block();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<MiniCParser::ExpressionStatementContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(90);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 492034) != 0)) {
        setState(89);
        expr();
      }
      setState(92);
      match(MiniCParser::T_SEMICOLON);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

MiniCParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

MiniCParser::AddExprContext* MiniCParser::ExprContext::addExpr() {
  return getRuleContext<MiniCParser::AddExprContext>(0);
}


size_t MiniCParser::ExprContext::getRuleIndex() const {
  return MiniCParser::RuleExpr;
}


std::any MiniCParser::ExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitExpr(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::ExprContext* MiniCParser::expr() {
  ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, getState());
  enterRule(_localctx, 18, MiniCParser::RuleExpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(95);
    addExpr(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AddExprContext ------------------------------------------------------------------

MiniCParser::AddExprContext::AddExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t MiniCParser::AddExprContext::getRuleIndex() const {
  return MiniCParser::RuleAddExpr;
}

void MiniCParser::AddExprContext::copyFrom(AddExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PassToMulExprContext ------------------------------------------------------------------

MiniCParser::MulExprContext* MiniCParser::PassToMulExprContext::mulExpr() {
  return getRuleContext<MiniCParser::MulExprContext>(0);
}

MiniCParser::PassToMulExprContext::PassToMulExprContext(AddExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::PassToMulExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitPassToMulExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AddSubExprContext ------------------------------------------------------------------

MiniCParser::AddExprContext* MiniCParser::AddSubExprContext::addExpr() {
  return getRuleContext<MiniCParser::AddExprContext>(0);
}

MiniCParser::MulExprContext* MiniCParser::AddSubExprContext::mulExpr() {
  return getRuleContext<MiniCParser::MulExprContext>(0);
}

tree::TerminalNode* MiniCParser::AddSubExprContext::T_ADD() {
  return getToken(MiniCParser::T_ADD, 0);
}

tree::TerminalNode* MiniCParser::AddSubExprContext::T_SUB() {
  return getToken(MiniCParser::T_SUB, 0);
}

MiniCParser::AddSubExprContext::AddSubExprContext(AddExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::AddSubExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitAddSubExpr(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::AddExprContext* MiniCParser::addExpr() {
   return addExpr(0);
}

MiniCParser::AddExprContext* MiniCParser::addExpr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  MiniCParser::AddExprContext *_localctx = _tracker.createInstance<AddExprContext>(_ctx, parentState);
  MiniCParser::AddExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 20;
  enterRecursionRule(_localctx, 20, MiniCParser::RuleAddExpr, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    _localctx = _tracker.createInstance<PassToMulExprContext>(_localctx);
    _ctx = _localctx;
    previousContext = _localctx;

    setState(98);
    mulExpr(0);
    _ctx->stop = _input->LT(-1);
    setState(105);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<AddSubExprContext>(_tracker.createInstance<AddExprContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleAddExpr);
        setState(100);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(101);
        _la = _input->LA(1);
        if (!(_la == MiniCParser::T_ADD

        || _la == MiniCParser::T_SUB)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(102);
        mulExpr(0); 
      }
      setState(107);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- MulExprContext ------------------------------------------------------------------

MiniCParser::MulExprContext::MulExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t MiniCParser::MulExprContext::getRuleIndex() const {
  return MiniCParser::RuleMulExpr;
}

void MiniCParser::MulExprContext::copyFrom(MulExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PassToUnaryExprContext ------------------------------------------------------------------

MiniCParser::UnaryExprContext* MiniCParser::PassToUnaryExprContext::unaryExpr() {
  return getRuleContext<MiniCParser::UnaryExprContext>(0);
}

MiniCParser::PassToUnaryExprContext::PassToUnaryExprContext(MulExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::PassToUnaryExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitPassToUnaryExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MulDivModExprContext ------------------------------------------------------------------

MiniCParser::MulExprContext* MiniCParser::MulDivModExprContext::mulExpr() {
  return getRuleContext<MiniCParser::MulExprContext>(0);
}

MiniCParser::UnaryExprContext* MiniCParser::MulDivModExprContext::unaryExpr() {
  return getRuleContext<MiniCParser::UnaryExprContext>(0);
}

tree::TerminalNode* MiniCParser::MulDivModExprContext::T_MUL() {
  return getToken(MiniCParser::T_MUL, 0);
}

tree::TerminalNode* MiniCParser::MulDivModExprContext::T_DIV() {
  return getToken(MiniCParser::T_DIV, 0);
}

tree::TerminalNode* MiniCParser::MulDivModExprContext::T_MOD() {
  return getToken(MiniCParser::T_MOD, 0);
}

MiniCParser::MulDivModExprContext::MulDivModExprContext(MulExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::MulDivModExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitMulDivModExpr(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::MulExprContext* MiniCParser::mulExpr() {
   return mulExpr(0);
}

MiniCParser::MulExprContext* MiniCParser::mulExpr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  MiniCParser::MulExprContext *_localctx = _tracker.createInstance<MulExprContext>(_ctx, parentState);
  MiniCParser::MulExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 22;
  enterRecursionRule(_localctx, 22, MiniCParser::RuleMulExpr, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    _localctx = _tracker.createInstance<PassToUnaryExprContext>(_localctx);
    _ctx = _localctx;
    previousContext = _localctx;

    setState(109);
    unaryExpr();
    _ctx->stop = _input->LT(-1);
    setState(116);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        auto newContext = _tracker.createInstance<MulDivModExprContext>(_tracker.createInstance<MulExprContext>(parentContext, parentState));
        _localctx = newContext;
        pushNewRecursionContext(newContext, startState, RuleMulExpr);
        setState(111);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(112);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & 7168) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(113);
        unaryExpr(); 
      }
      setState(118);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- UnaryExprContext ------------------------------------------------------------------

MiniCParser::UnaryExprContext::UnaryExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t MiniCParser::UnaryExprContext::getRuleIndex() const {
  return MiniCParser::RuleUnaryExpr;
}

void MiniCParser::UnaryExprContext::copyFrom(UnaryExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PassToPrimaryExprContext ------------------------------------------------------------------

MiniCParser::PrimaryExprContext* MiniCParser::PassToPrimaryExprContext::primaryExpr() {
  return getRuleContext<MiniCParser::PrimaryExprContext>(0);
}

MiniCParser::PassToPrimaryExprContext::PassToPrimaryExprContext(UnaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::PassToPrimaryExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitPassToPrimaryExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- NegationExprContext ------------------------------------------------------------------

tree::TerminalNode* MiniCParser::NegationExprContext::T_SUB() {
  return getToken(MiniCParser::T_SUB, 0);
}

MiniCParser::UnaryExprContext* MiniCParser::NegationExprContext::unaryExpr() {
  return getRuleContext<MiniCParser::UnaryExprContext>(0);
}

MiniCParser::NegationExprContext::NegationExprContext(UnaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::NegationExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitNegationExpr(this);
  else
    return visitor->visitChildren(this);
}
MiniCParser::UnaryExprContext* MiniCParser::unaryExpr() {
  UnaryExprContext *_localctx = _tracker.createInstance<UnaryExprContext>(_ctx, getState());
  enterRule(_localctx, 24, MiniCParser::RuleUnaryExpr);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(122);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case MiniCParser::T_SUB: {
        _localctx = _tracker.createInstance<MiniCParser::NegationExprContext>(_localctx);
        enterOuterAlt(_localctx, 1);
        setState(119);
        match(MiniCParser::T_SUB);
        setState(120);
        unaryExpr();
        break;
      }

      case MiniCParser::T_L_PAREN:
      case MiniCParser::T_ID:
      case MiniCParser::T_HEX_LITERAL:
      case MiniCParser::T_OCT_LITERAL:
      case MiniCParser::T_DEC_LITERAL: {
        _localctx = _tracker.createInstance<MiniCParser::PassToPrimaryExprContext>(_localctx);
        enterOuterAlt(_localctx, 2);
        setState(121);
        primaryExpr();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExprContext ------------------------------------------------------------------

MiniCParser::PrimaryExprContext::PrimaryExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t MiniCParser::PrimaryExprContext::getRuleIndex() const {
  return MiniCParser::RulePrimaryExpr;
}

void MiniCParser::PrimaryExprContext::copyFrom(PrimaryExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- ParenthesizedExprContext ------------------------------------------------------------------

tree::TerminalNode* MiniCParser::ParenthesizedExprContext::T_L_PAREN() {
  return getToken(MiniCParser::T_L_PAREN, 0);
}

MiniCParser::ExprContext* MiniCParser::ParenthesizedExprContext::expr() {
  return getRuleContext<MiniCParser::ExprContext>(0);
}

tree::TerminalNode* MiniCParser::ParenthesizedExprContext::T_R_PAREN() {
  return getToken(MiniCParser::T_R_PAREN, 0);
}

MiniCParser::ParenthesizedExprContext::ParenthesizedExprContext(PrimaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::ParenthesizedExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitParenthesizedExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- FunctionCallAtomContext ------------------------------------------------------------------

tree::TerminalNode* MiniCParser::FunctionCallAtomContext::T_ID() {
  return getToken(MiniCParser::T_ID, 0);
}

tree::TerminalNode* MiniCParser::FunctionCallAtomContext::T_L_PAREN() {
  return getToken(MiniCParser::T_L_PAREN, 0);
}

tree::TerminalNode* MiniCParser::FunctionCallAtomContext::T_R_PAREN() {
  return getToken(MiniCParser::T_R_PAREN, 0);
}

MiniCParser::RealParamListContext* MiniCParser::FunctionCallAtomContext::realParamList() {
  return getRuleContext<MiniCParser::RealParamListContext>(0);
}

MiniCParser::FunctionCallAtomContext::FunctionCallAtomContext(PrimaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::FunctionCallAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitFunctionCallAtom(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LValAtomContext ------------------------------------------------------------------

MiniCParser::LValContext* MiniCParser::LValAtomContext::lVal() {
  return getRuleContext<MiniCParser::LValContext>(0);
}

MiniCParser::LValAtomContext::LValAtomContext(PrimaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::LValAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitLValAtom(this);
  else
    return visitor->visitChildren(this);
}
//----------------- IntegerAtomContext ------------------------------------------------------------------

MiniCParser::IntegerLiteralContext* MiniCParser::IntegerAtomContext::integerLiteral() {
  return getRuleContext<MiniCParser::IntegerLiteralContext>(0);
}

MiniCParser::IntegerAtomContext::IntegerAtomContext(PrimaryExprContext *ctx) { copyFrom(ctx); }


std::any MiniCParser::IntegerAtomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitIntegerAtom(this);
  else
    return visitor->visitChildren(this);
}
MiniCParser::PrimaryExprContext* MiniCParser::primaryExpr() {
  PrimaryExprContext *_localctx = _tracker.createInstance<PrimaryExprContext>(_ctx, getState());
  enterRule(_localctx, 26, MiniCParser::RulePrimaryExpr);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(136);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<MiniCParser::ParenthesizedExprContext>(_localctx);
      enterOuterAlt(_localctx, 1);
      setState(124);
      match(MiniCParser::T_L_PAREN);
      setState(125);
      expr();
      setState(126);
      match(MiniCParser::T_R_PAREN);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<MiniCParser::IntegerAtomContext>(_localctx);
      enterOuterAlt(_localctx, 2);
      setState(128);
      integerLiteral();
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<MiniCParser::LValAtomContext>(_localctx);
      enterOuterAlt(_localctx, 3);
      setState(129);
      lVal();
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<MiniCParser::FunctionCallAtomContext>(_localctx);
      enterOuterAlt(_localctx, 4);
      setState(130);
      match(MiniCParser::T_ID);
      setState(131);
      match(MiniCParser::T_L_PAREN);
      setState(133);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & 492034) != 0)) {
        setState(132);
        realParamList();
      }
      setState(135);
      match(MiniCParser::T_R_PAREN);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IntegerLiteralContext ------------------------------------------------------------------

MiniCParser::IntegerLiteralContext::IntegerLiteralContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::IntegerLiteralContext::T_HEX_LITERAL() {
  return getToken(MiniCParser::T_HEX_LITERAL, 0);
}

tree::TerminalNode* MiniCParser::IntegerLiteralContext::T_OCT_LITERAL() {
  return getToken(MiniCParser::T_OCT_LITERAL, 0);
}

tree::TerminalNode* MiniCParser::IntegerLiteralContext::T_DEC_LITERAL() {
  return getToken(MiniCParser::T_DEC_LITERAL, 0);
}


size_t MiniCParser::IntegerLiteralContext::getRuleIndex() const {
  return MiniCParser::RuleIntegerLiteral;
}


std::any MiniCParser::IntegerLiteralContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitIntegerLiteral(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::IntegerLiteralContext* MiniCParser::integerLiteral() {
  IntegerLiteralContext *_localctx = _tracker.createInstance<IntegerLiteralContext>(_ctx, getState());
  enterRule(_localctx, 28, MiniCParser::RuleIntegerLiteral);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(138);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 458752) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RealParamListContext ------------------------------------------------------------------

MiniCParser::RealParamListContext::RealParamListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<MiniCParser::ExprContext *> MiniCParser::RealParamListContext::expr() {
  return getRuleContexts<MiniCParser::ExprContext>();
}

MiniCParser::ExprContext* MiniCParser::RealParamListContext::expr(size_t i) {
  return getRuleContext<MiniCParser::ExprContext>(i);
}

std::vector<tree::TerminalNode *> MiniCParser::RealParamListContext::T_COMMA() {
  return getTokens(MiniCParser::T_COMMA);
}

tree::TerminalNode* MiniCParser::RealParamListContext::T_COMMA(size_t i) {
  return getToken(MiniCParser::T_COMMA, i);
}


size_t MiniCParser::RealParamListContext::getRuleIndex() const {
  return MiniCParser::RuleRealParamList;
}


std::any MiniCParser::RealParamListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitRealParamList(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::RealParamListContext* MiniCParser::realParamList() {
  RealParamListContext *_localctx = _tracker.createInstance<RealParamListContext>(_ctx, getState());
  enterRule(_localctx, 30, MiniCParser::RuleRealParamList);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(140);
    expr();
    setState(145);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == MiniCParser::T_COMMA) {
      setState(141);
      match(MiniCParser::T_COMMA);
      setState(142);
      expr();
      setState(147);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValContext ------------------------------------------------------------------

MiniCParser::LValContext::LValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* MiniCParser::LValContext::T_ID() {
  return getToken(MiniCParser::T_ID, 0);
}


size_t MiniCParser::LValContext::getRuleIndex() const {
  return MiniCParser::RuleLVal;
}


std::any MiniCParser::LValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<MiniCVisitor*>(visitor))
    return parserVisitor->visitLVal(this);
  else
    return visitor->visitChildren(this);
}

MiniCParser::LValContext* MiniCParser::lVal() {
  LValContext *_localctx = _tracker.createInstance<LValContext>(_ctx, getState());
  enterRule(_localctx, 32, MiniCParser::RuleLVal);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(148);
    match(MiniCParser::T_ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool MiniCParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 10: return addExprSempred(antlrcpp::downCast<AddExprContext *>(context), predicateIndex);
    case 11: return mulExprSempred(antlrcpp::downCast<MulExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool MiniCParser::addExprSempred(AddExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool MiniCParser::mulExprSempred(MulExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

void MiniCParser::initialize() {
  ::antlr4::internal::call_once(minicParserOnceFlag, minicParserInitialize);
}
