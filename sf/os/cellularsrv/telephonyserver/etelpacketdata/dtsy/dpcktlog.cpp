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
// DPACKETLOG.CPP
// 
//

#include "dpcktlog.h"

#ifdef __LOGGER__
#ifdef __EXE__
GLDEF_D CDTSYLogger* CDTSYLogger;
#endif

_LIT(KLogFileName,"C:\\DTSY.LOG");
#define	KGenericBufferSize	(150)

CDTSYLogger* CDTSYLogger::NewL()
	{
	CDTSYLogger* logger=new(ELeave) CDTSYLogger();
	CleanupStack::PushL(logger);
	logger->ConstructL();
	CleanupStack::Pop();
	return logger;
	}

CDTSYLogger::CDTSYLogger() : iValid(EFalse)
	{}

void CDTSYLogger::ConstructL()
/**
 *	In debug mode the logfile will not be deleted at start of new session
 */
	{
	User::LeaveIfError(iFs.Connect());
	TInt ret=KErrNone;
	ret=iFile.Open(iFs,KLogFileName,EFileShareAny|EFileWrite);
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

void CDTSYLogger::Destruct()
	{
#ifdef __EXE__
	CDTSYLogger* context=aScriptLoggerContext;
	delete context;
	aScriptLoggerContext=NULL;
#else
	CDTSYLogger* context=(CDTSYLogger*) Dll::Tls();
	delete context;
	Dll::SetTls(NULL);
#endif
	}

CDTSYLogger::~CDTSYLogger()
	{
	if(iValid)
		iFile.Close();
	iFs.Close();
	}

void CDTSYLogger::Write(const TDesC8& aText)
	{
#ifdef __EXE__
	CDTSYLogger* context=aScriptLoggerContext;
#else
	CDTSYLogger* context=(CDTSYLogger*) Dll::Tls();
#endif
	if(context==NULL)
		{
		TRAPD(err,context=CDTSYLogger::NewL());
		if (KErrNone != err)
			return;
#ifdef __EXE__
		aScriptLoggerContext=context;
#else
		Dll::SetTls(context);
#endif
		}
	if(context->iValid)
		context->WriteRecord(aText);
	}

void CDTSYLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	TBuf8<KGenericBufferSize> buf;
    VA_LIST list;
    VA_START(list,aFmt);
    buf.FormatList(aFmt,list);
	TChar tmpchar;
	for(TInt i=0;i<buf.Length();i++)
		{
		tmpchar=buf[i];
		if(!((tmpchar.IsPrint()) || (tmpchar=='\n') || (tmpchar=='\r') || (tmpchar=='\t')))
			buf[i]='.';
		}
#ifdef __EXE__
	CDTSYLogger* context=aScriptLoggerContext;
#else
	CDTSYLogger* context=(CDTSYLogger*) Dll::Tls();
#endif
	if(context==NULL)
		{
		TRAPD(err,context=CDTSYLogger::NewL());
		if (KErrNone != err)
			return;
#ifdef __EXE__
		aScriptLoggerContext=context;
#else
		Dll::SetTls(context);
#endif
		}
	if(context->iValid)
		context->WriteRecord(buf);
	}

void CDTSYLogger::WriteRecord(const TDesC8& aText)
	{
	if(iValid)
		{
		TBuf8<KGenericBufferSize> buf;
		TTime now;
		now.UniversalTime();
		TDateTime dateTime;
		dateTime = now.DateTime();
		buf.Format(_L8 ("%04d/%02d/%02d %02d.%02d:%02d:%06d "),dateTime.Year(),dateTime.Month()+1, dateTime.Day()+1,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
		buf.AppendFormat(_L8("%S\015\012"),&aText);
		iFile.Write(buf);
		iFile.Flush();
		}
	}

#endif
