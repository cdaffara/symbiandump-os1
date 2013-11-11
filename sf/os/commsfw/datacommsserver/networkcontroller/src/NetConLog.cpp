// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file NetConLog.cpp
*/


#if defined(_DEBUG)

#include "NetConLog.h"

void NetConLog::Printf(TRefByValue<const TDesC> aFmt,...)
/**
Write a mulitple argument list to the log, trapping and ignoring any leave
*/
	{

	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KNetConLogFolder(),KNetConLogFile(),EFileLoggingModeAppend,aFmt,list);
	}

/*  The following two functions are commented out as they not currently used

void NetConLog::Write(const TDesC& aDes)
//
// Write aDes to the log
//
	{
	RFileLogger::Write(KNetConLogFolder(),KNetConLogFile(),EFileLoggingModeAppend,aDes);
	}

void NetConLog::HexDump(const TText* aHeader, const TText* aMargin, const TUint8* aPtr, TInt aLen, TInt aWidth)
	{

	TBuf<0x100> buf;
	TInt i = 0;
	const TText* p = aHeader;
	while (aLen>0)
		{
		TInt n = aLen>aWidth ? aWidth : aLen;
		if (p!=NULL)
			{
			_LIT(string1,"%s%04x : ");
			buf.AppendFormat(string1, p, i);
			}
		TInt j;
		_LIT(string2,"%02x ");
		for (j=0; j<n; j++)
			buf.AppendFormat(string2, aPtr[i+j]);
		_LIT(string3,"   ");
		while (j++<KNetConLogHexDumpWidth)
			buf.Append(string3);
		_LIT(string4," ");
		buf.Append(string4);
		_LIT(string5,"%c");
		for (j=0; j<n; j++)
			buf.AppendFormat(string5, aPtr[i+j]<32 || aPtr[i+j]>126 ? '.' : aPtr[i+j]);
		buf.Append(KEndOfLine);
		Write(buf);
		buf.SetLength(0);
		aLen -= n;
		i += n;
		p = aMargin;
		}
	}

*/

#endif // defined(_DEBUG)

