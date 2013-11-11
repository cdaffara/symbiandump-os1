// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// code for the serial device derived class 
// 
//

#include "../SERVER/w32cmd.h"
#include "DEBLOGSR.H"

/*#if defined(__WINS__)
#pragma data_seg(".E32_UID")
__WINS_UID(0, KWservLoggingDllUidValue, 0)
#pragma data_seg()
#endif*/

//#define DEBUGLOG_SHOW_TRACE

#ifdef DEBUGLOG_SHOW_TRACE
void ShowTrace(TRefByValue<const TDesC> aFmt,...)
	{
	RDebug::Print(aFmt);
	}
#else
void ShowTrace(TRefByValue<const TDesC> /*aFmt*/,...)
	{
	}
#endif



EXPORT_C CDebugLogBase *CreateDebugLog(TBool aIsFirst, TDesC &aParams)
	{
	CDebugLogSerial *device=new(ELeave) CDebugLogSerial();
	CDebugLog *log=NULL;
	ShowTrace(_L("!!$L Allocating Log"));
	TRAPD(err,log=new(ELeave) CDebugLog(device));
	if (err!=KErrNone)
		{
		delete device;
		User::Leave(err);
		}
	ShowTrace(_L("!!$L Initialising Log"));
	TRAP(err,log->ConstructL(aIsFirst, aParams));
	if (err!=KErrNone)
		{
		delete log;
		User::Leave(err);
		}
	return(log);
	}


CDebugLogSerial::CDebugLogSerial()
	{
	__DECLARE_NAME(_S("CDebugLogSerial"));
	}

CDebugLogSerial::~CDebugLogSerial()
	{
	iSerialPort.Close();
	}

void CDebugLogSerial::ConstructL(TBool , TDesC &)
	{
#if defined(__EPOC32__)
	ShowTrace(_L("!!$L Getting Dev1"));
	User::LeaveIfError(User::LoadPhysicalDevice(_L("EUART1")));
#else
	User::LeaveIfError(User::LoadPhysicalDevice(_L("ECDRV")));
#endif
	ShowTrace(_L("!!$L Getting Dev2"));
	User::LeaveIfError(User::LoadLogicalDevice(_L("ECOMM")));
	iSerialPort.Open(0);
//
	TCommConfig cBuf;
	TCommConfigV01& c=cBuf();
	iSerialPort.Config(cBuf);
//	c.iRate=EBps19200;
	c.iRate=EBps115200;
	c.iHandshake=0;
	ShowTrace(_L("!!$L Configering Port"));
	User::LeaveIfError(iSerialPort.SetConfig(cBuf));
	}

void CDebugLogSerial::WriteToLogL(const TDesC &aDes, const TDesC &aDes2)
	{
	TBuf<LogTBufSize+2> buf(aDes);
	buf.Append(TPtrC(_S("\r\n")));
	TRequestStatus stat;
	iSerialPort.Write(stat,TPtrC8((TUint8 *)buf.Ptr(),buf.Size()));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
//
	iSerialPort.Write(stat,TPtrC8((TUint8 *)aDes2.Ptr(),aDes2.Size()));
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	}

void CDebugLogSerial::WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2)
	{
	TBuf8<LogTBufSize+2> buf(aDes);
	buf.Append(TPtrC(_S("\r\n")));
	TRequestStatus stat;
	iSerialPort.Write(stat,buf);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
//
	iSerialPort.Write(stat,aDes2);
	User::WaitForRequest(stat);
	User::LeaveIfError(stat.Int());
	}

