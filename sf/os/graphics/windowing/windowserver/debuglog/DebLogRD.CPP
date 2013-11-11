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
// code that logs using RDebug::Print
// 
//

#include "DebLogRD.H"


EXPORT_C CDebugLogBase *CreateDebugLog(TBool aIsFirst, TDesC &aParams)
	{
	CDebugLogPrint *device=new(ELeave) CDebugLogPrint();
	CDebugLog *log=NULL;
	TRAPD(err,log=new(ELeave) CDebugLog(device));
	if (err!=KErrNone)
		{
		delete device;
		User::Leave(err);
		}
	TRAP(err,log->ConstructL(aIsFirst, aParams));
	if (err!=KErrNone)
		{
		delete log;
		User::Leave(err);
		}
	return(log);
	}

CDebugLogPrint::CDebugLogPrint()
	{}

CDebugLogPrint::~CDebugLogPrint()
	{}

void CDebugLogPrint::ConstructL(TBool /*aIsFirst*/, TDesC& /*aParams*/)
	{}

void CDebugLogPrint::WriteToLogL(const TDesC &aDes, const TDesC &aDes2)
	{
	TBuf<256> buf;
	TInt pos=aDes.LocateReverse(' ');
	if (pos<0)
		pos=0;
	buf.Copy(aDes.Mid(pos));
	buf.Append(' ');
	buf.Append(aDes2);
	_LIT(KDebugFormatString, "%S");
	RDebug::Print(KDebugFormatString, &buf);
	}

void CDebugLogPrint::WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2)
	{
	TBuf16<256> buf;
	TInt pos=aDes.LocateReverse(' ');
	if (pos<0)
		pos=0;
	buf.Copy(aDes.Mid(pos));
	buf.Append(' ');
	TInt bufLen=buf.Length();
	TPtr16 ptr(&buf[bufLen],buf.MaxLength()-bufLen);
	ptr.Copy(aDes2);
	buf.SetLength(bufLen+aDes2.Length());
	_LIT(KDebugFormatString, "%S");
	RDebug::Print(KDebugFormatString, &buf);	
	}

