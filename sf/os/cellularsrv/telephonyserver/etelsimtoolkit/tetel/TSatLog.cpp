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
// Writes and formats test log information to a log file
// Main class is CTestLogger
// TSatLog.cpp is based on T_Reslog.cpp & T_Reslog.h
// 
//

/**
 @file
*/

#include "TSatlog.h"

#ifdef __LOGGER__

GLDEF_D CTestLogger* aTestLoggerContext;

#define KLogFileName _L("C:\\TSATCLI.LOG")
#define	KGenericBufferSize	(150)

CTestLogger* CTestLogger::NewL()
	{
	CTestLogger* aA=new(ELeave) CTestLogger();
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestLogger::CTestLogger() : iValid(EFalse)
	{}

void CTestLogger::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	TInt ret=iFile.Open(iFs,KLogFileName,EFileShareAny|EFileWrite);
	if(ret!=KErrNone)
		ret=iFile.Create(iFs,KLogFileName,EFileShareAny|EFileWrite);
	if(ret==KErrNone)
		{
		iValid=ETrue;
		TInt aPos=0;
		iFile.Seek(ESeekEnd,aPos);
		ret=iFile.Write(_L8("----------New Log----------\015\012"));
		}
	}

CTestLogger::~CTestLogger()
	{
	if(iValid)
		iFile.Close();
	iFs.Close();
	}

void CTestLogger::Destruct()
	{
	CTestLogger* aContext=aTestLoggerContext;
	delete aContext;
	aContext=NULL;
	}

void CTestLogger::Write(const TDesC8& aText)
	{
	CTestLogger* aContext=aTestLoggerContext;
	if(aContext==NULL)
		{
		TRAPD(ret,aContext=CTestLogger::NewL());
		if (ret==KErrNone)
			aTestLoggerContext=aContext;
		else return;
		}
	if(aContext->iValid)
		aContext->WriteRecord(aText);
	}

void CTestLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	TBuf8<KGenericBufferSize> aBuf;
    VA_LIST list;
    VA_START(list,aFmt);
    aBuf.FormatList(aFmt,list);
	TChar aChar;
	for(TInt i=0;i<aBuf.Length();i++)
		{
		aChar=aBuf[i];
		if( !((aChar.IsPrint()) || (aChar=='\n') || (aChar=='\r') || (aChar=='\t')))
			aBuf[i]='.';
		}
	CTestLogger* aContext=aTestLoggerContext;
	if(aContext==NULL)
		{
		TRAPD(ret,aContext=CTestLogger::NewL());
		if (ret==KErrNone)
			aTestLoggerContext=aContext;
		else return;
		}
	if(aContext->iValid)
		aContext->WriteRecord(aBuf);
	}
#pragma warning(disable:4710)
void CTestLogger::WriteRecord(const TDesC8& aText)
	{	
	if(iValid)
		{
		TBuf8<KGenericBufferSize> buf;
		TTime now;
		now.UniversalTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		buf.Format(_L8 ("%02d.%02d:%02d:%06d "),dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
		buf.AppendFormat(_L8("%S\015\012"),&aText);
		iFile.Write(buf);
		iFile.Flush();
		}
	}
#pragma warning(default:4710)
#endif
