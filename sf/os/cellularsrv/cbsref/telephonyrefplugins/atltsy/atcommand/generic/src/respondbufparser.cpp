// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains CRespondBufParser which provide an algorithm to analize the passed data from baseband
//

#include "respondbufparser.h"
#include "mslogger.h"

_LIT8(KSpace, " ");

CRespondBufParser::CRespondBufParser() 
	{	
	}

CRespondBufParser::~CRespondBufParser()
	{
	}

// ---------------------------------------------------------------------------
// CRespondBufParser::ParseRespondedBuffer
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CRespondBufParser::ParseRespondedBuffer(RArray<TPtrC8>& aArray, const TDesC8& aBuf)
	{
	aArray.Reset();
	
	TBool firstDoubleQuoteFound = EFalse;
	TBool endByComma = EFalse;
	TBool Marked = EFalse;
	TLex8 tmpLex(aBuf);
	
	//Move cursor past any spaces or open brackets
	while(!tmpLex.Eos())
		{
		TChar peek=tmpLex.Peek();
		switch(peek)
			{
			//Skip the '(','[', and '{' in end
			case '(':
			case '[':
			case '{':
				break;
			//Skip the '(','[', and '{' in end
			case ')':
			case ']':
			case '}':
				break;
			case '"': 
				//Skip first '"'
				if(!firstDoubleQuoteFound)
					{
					firstDoubleQuoteFound = ETrue;
					}
				else
					{
					//Extracts the marked token(No include the '"' in end)
					firstDoubleQuoteFound = EFalse;
					TPtrC8 temp = tmpLex.MarkedToken();
					aArray.Append(temp);
					LOGTEXT2(_L8("normal parameter >%S<"),&(temp));
					Marked = EFalse;
					endByComma = EFalse;
					}
				break;
			case ',':
				if(!firstDoubleQuoteFound)
					{
					if(Marked)
						{
						//Extracts the marked token
						TPtrC8 temp = tmpLex.MarkedToken();
						aArray.Append(temp);
						LOGTEXT2(_L8("normal parameter >%S<"),&(temp));
						Marked = EFalse;
						}
					else if(endByComma)
						{
						//Add a space between two camma
						aArray.Append(KSpace());
						LOGTEXT2(_L8("normal parameter >%S<"),&(KSpace()));
						}
					endByComma = ETrue;
					}
				break;
			case ';':
			case ' ': 
			case '\r': 
			case '\n': 
				if(!firstDoubleQuoteFound&&Marked)
					{
					//Extracts the marked token
					TPtrC8 temp = tmpLex.MarkedToken();
					aArray.Append(temp);
					LOGTEXT2(_L8("normal parameter >%S<"),&(temp));
					Marked = EFalse;	
					endByComma = EFalse;
					}
				break;
			case ':': 
				if(!firstDoubleQuoteFound&&Marked)
					{
					//Extracts the marked token(Include ':')
					tmpLex.Inc();
					TPtrC8 temp = tmpLex.MarkedToken();
					aArray.Append(temp);
					LOGTEXT2(_L8("normal parameter >%S<"),&(temp));
					//back one char because add it in end 
					tmpLex.Inc(-1);
					Marked = EFalse;
					endByComma = EFalse;
					}
				break;
			default:
				if(!Marked)
					{
					tmpLex.Mark();
					Marked = ETrue;	
					}
				break;
			}
		tmpLex.Inc();
		}
	if(Marked)
		{
		//Extracts the marked token which haven't 
		TPtrC8 temp = tmpLex.MarkedToken();
		aArray.Append(temp);
		LOGTEXT2(_L8("normal parameter >%S<"),&(temp));
		}
	}
