// lexer.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __LEXER_H__
#define __LEXER_H__

#include <e32base.h>

class CReservedLookup;


class TToken
	{
public:
	enum TType
		{
		ENull,
		EString,
		EPipe,
		EDoublePipe,
		ERedirectStdinFromFile,
		ERedirectStdoutToFile,
		ERedirectStdoutToFileAppend,
		ERedirectStdoutToStderr,
		ERedirectStderrToFile,
		ERedirectStderrToFileAppend,
		ERedirectStderrToStdout,
		EAmpersand,
		EDoubleAmpersand,
		EAmpersandPipe,
		ENewLine,
		ESemicolon,
		EVariable
		};
public:
	TToken();
	TToken(TType aType, const TDesC& aToken, TInt aPos);
	TToken& operator=(const TToken& aToken);
	TType Type() const;
	TInt Position() const;
	const TDesC& String() const;
private:
	TType iType;
	TPtrC iToken;
	TInt iPos;
	};


class CLexer : public CBase
	{
public:
	enum TBehaviour
		{
		EHandleSingleQuotes	= 0x00000001,	///< Causes text within single quotes to be reported as a single TToken::EString type token.
		EHandleDoubleQuotes	= 0x00000002,	///< Causes text within double quotes to be reported as a single TToken::EString type token.
		EHandleComments		= 0x00000004	///< Causes text following a '#' until the end of the line to be discarded.
		};
public:
	static CLexer* NewL();
	static CLexer* NewL(TUint aBehaviour);
	static CLexer* NewLC();
	static CLexer* NewLC(TUint aBehaviour);
	~CLexer();
	void DefineTokenTypeL(TToken::TType aTokenType, const TDesC& aString);
	void Set(const TDesC& aDes, const TChar& aEscapeChar);
	TToken NextToken();
	TInt CurrentOffset() const;
	TBool More();
private:
	CLexer(TUint aBehaviour);
	void ConstructL();
	void SkipSingleQuotedChars();
	void SkipDoubleQuotedChars();
	void SkipComment();
	void SkipWhiteSpace();
private:
	TUint iBehaviour;
	TChar iEscapeChar;
	TLex iLex;
	CReservedLookup* iReservedLookup;
	};


#endif // __LEXER_H__
