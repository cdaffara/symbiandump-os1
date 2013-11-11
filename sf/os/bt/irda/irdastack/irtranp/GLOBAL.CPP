// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "debug.h"
#include "GLOBAL.H"

void IrTranpUtil::DAppend(TDes8* aBuffer, TUint16 aValue)
	{
	aBuffer->Append((aValue >> 8) & 0xff);
	aBuffer->Append(aValue & 0xff);
	}

/*
* Method description:	Appends a 32 bit value to a buffer
*
* Parameter:			aBuffer - the buffer to append the value to
* Parameter:			aValue - the value to append
*/

void IrTranpUtil::LAppend(TDes8* aBuffer, TUint32 aValue)
	{
	aBuffer->Append((aValue >> 24) & 0xff);
	aBuffer->Append((aValue >> 16) & 0xff);
	aBuffer->Append((aValue >> 8) & 0xff);
	aBuffer->Append(aValue & 0xff);
	}

/*
* Method description:	Extracts a 16 bit value from a buffer
*
* Parameter:			aBuffer - the buffer to extract the value from
* Parameter:			aOffset - the offset into the buffer where the value starts
* Returns:				the 16 bit value
*/

TUint16 IrTranpUtil::DExtract(const TDesC8& aBuffer, TInt aOffset)
	{
	TUint16 integer = (TUint16) ((aBuffer[aOffset] << 8) + (aBuffer[aOffset+1]));
	return integer;
	}

/*
* Method description:	Extracts a 32 bit value from a buffer
*
* Parameter:			aBuffer - the buffer to extract the value from
* Parameter:			aOffset - the offset into the buffer where the value starts
* Returns:				the 32 bit value
*/

TUint32 IrTranpUtil::LExtract(const TDesC8& aBuffer, TInt aOffset)
	{
	TUint32 integer = ((aBuffer[aOffset] << 24) + (aBuffer[aOffset+1] << 16) + (aBuffer[aOffset+2] << 8) + (aBuffer[aOffset+3]));
	return integer;
	}

/*
* Method description:	binary to ASCII converter - prints to the debugging component
*
* Parameter:			aBuffer - the buffer containg data we want in hexadecimal ASCII representation
*/

void IrTranpUtil::HexOut(const TDesC8& aBuffer)
	{
	const TUint KCharBufLen = aBuffer.Length()*2 + 1; //twice length of aBuffer + 1 for null terminator
	TText* charbuffer = new TText[KCharBufLen];
	if(!charbuffer)
		{
		_LIT(lit, "WARNING! HexOut failed to allocate sufficient memory!!");
		TPtrC outbuf(lit);
		SCEPPRINT_2(_L("%S\r\n"),&outbuf);
		return;
		}
	TInt i;
	for(i=0;i<aBuffer.Length();i++)
		{
		TInt hi = ((aBuffer[i] >> 4) & 0x0f);
		TInt lo = (aBuffer[i] & 0x0f);
		
		switch(hi)
			{
			case 0x0a:
				{
				charbuffer[i*2] = 'A';
				break;
				}
			case 0x0b:
				{
				charbuffer[i*2] = 'B';
				break;
				}
			case 0x0c:
				{
				charbuffer[i*2] = 'C';
				break;
				}
			case 0x0d:
				{
				charbuffer[i*2] = 'D';
				break;
				}
			case 0x0e:
				{
				charbuffer[i*2] = 'E';
				break;
				}
			case 0x0f:
				{
				charbuffer[i*2] = 'F';
				break;
				}
			default:
				{
				charbuffer[i*2] = TText8('9' - (9 - hi));
				break;
				}
			}
		switch(lo)
			{
			case 0x0a:
				{
				charbuffer[i*2+1] = 'A';
				break;
				}
			case 0x0b:
				{
				charbuffer[i*2+1] = 'B';
				break;
				}
			case 0x0c:
				{
				charbuffer[i*2+1] = 'C';
				break;
				}
			case 0x0d:
				{
				charbuffer[i*2+1] = 'D';
				break;
				}
			case 0x0e:
				{
				charbuffer[i*2+1] = 'E';
				break;
				}
			case 0x0f:
				{
				charbuffer[i*2+1] = 'F';
				break;
				}
			default:
				{
				charbuffer[i*2+1] = TText8('9' - (9 - lo));
				break;
				}
			}
		}
	charbuffer[i*2] = '\0';

	TPtrC outbuf(charbuffer);
	SCEPPRINT(_L("IrTranpUtil: charbuffer"));
	SCEPPRINT_2(_L("%S\r\n"),&outbuf);
	delete [] charbuffer;
	}
