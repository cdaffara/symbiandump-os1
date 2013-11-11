// pod_lexer.cpp
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

#include "pod_lexer.h"


TPodLexer::TPodLexer(const TDesC& aString)
	: iLex(aString), iAttributes(0), iAttributeStackIndex(0)
	{
	}

void TPodLexer::NextTokenL(TPtrC& aToken, TTokenType& aTokenType, TUint& aAttributes, TBool& aEop, TBool& aEos)
	{
	aTokenType = ETextBlock;
	iScratch.Zero();
	TBool finished(EFalse);
	TBool setAttributes(ETrue);
	while (!iLex.Eos() && !finished && (iScratch.Length() < (iScratch.MaxLength() - 1)))
		{
		iLex.Mark();
		TUint16 sequenceIdentifier;
		if ((iAttributeStackIndex > 0) && (iLex.Peek() == '>'))
			{
			if (iScratch.Length() > 0)
				{
				aAttributes = iAttributes;
				setAttributes = EFalse;
				finished = ETrue;
				}
			else
				{
				iLex.Get();
				aTokenType = EAttributePop;
				aAttributes = PopAttribute();
				setAttributes = EFalse;
				finished = ETrue;
				}
			}
		else if (IsInteriorSequenceStart(sequenceIdentifier))
			{
			if (iScratch.Length() > 0)
				{
				iLex.UnGetToMark();
				finished = ETrue;
				}
			else
				{
				switch (sequenceIdentifier)
					{
					case 'B':
						PushAttribute(EBold);
						aAttributes = EBold;
						setAttributes = EFalse;
						aTokenType = EAttributePush;
						finished = ETrue;
						break;
					case 'C':
						PushAttribute(ECode);
						aAttributes = ECode;
						setAttributes = EFalse;
						aTokenType = EAttributePush;
						finished = ETrue;
						break;
					case 'E':
						if (iLex.Remainder().Length() >= 3)
							{
							if (iLex.Remainder().Left(3) == _L("gt>"))
								{
								iLex.Inc(3);
								iScratch.Append('>');
								}
							else if (iLex.Remainder().Left(3) == _L("lt>"))
								{
								iLex.Inc(3);
								iScratch.Append('<');
								}
							else
								{
								TUint num;
								TInt err = iLex.Val(num);
								if (err == KErrNone)
									{
									iScratch.Append(num);
									}
								else
									{
									User::Leave(err);
									}
								}
							}
						break;
					case 'F':
						PushAttribute(EFileName);
						aAttributes = EFileName;
						setAttributes = EFalse;
						aTokenType = EAttributePush;
						finished = ETrue;
						break;
					case 'I':
						PushAttribute(EItalic);
						aAttributes = EItalic;
						setAttributes = EFalse;
						aTokenType = EAttributePush;
						finished = ETrue;
						break;
					case 'L':
						iLex.Mark();
						SkipToClosingAngleBracket();
						iLex.UnGet();
						iScratch.Copy(iLex.MarkedToken());
						iLex.Get();
						aTokenType = ELink;
						finished = ETrue;
						break;
					case 'S':
					case 'X':
						PushAttribute(ENull);
						aAttributes = ENull;
						setAttributes = EFalse;
						aTokenType = EAttributePush;
						finished = ETrue;
						break;
					case 'Z':
						if (iLex.Peek() == '>')
							{
							iLex.Inc();
							iLex.Mark();
							}
						else
							{
							User::Leave(KErrArgument);
							}
						break;
					}
				}
			}
		else
			{
			iScratch.Append(iLex.Get());
			}
		}

	if (setAttributes)
		{
		aAttributes = iAttributes;
		}
	aToken.Set(iScratch);
	aEop = Eop();
	aEos = iLex.Eos();
	}

TInt TPodLexer::SkipToClosingAngleBracket()
	{
	TInt numSkippedChars = 0;
	while (!iLex.Eos())
		{
		TUint16 sequenceIdentifier;
		if (IsInteriorSequenceStart(sequenceIdentifier))
			{
			numSkippedChars = SkipToClosingAngleBracket();
			}
		else if (iLex.Get() == '>')
			{
			break;
			}
		else
			{
			++numSkippedChars;
			}
		}
	return numSkippedChars;
	}

TBool TPodLexer::Eop()
	{
	_LIT(KParagraphEnd, "\r\n\r\n");
	return ((iLex.Remainder().Length() >= 4) && (iLex.Remainder().Left(4) == KParagraphEnd));
	}

TBool TPodLexer::IsInteriorSequenceStart(TUint16& aSequenceIdentifier)
	{
	if (iLex.Remainder().Length() >= 2)
		{
		TUint16 first = iLex.Get();
		TUint16 second = iLex.Get();
		if (second == '<')
			{
			switch (first)
				{
				case 'B':
				case 'C':
				case 'E':
				case 'F':
				case 'I':
				case 'L':
				case 'S':
				case 'X':
				case 'Z':
					aSequenceIdentifier = first;
					return ETrue;
				}
			}
		iLex.Inc(-2);
		}
	return EFalse;
	}

void TPodLexer::PushAttribute(TAttribute aAttribute)
	{
	iAttributes |= aAttribute;
	iAttributeStack[iAttributeStackIndex++] = aAttribute;
	}

TPodLexer::TAttribute TPodLexer::PopAttribute()
	{
	TAttribute poppedAttribute = iAttributeStack[--iAttributeStackIndex];
	iAttributes &= ~poppedAttribute;
	return poppedAttribute;
	}

