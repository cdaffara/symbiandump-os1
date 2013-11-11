// lexer.cpp
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

#include "lexer.h"


//
// TToken
//

TToken::TToken()
	: iToken(NULL, 0), iPos(0)
	{
	}

TToken::TToken(TType aType, const TDesC& aToken, TInt aPos)
	: iType(aType), iToken(aToken), iPos(aPos)
	{
	}

TToken& TToken::operator=(const TToken& aToken)
	{
	iType = aToken.iType;
	iToken.Set(aToken.iToken);
	iPos = aToken.iPos;
	return *this;
	}

TToken::TType TToken::Type() const
	{
	return iType;
	}

TInt TToken::Position() const
	{
	return iPos;
	}

const TDesC& TToken::String() const
	{
	return iToken;
	}


//
// CReservedLookup
//

class CReservedLookup : public CBase
	{
public:
	class TResult
		{
	public:
		enum TType
			{
			ENoMatch,
			EMatch
			};
	public:
		TResult();
	public:
		TType iResultType;
		TToken::TType iTokenType;
		TInt iTokenLength;
		};
	enum TCharPos
		{
		ENotLast,
		ELast
		};
public:
	static CReservedLookup* NewL();
	~CReservedLookup();
	void DefineTokenTypeL(TToken::TType aTokenType, const TDesC& aString);
	void Reset();
	TResult Lookup(const TDesC& aDes);
private:
	class TReserved
		{
	public:
		TReserved(TToken::TType aType, const TDesC& aString);
	public:
		TToken::TType iType;
		TPtrC iString;
		};
private:
	RArray<TReserved> iList;
	};

CReservedLookup* CReservedLookup::NewL()
	{
	return new(ELeave) CReservedLookup();
	}

CReservedLookup::~CReservedLookup()
	{
	iList.Close();
	}

void CReservedLookup::DefineTokenTypeL(TToken::TType aTokenType, const TDesC& aString)
	{
	User::LeaveIfError(iList.Append(TReserved(aTokenType, aString)));
	}

CReservedLookup::TResult CReservedLookup::Lookup(const TDesC& aDes)
	{
	// Find the longest reserved word that matches from the beginning of this string.
	TResult result;
	const TInt count = iList.Count();
	for (TInt i = 0; i < count; ++i)
		{
		const TReserved& reserved = iList[i];
		if (aDes.Left(reserved.iString.Length()) == reserved.iString)
			{
			if (result.iTokenLength < reserved.iString.Length())
				{
				result.iTokenLength = reserved.iString.Length();
				result.iResultType = TResult::EMatch;
				result.iTokenType = reserved.iType;
				}
			}
		}
	return result;
	}

CReservedLookup::TReserved::TReserved(TToken::TType aType, const TDesC& aString)
	: iType(aType), iString(aString)
	{
	}

CReservedLookup::TResult::TResult()
	: iResultType(ENoMatch), iTokenType(TToken::EString), iTokenLength(0)
	{
	}


//
// CLexer
//

CLexer* CLexer::NewL()
	{
	CLexer* self = CLexer::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CLexer* CLexer::NewL(TUint aBehaviour)
	{
	CLexer* self = CLexer::NewLC(aBehaviour);
	CleanupStack::Pop(self);
	return self;
	}

CLexer* CLexer::NewLC()
	{
	CLexer* self = new(ELeave) CLexer(EHandleSingleQuotes | EHandleDoubleQuotes | EHandleComments);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLexer* CLexer::NewLC(TUint aBehaviour)
	{
	CLexer* self = new(ELeave) CLexer(aBehaviour);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLexer::~CLexer()
	{
	delete iReservedLookup;
	}

void CLexer::DefineTokenTypeL(TToken::TType aTokenType, const TDesC& aString)
	{
	iReservedLookup->DefineTokenTypeL(aTokenType, aString);
	}

void CLexer::Set(const TDesC& aDes, const TChar& aEscapeChar)
	{
	iLex = aDes;
	iEscapeChar = aEscapeChar;
	}

TToken CLexer::NextToken()
	{
	SkipWhiteSpace();
	iLex.Mark();
	TToken::TType type(TToken::ENull);

	while (!iLex.Eos())
		{
		type = TToken::EString;
		TChar c = iLex.Get();
		if (c == iEscapeChar)
			{
			iLex.Get();
			}
		else if ((c == '\'') && (iBehaviour & EHandleSingleQuotes))
			{
			SkipSingleQuotedChars();
			}
		else if ((c == '\"') && (iBehaviour & EHandleDoubleQuotes))
			{
			SkipDoubleQuotedChars();
			}
		else if ((c == '#') && (iBehaviour & EHandleComments))
			{
			if (iLex.MarkedToken().Length() > 1)
				{
				iLex.UnGet();
				break;
				}
			else
				{
				SkipComment();
				if (iLex.Eos())
					{
					type = TToken::ENull;
					break;
					}
				else
					{
					iLex.Mark();
					}
				}
			}
		else if (c.IsSpace() && (c != '\n') && (c != '\r'))
			{
			iLex.UnGet();
			break;
			}
		else
			{
			iLex.UnGet();
			CReservedLookup::TResult result = iReservedLookup->Lookup(iLex.Remainder());
			if (result.iResultType == CReservedLookup::TResult::EMatch)
				{
				if (iLex.MarkedToken().Length() > 0)
					{
					break;
					}
				else
					{
					iLex.Inc(result.iTokenLength);
					type = result.iTokenType;
					break;
					}
				}
			iLex.Get();
			}
		}

	return TToken(type, iLex.MarkedToken(), iLex.MarkedOffset());
	}

TInt CLexer::CurrentOffset() const
	{
	return iLex.Offset();
	}

TBool CLexer::More()
	{
	SkipWhiteSpace();
	return !iLex.Eos();
	}

CLexer::CLexer(TUint aBehaviour)
	: iBehaviour(aBehaviour)
	{
	}

void CLexer::ConstructL()
	{
	iReservedLookup = CReservedLookup::NewL();
	}

void CLexer::SkipSingleQuotedChars()
	{
	while (!iLex.Eos())
		{
		TChar c = iLex.Get();
		if ((c == iEscapeChar) && !iLex.Eos() && (iLex.Peek() == '\''))
			{
			// Allow quoted single quote characters. Note, the is a departure from Bash behaviour, but is in line with Perl and is generally helpful.
			iLex.Get();
			}
		else if (c == '\'')
			{
			break;
			}
		}
	}

void CLexer::SkipDoubleQuotedChars()
	{
	while (!iLex.Eos())
		{
		TChar c = iLex.Get();
		if ((c == iEscapeChar) && !iLex.Eos())
			{
			iLex.Get();
			}
		else if (c == '"')
			{
			break;
			}
		}
	}

void CLexer::SkipComment()
	{
	while (!iLex.Eos())
		{
		TChar c = iLex.Get();
		if ((c == '\n') || (c == '\r'))
			{
			iLex.UnGet();
			break;
			}
		}
	}

void CLexer::SkipWhiteSpace()
	{
	while (!iLex.Eos())
		{
		TChar c = iLex.Get();
		if (!c.IsSpace() || (c == '\n') || (c == '\r'))
			{
			iLex.UnGet();
			break;
			}
		}
	}
