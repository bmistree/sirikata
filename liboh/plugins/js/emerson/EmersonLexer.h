/** \file
 *  This C header file was generated by $ANTLR version 3.1.3 Mar 17, 2009 19:23:44
 *
 *     -  From the grammar source file : .//Emerson.g
 *     -                            On : 2011-08-02 18:53:08
 *     -                 for the lexer : EmersonLexerLexer *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The lexer EmersonLexer has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pEmersonLexer, which is returned from a call to EmersonLexerNew().
 *
 * As this is a generated lexer, it is unlikely you will call it 'manually'. However
 * the methods are provided anyway.
 * * The methods in pEmersonLexer are  as follows:
 *
 *  -  void      pEmersonLexer->T__139(pEmersonLexer)
 *  -  void      pEmersonLexer->T__140(pEmersonLexer)
 *  -  void      pEmersonLexer->T__141(pEmersonLexer)
 *  -  void      pEmersonLexer->T__142(pEmersonLexer)
 *  -  void      pEmersonLexer->T__143(pEmersonLexer)
 *  -  void      pEmersonLexer->T__144(pEmersonLexer)
 *  -  void      pEmersonLexer->T__145(pEmersonLexer)
 *  -  void      pEmersonLexer->T__146(pEmersonLexer)
 *  -  void      pEmersonLexer->T__147(pEmersonLexer)
 *  -  void      pEmersonLexer->T__148(pEmersonLexer)
 *  -  void      pEmersonLexer->T__149(pEmersonLexer)
 *  -  void      pEmersonLexer->T__150(pEmersonLexer)
 *  -  void      pEmersonLexer->T__151(pEmersonLexer)
 *  -  void      pEmersonLexer->T__152(pEmersonLexer)
 *  -  void      pEmersonLexer->T__153(pEmersonLexer)
 *  -  void      pEmersonLexer->T__154(pEmersonLexer)
 *  -  void      pEmersonLexer->T__155(pEmersonLexer)
 *  -  void      pEmersonLexer->T__156(pEmersonLexer)
 *  -  void      pEmersonLexer->T__157(pEmersonLexer)
 *  -  void      pEmersonLexer->T__158(pEmersonLexer)
 *  -  void      pEmersonLexer->T__159(pEmersonLexer)
 *  -  void      pEmersonLexer->T__160(pEmersonLexer)
 *  -  void      pEmersonLexer->T__161(pEmersonLexer)
 *  -  void      pEmersonLexer->T__162(pEmersonLexer)
 *  -  void      pEmersonLexer->T__163(pEmersonLexer)
 *  -  void      pEmersonLexer->T__164(pEmersonLexer)
 *  -  void      pEmersonLexer->T__165(pEmersonLexer)
 *  -  void      pEmersonLexer->T__166(pEmersonLexer)
 *  -  void      pEmersonLexer->T__167(pEmersonLexer)
 *  -  void      pEmersonLexer->T__168(pEmersonLexer)
 *  -  void      pEmersonLexer->T__169(pEmersonLexer)
 *  -  void      pEmersonLexer->T__170(pEmersonLexer)
 *  -  void      pEmersonLexer->T__171(pEmersonLexer)
 *  -  void      pEmersonLexer->T__172(pEmersonLexer)
 *  -  void      pEmersonLexer->T__173(pEmersonLexer)
 *  -  void      pEmersonLexer->T__174(pEmersonLexer)
 *  -  void      pEmersonLexer->T__175(pEmersonLexer)
 *  -  void      pEmersonLexer->T__176(pEmersonLexer)
 *  -  void      pEmersonLexer->T__177(pEmersonLexer)
 *  -  void      pEmersonLexer->T__178(pEmersonLexer)
 *  -  void      pEmersonLexer->T__179(pEmersonLexer)
 *  -  void      pEmersonLexer->T__180(pEmersonLexer)
 *  -  void      pEmersonLexer->T__181(pEmersonLexer)
 *  -  void      pEmersonLexer->T__182(pEmersonLexer)
 *  -  void      pEmersonLexer->T__183(pEmersonLexer)
 *  -  void      pEmersonLexer->T__184(pEmersonLexer)
 *  -  void      pEmersonLexer->T__185(pEmersonLexer)
 *  -  void      pEmersonLexer->T__186(pEmersonLexer)
 *  -  void      pEmersonLexer->T__187(pEmersonLexer)
 *  -  void      pEmersonLexer->T__188(pEmersonLexer)
 *  -  void      pEmersonLexer->T__189(pEmersonLexer)
 *  -  void      pEmersonLexer->T__190(pEmersonLexer)
 *  -  void      pEmersonLexer->T__191(pEmersonLexer)
 *  -  void      pEmersonLexer->T__192(pEmersonLexer)
 *  -  void      pEmersonLexer->T__193(pEmersonLexer)
 *  -  void      pEmersonLexer->T__194(pEmersonLexer)
 *  -  void      pEmersonLexer->T__195(pEmersonLexer)
 *  -  void      pEmersonLexer->T__196(pEmersonLexer)
 *  -  void      pEmersonLexer->T__197(pEmersonLexer)
 *  -  void      pEmersonLexer->T__198(pEmersonLexer)
 *  -  void      pEmersonLexer->T__199(pEmersonLexer)
 *  -  void      pEmersonLexer->T__200(pEmersonLexer)
 *  -  void      pEmersonLexer->T__201(pEmersonLexer)
 *  -  void      pEmersonLexer->T__202(pEmersonLexer)
 *  -  void      pEmersonLexer->T__203(pEmersonLexer)
 *  -  void      pEmersonLexer->T__204(pEmersonLexer)
 *  -  void      pEmersonLexer->T__205(pEmersonLexer)
 *  -  void      pEmersonLexer->T__206(pEmersonLexer)
 *  -  void      pEmersonLexer->T__207(pEmersonLexer)
 *  -  void      pEmersonLexer->T__208(pEmersonLexer)
 *  -  void      pEmersonLexer->T__209(pEmersonLexer)
 *  -  void      pEmersonLexer->T__210(pEmersonLexer)
 *  -  void      pEmersonLexer->T__211(pEmersonLexer)
 *  -  void      pEmersonLexer->T__212(pEmersonLexer)
 *  -  void      pEmersonLexer->StringLiteral(pEmersonLexer)
 *  -  void      pEmersonLexer->VerbatimStringCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->DoubleStringCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->SingleStringCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->EscapeSequence(pEmersonLexer)
 *  -  void      pEmersonLexer->CharacterEscapeSequence(pEmersonLexer)
 *  -  void      pEmersonLexer->NonEscapeCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->SingleEscapeCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->EscapeCharacter(pEmersonLexer)
 *  -  void      pEmersonLexer->HexEscapeSequence(pEmersonLexer)
 *  -  void      pEmersonLexer->UnicodeEscapeSequence(pEmersonLexer)
 *  -  void      pEmersonLexer->NumericLiteral(pEmersonLexer)
 *  -  void      pEmersonLexer->HexIntegerLiteral(pEmersonLexer)
 *  -  void      pEmersonLexer->HexDigit(pEmersonLexer)
 *  -  void      pEmersonLexer->DecimalLiteral(pEmersonLexer)
 *  -  void      pEmersonLexer->DecimalDigit(pEmersonLexer)
 *  -  void      pEmersonLexer->ExponentPart(pEmersonLexer)
 *  -  void      pEmersonLexer->Identifier(pEmersonLexer)
 *  -  void      pEmersonLexer->IdentifierStart(pEmersonLexer)
 *  -  void      pEmersonLexer->IdentifierPart(pEmersonLexer)
 *  -  void      pEmersonLexer->UnicodeLetter(pEmersonLexer)
 *  -  void      pEmersonLexer->UnicodeCombiningMark(pEmersonLexer)
 *  -  void      pEmersonLexer->UnicodeDigit(pEmersonLexer)
 *  -  void      pEmersonLexer->UnicodeConnectorPunctuation(pEmersonLexer)
 *  -  void      pEmersonLexer->Comment(pEmersonLexer)
 *  -  void      pEmersonLexer->LineComment(pEmersonLexer)
 *  -  void      pEmersonLexer->LTERM(pEmersonLexer)
 *  -  void      pEmersonLexer->WhiteSpace(pEmersonLexer)
 *  -  void      pEmersonLexer->Tokens(pEmersonLexer)
 * 
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_EmersonLexer_H
#define _EmersonLexer_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct EmersonLexer_Ctx_struct EmersonLexer, * pEmersonLexer;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */

/** Context tracking structure for EmersonLexer
 */
struct EmersonLexer_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_LEXER    pLexer;


     void (*mT__139)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__140)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__141)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__142)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__143)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__144)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__145)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__146)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__147)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__148)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__149)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__150)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__151)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__152)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__153)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__154)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__155)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__156)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__157)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__158)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__159)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__160)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__161)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__162)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__163)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__164)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__165)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__166)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__167)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__168)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__169)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__170)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__171)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__172)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__173)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__174)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__175)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__176)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__177)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__178)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__179)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__180)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__181)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__182)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__183)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__184)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__185)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__186)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__187)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__188)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__189)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__190)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__191)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__192)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__193)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__194)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__195)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__196)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__197)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__198)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__199)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__200)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__201)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__202)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__203)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__204)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__205)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__206)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__207)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__208)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__209)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__210)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__211)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mT__212)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mStringLiteral)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mVerbatimStringCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mDoubleStringCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mSingleStringCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mEscapeSequence)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mCharacterEscapeSequence)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mNonEscapeCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mSingleEscapeCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mEscapeCharacter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mHexEscapeSequence)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mUnicodeEscapeSequence)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mNumericLiteral)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mHexIntegerLiteral)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mHexDigit)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mDecimalLiteral)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mDecimalDigit)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mExponentPart)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mIdentifier)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mIdentifierStart)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mIdentifierPart)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mUnicodeLetter)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mUnicodeCombiningMark)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mUnicodeDigit)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mUnicodeConnectorPunctuation)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mComment)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mLineComment)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mLTERM)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mWhiteSpace)	(struct EmersonLexer_Ctx_struct * ctx);
     void (*mTokens)	(struct EmersonLexer_Ctx_struct * ctx);
    const char * (*getGrammarFileName)();
    void	    (*free)   (struct EmersonLexer_Ctx_struct * ctx);
        
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pEmersonLexer EmersonLexerNew         (pANTLR3_INPUT_STREAM instream);
ANTLR3_API pEmersonLexer EmersonLexerNewSSD      (pANTLR3_INPUT_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the lexer will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define T__159      159
#define T__158      158
#define VERBATIM      106
#define MOD      72
#define T__160      160
#define DO      14
#define NOT      84
#define T__167      167
#define T__168      168
#define EOF      -1
#define T__165      165
#define T__166      166
#define T__163      163
#define UNARY_PLUS      81
#define T__164      164
#define T__161      161
#define SingleStringCharacter      117
#define T__162      162
#define SENDER_CONSTRUCT_NO_IN      110
#define T__148      148
#define MESSAGE_RECV_NO_SENDER_NO_IN      93
#define T__147      147
#define T__149      149
#define PATTERN_LITERAL      98
#define INSTANCE_OF      66
#define RETURN      23
#define UnicodeLetter      132
#define NOOP      33
#define WhiteSpace      138
#define T__154      154
#define T__155      155
#define T__156      156
#define T__157      157
#define T__150      150
#define T__151      151
#define T__152      152
#define UnicodeCombiningMark      135
#define T__153      153
#define T__139      139
#define LTERM      111
#define UnicodeDigit      133
#define NumericLiteral      113
#define QUATERNION      31
#define UNARY_MINUS      82
#define DoubleStringCharacter      116
#define VALUE      102
#define T__141      141
#define T__142      142
#define BLANK_NAME_VAL_PROT      100
#define T__140      140
#define T__145      145
#define T__146      146
#define T__143      143
#define T__144      144
#define TYPEOF      78
#define LESS_THAN      62
#define COMPLEMENT      83
#define CASE_BLOCK      42
#define NAME_VALUE      75
#define CALL      5
#define CharacterEscapeSequence      119
#define PLUSPLUS      79
#define SUB      69
#define NOT_EQUALS      59
#define HexDigit      126
#define PAREN      97
#define VerbatimStringCharacter      115
#define T__202      202
#define T__203      203
#define T__204      204
#define T__205      205
#define T__206      206
#define T__207      207
#define T__208      208
#define T__209      209
#define WHEN_CHECKED_LIST_SUBSEQUENT      28
#define ARRAY_INDEX      6
#define SLIST      9
#define IDENT      60
#define SEND_CONSTRUCT_NO_IN      109
#define T__210      210
#define T__212      212
#define T__211      211
#define GREATER_THAN      63
#define EXP_ASSIGN      51
#define UnicodeEscapeSequence      121
#define FUNC_DECL      87
#define NOT_IDENT      61
#define StringLiteral      114
#define OR_ASSIGN      52
#define FORCOND      18
#define VECTOR      30
#define WHEN      26
#define HexIntegerLiteral      128
#define NonEscapeCharacter      123
#define SEND_CONSTRUCT      107
#define LESS_THAN_EQUAL      64
#define DOLLAR_EXPRESSION      34
#define MESSAGE_RECV_AND_SENDER_NO_IN      94
#define EXPR_NO_IN      105
#define DIV      71
#define SUB_ASSIGN      49
#define WHEN_PRED      29
#define OBJ_LITERAL      74
#define WHILE      15
#define SENDER_CONSTRUCT      108
#define MOD_ASSIGN      47
#define CASE      41
#define NEW      25
#define MINUSMINUS      80
#define ARGLIST      89
#define EQUALS      58
#define ARRAY_LITERAL      73
#define DecimalDigit      125
#define FUNC_EXPR      88
#define DIV_ASSIGN      46
#define BREAK      21
#define Identifier      112
#define NAME      101
#define BIT_OR      55
#define Comment      136
#define EXP      56
#define MESSAGE_RECV_AND_SENDER      96
#define NAME_VALUE_PROTO      99
#define SingleEscapeCharacter      122
#define WHEN_CHECKED_LIST_FIRST      27
#define ExponentPart      129
#define VAR      12
#define VOID      77
#define FORINIT      17
#define GREATER_THAN_EQUAL      65
#define ADD_ASSIGN      48
#define SWITCH      40
#define MESSAGE_RECV_NO_SENDER      95
#define ADD_OP      68
#define IdentifierStart      130
#define FUNC_PARAMS      86
#define MULT      70
#define EMPTY_FUNC_BODY      92
#define TRY      35
#define T__200      200
#define T__201      201
#define FUNC      8
#define OR      53
#define VARLIST      11
#define CATCH      37
#define TERNARYOP_NO_IN      91
#define EscapeSequence      118
#define THROW      36
#define UnicodeConnectorPunctuation      134
#define DELETE_OP      76
#define HexEscapeSequence      120
#define BIT_AND      57
#define QUATERNION_AXISANGLE      32
#define MULT_ASSIGN      45
#define LineComment      137
#define FOR      16
#define AND      54
#define AND_ASSIGN      50
#define IF      10
#define T__199      199
#define PROG      13
#define T__198      198
#define T__197      197
#define EXPR      104
#define T__196      196
#define IN      67
#define T__195      195
#define T__194      194
#define T__193      193
#define CONTINUE      22
#define T__192      192
#define T__191      191
#define T__190      190
#define FORITER      19
#define EscapeCharacter      124
#define UNDEF      4
#define DOT      7
#define TERNARYOP      90
#define IdentifierPart      131
#define WITH      24
#define T__184      184
#define T__183      183
#define T__186      186
#define T__185      185
#define T__188      188
#define T__187      187
#define T__189      189
#define T__180      180
#define DEFAULT      39
#define T__182      182
#define POSTEXPR      85
#define T__181      181
#define FORIN      20
#define DecimalLiteral      127
#define T__175      175
#define T__174      174
#define T__173      173
#define T__172      172
#define T__179      179
#define T__178      178
#define T__177      177
#define FINALLY      38
#define T__176      176
#define LABEL      43
#define T__171      171
#define T__170      170
#define ASSIGN      44
#define PROTO      103
#define T__169      169
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for EmersonLexer
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
