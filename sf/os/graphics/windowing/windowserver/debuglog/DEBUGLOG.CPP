// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common code for all debug logging variants
// 
//

#include "../SERVER/w32cmd.h"
#include "DEBUGLOG.H"

//#define __LIMIT_LOGGING yes		//These should be turned off for released code
//#define __FORCE_ASCII yes

#if defined(__LIMIT_LOGGING)
_LIT(limitLogging,"!!");
#endif

_LIT(KDebugLogFound,"Found");
_LIT(KDebugLogSearchedFor,"Searched for");

EXPORT_C CDebugLog::CDebugLog(CDebugLogDevice *aDevice) : iDevice(aDevice), iLevel(ELogEverything)
	{
	__DECLARE_NAME(_S("CDebugLog"));
	}

CDebugLog::~CDebugLog()
	{
	delete iDevice;
	}

EXPORT_C void CDebugLog::ConstructL(TBool aIsFirst, TDesC &aParams)
	{
	_LIT(LogLine,"=================");
	iDevice->ConstructL(aIsFirst, aParams);
	iPrevTime.HomeTime();
	iExtraLen=0;	// Indicates don't print time part to log
	MiscMessage(LogLine);
	MiscMessage(_L("[Logging Enabled]"));
	TBuf<32> timeBuf;
	iPrevTime.FormatL(timeBuf,_L("[  %:0%H%:1%T%:2%S%.%*C2%:3  ]"));
	MiscMessage(timeBuf);
	MiscMessage(LogLine);
	}

void CDebugLog::CommandBuf(TInt aApp)
	{
	if (aApp==iLastApp)
		return;
	TPtrC text(iWsDecoder.CommandBuf(aApp));
	WriteToLog(text);
	iLastApp=aApp;
	}

void CDebugLog::Command(TInt aClass, TInt aOpcode, const TAny *aCmdData, TInt aHandle)
	{
	TPtrC text(iWsDecoder.Command(aClass, aOpcode, aCmdData, aHandle));
	if (text!=KNullDesC)
		WriteToLog(text);
	}

void CDebugLog::NewClient(TUint aConnectionHandle)
	{
	TPtrC text(iWsDecoder.NewClient(aConnectionHandle));
	WriteToLog(text,ELogImportant);
	}

void CDebugLog::Reply(TInt aData)
	{
	TPtrC text(iWsDecoder.CommandReply(aData));
	WriteToLog(text);
	}

void CDebugLog::ReplyBuf(const TDesC8 &aDes)
	{
	TPtrC text(iWsDecoder.CommandReplyBuf(aDes));
	if (text!=KNullDesC)
		WriteToLog(text);
	}

void CDebugLog::ReplyBuf(const TDesC16 &aDes)
	{
	TPtrC text(iWsDecoder.CommandReplyBuf(aDes));
	if (text!=KNullDesC)
		WriteToLog(text);
	}

void CDebugLog::SignalEvent(TInt aApp)
	{
	TPtrC text(iWsDecoder.SignalEvent(aApp));
	WriteToLog(text);
	}

void CDebugLog::Panic(TInt aApp, TInt aReason)
	{
	TPtrC text(iWsDecoder.Panic(aApp, aReason));
	WriteToLog(text,ELogImportant);
	}

void CDebugLog::MiscMessage(TInt aPriority,const TDesC &aFormat,TInt aParam)
	{
	if (aPriority>=iLevel)
		MiscMessage(aFormat,aParam);
	}

void CDebugLog::MiscMessage(const TDesC &aFormat,TInt aParam)
	{
	TPtrC text(iWsDecoder.MiscMessage(aFormat,aParam));
	WriteToLog(text,iLevel);
	}

void CDebugLog::IniFileSettingRead(TInt aScreen, const TDesC& aVarName, TBool aFound, const TDesC& aResult)
	{
	if (!aFound && (iLevel != ELogEverything))
		{ // nothing to print
		return;
		}
		
	TBuf<256> buf;

	buf.Format(_L("%S .ini setting { "), &(aFound ? KDebugLogFound() : KDebugLogSearchedFor()));   	

	if (aScreen >= 0)
		{
		buf.AppendFormat(_L("Screen %i, "), aScreen);
		}
	else
		{
		buf.Append(_L("Default, "));
		}

	if (aFound)
		{
		buf.AppendFormat(_L("\"%S\", \"%S\" }"), &aVarName, &aResult);
		}
	else
		{
		buf.AppendFormat(_L("\"%S\" }"), &aVarName);
		}

	MiscMessage(buf);
	}


void CDebugLog::WriteToLog(const TDesC &aDes,TInt aLevel/*=ELogEverything*/)
	{
	if (aLevel<iLevel)
		return;
#if defined(__LIMIT_LOGGING)
	TBuf<8> start(limitLogging);
	if (start!=aDes.Left(start.Length()))
		return;
#endif
	if (iErr==KErrNone)
		{
		TBuf<80> timeBuf;
		TTime time;
		time.HomeTime();
		TTimeIntervalMicroSeconds diff(time.MicroSecondsFrom(iPrevTime));
		TInt diffi = I64LOW(diff.Int64());
//
		if (iExtraLen>0)
			timeBuf.Format(_L("%+ *p%d.%03d"),iExtraLen,diffi/1000000,(diffi%1000000)/1000);
		iExtraLen=74-aDes.Length();
		if (iExtraLen<1)
			iExtraLen=1;
/* Old version, logs current time
		TBuf<256> buf(aDes);
		time.FormatL(timeBuf,_L("%:0%H%:1%T%:2%S%.%*C2%:3"));
		buf.Format(_L("%+ *p%S"),len,&timeBuf);
*/
#if defined(__FORCE_ASCII)
		TBuf8<80> timeBuf8;
		TBuf8<128> des8;
		timeBuf8.Copy(timeBuf);
		des8.Copy(aDes);
		TRAP(iErr,iDevice->WriteToLog8L(timeBuf8, des8));
#else
		TRAP(iErr,iDevice->WriteToLogL(timeBuf, aDes));
#endif
		iPrevTime.HomeTime();
		iLastApp=0;
		}
	}


void CDebugLog::HeapDump()
	{
	_LIT(LogLine,"===========");
	MiscMessage(LogLine);
	TBuf<128> buf;
	TInt biggestblock;
	buf.Format(_L("Heap size=%d, available=%d, count=%d"),User::Heap().Size(),User::Heap().Available(biggestblock),User::Heap().Count());
	MiscMessage(buf);
//
	MiscMessage(LogLine);
//
	MiscMessage(LogLine);
//
// Plus profile information for now
//
	LogProfiles();
	}

void CDebugLog::SetLoggingLevel(TInt aLevel)
	{
	iLevel=aLevel;
	}

void CDebugLog::AppendProfileNum(TDes &aDes, TInt aNum)
	{
	aDes.AppendFormat(_L("%d.%02d, "),aNum/1000000,(aNum%1000000)/10000);
	}

void CDebugLog::AppendProfileCount(TDes &aDes, TInt aNum)
	{
	aDes.AppendFormat(_L("[%d], "),aNum);
	}

void CDebugLog::LogProfiles()
	{
	}
