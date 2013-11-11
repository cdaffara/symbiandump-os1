// hexdump.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/ltkutils.h>
#include <e32base.h>
#include <fshell/iocli.h>
#include <e32debug.h>

static const TInt KLineSize = 16;
// 00000000:  00 01 02 03 04 05 06 07 08  09 0A 0B 0C 0D 0E 0F  ................<CR><LF>
// So line len is 8 + 3 + (3 * lineSize) + 1 + 1 + lineSize + 2
static const TInt KTotalLen = 8 + 3 + (3 * KLineSize) + 1 + 1 + KLineSize + 2;

void HexDumpLine(const TDesC8& aBuf, TDes& lineBuf, TInt aOffset)
	{
	TBuf<KLineSize> ascii;
	lineBuf.NumFixedWidthUC(aOffset, EHex, 8);
	_LIT(KColon, ":  ");
	lineBuf.Append(KColon);
	for (TInt i = 0; i < KLineSize; i++)
		{
		if (i < aBuf.Length())
			{
			TUint8 ch = aBuf[i];
			lineBuf.AppendNumFixedWidthUC(ch, EHex, 2);
			lineBuf.Append(' ');
			if (ch < 32 || ch >= 128) ch = '.';
			ascii.Append(ch);
			}
		else
			{
			_LIT(KFiller, "   ");
			lineBuf.Append(KFiller);
			}
		if (i == 7) lineBuf.Append(' '); // The space between the two groups of 8
		}
	lineBuf.Append(' ');
	lineBuf.Append(ascii);
	lineBuf.Append('\r');
	lineBuf.Append('\n');
	}

EXPORT_C HBufC* LtkUtils::HexDumpL(const TDesC8& aBuf)
	{
	TInt numLines = (aBuf.Length() + KLineSize-1) / KLineSize;
	HBufC* result = HBufC::NewLC(numLines * KTotalLen);
	TPtr resultPtr = result->Des();
	for (TInt i = 0; i < aBuf.Length(); i += KLineSize)
		{
		TPtrC8 line = aBuf.Mid(i, Min(KLineSize, aBuf.Length()-i));
		TPtr wptr((TUint16*)resultPtr.Ptr() + resultPtr.Length(), KTotalLen);
		HexDumpLine(line, wptr, i);
		resultPtr.SetLength(resultPtr.Length() + wptr.Length()); // MidTPtr doesn't update length of original
		}
	CleanupStack::Pop(result);
	return result;
	}

EXPORT_C void LtkUtils::HexDumpToOutput(const TDesC8& aBuf, RIoWriteHandle& aHandle)
	{
	TInt offset = 0;
	LtkUtils::HexDumpToOutput(aBuf, aHandle, offset);
	}

EXPORT_C void LtkUtils::HexDumpToOutput(const TDesC8& aBuf, RIoWriteHandle& aHandle, TInt& aOffset)
	{
	TBuf<KTotalLen> lineBuf;
	const TInt len = aBuf.Length();
	for (TInt i = 0; i < len; i += KLineSize)
		{
		TPtrC8 line = aBuf.Mid(i, Min(KLineSize, len-i));
		HexDumpLine(line, lineBuf, aOffset+i);
		aHandle.Write(lineBuf);
		}
	aOffset += len;
	}

EXPORT_C void LtkUtils::HexDumpToRDebug(const TDesC8& aBuf)
	{
	TInt offset = 0;
	HexDumpToRDebug(aBuf, offset);
	}

EXPORT_C void LtkUtils::HexDumpToRDebug(const TDesC8& aBuf, TInt& aOffset)
	{
	TBuf<KTotalLen> lineBuf;
	const TInt len = aBuf.Length();
	for (TInt i = 0; i < len; i += KLineSize)
		{
		TPtrC8 line = aBuf.Mid(i, Min(KLineSize, len-i));
		HexDumpLine(line, lineBuf, aOffset+i);
		RDebug::RawPrint(lineBuf);
		}
	aOffset += len;
	}

EXPORT_C void LtkUtils::RawPrint(const TDesC8 &aDes)
	{
	TPtrC8 rem(aDes);
	TBuf<256> buf;
	while (rem.Length())
		{
		buf.Copy(rem.Left(256));
		RDebug::RawPrint(buf);
		rem.Set(rem.Mid(buf.Length()));
		}
	}
