// pod_lexer.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __POD_LEXER_H__
#define __POD_LEXER_H__

#include <e32std.h>

const TInt KMaxNestedAttributes = 16;


/**
 * Responsible for understanding POD interior sequences (B<bold>, I<italic>, etc.).
 */
class TPodLexer
	{
public:
	enum TTokenType
		{
		EAttributePush,
		EAttributePop,
		ETextBlock,
		ECodeBlock,
		ELink,
		EIndexEntry
		};
	enum TAttribute
		{
		ENull		= 0x00000000,
		EBold		= 0x00000001,
		ECode		= 0x00000002,
		EItalic		= 0x00000004,
		EFileName	= 0x00000008
		};
public:
	TPodLexer(const TDesC& aString);
	void NextTokenL(TPtrC& aToken, TTokenType& aTokenType, TUint& aAttributes, TBool& aEop, TBool& aEos);
private:
	TInt SkipToClosingAngleBracket();
	TBool Eop();
	TBool IsInteriorSequenceStart(TUint16& aSequenceIdentifier);
	void PushAttribute(TAttribute aAttribute);
	TAttribute PopAttribute();
private:
	TLex iLex;
	TUint iAttributes;
	TBuf<64> iScratch;
	TFixedArray<TAttribute, KMaxNestedAttributes> iAttributeStack;
	TInt iAttributeStackIndex;
	};

#endif // __POD_LEXER_H__
