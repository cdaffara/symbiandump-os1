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
// General utility functions that don't belong anywhere else
// 
//

#include "server.h"
#include "rootwin.h"
#include "wstop.h"
#include "panics.h"

GLREF_D CDebugLogBase *wsDebugLog;

GLDEF_C RWsRegion *GetRegionFromClientL(CWsClient *aClient, TInt aCount)
	{
	TInt bufSize=sizeof(TRect)*aCount;
	TUint8* rectList=STATIC_CAST(TUint8*,User::Alloc(bufSize));
	User::LeaveIfNull(rectList);
	CleanupStack::PushL(rectList);
	TPtr8 rectBuf(rectList,bufSize);
	aClient->RemoteRead(rectBuf,0);
	RWsRegion* region=new(ELeave) RWsRegion(aCount,REINTERPRET_CAST(TRect*,rectList));
	CleanupStack::Pop(rectList);
	return(region);
	}

GLDEF_C TInt ExternalizeRegionL(RWriteStream& aWriteStream, const RWsRegion& aRegion)
	{
	TInt dataLen = sizeof(TRect)*aRegion.Count();
	aWriteStream.WriteInt32L(aRegion.Count());
	aWriteStream.WriteL(REINTERPRET_CAST(const TUint8*,aRegion.RectangleList()),dataLen);
	aWriteStream.CommitL();
	return sizeof(TInt32)+dataLen;
	}

GLDEF_C RWsRegion* InternalizeRegionL(RReadStream& aReadStream)
	{
	TInt numRects = aReadStream.ReadInt32L();
	TInt bufSize = sizeof(TRect)*numRects;
	// Allocate buffer for list of clipping rectangles. We leave if we 
	// cannot create the buffer
	TUint8* rectList = static_cast<TUint8*>(User::AllocL(bufSize));
	CleanupStack::PushL(rectList);
	// Read the list of rectangles into the buffer
	aReadStream.ReadL(rectList,bufSize);
	// Point member pointer to our new rectangle list buffer
	RWsRegion* region = new(ELeave) RWsRegion(numRects,reinterpret_cast<TRect*>(rectList));
	CleanupStack::Pop(rectList);
	return region;
	}

void Panic(TWservPanic aPanic)
	{
	if (wsDebugLog)
		wsDebugLog->Panic(CDebugLogBase::EDummyConnectionId,aPanic);		//Dummy value meaning WSERV
	_LIT(KWSERVInternalPanicCategory,"WSERV-INTERNAL");
	User::Panic(KWSERVInternalPanicCategory,aPanic);
	}

_LIT(KWSERVPanicDesc1,"WServ internal Panic %S, in file %S @ line %i");
_LIT(KWSERVPanicDesc2,"Assert condition = \"%S\"");

void PanicWithInfo(TWservPanic aPanic, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KWSERVPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);
	if (wsDebugLog)
		{
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, buf);
		}
	Panic(aPanic);
	}

void PanicWithCondAndInfo(TWservPanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KWSERVPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);
	if (wsDebugLog)
		{
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, buf);
		}
	buf.Format(KWSERVPanicDesc2, &aCondition);
	RDebug::Print(buf);
	if (wsDebugLog)
		{
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant, buf);
		}
	Panic(aPanic);
	}


GLDEF_C void StateDump()
	{
	CWsTop::StateDump();
	}

GLDEF_C void StateDump(CWsRootWindow* aRootWindow)
	{
	TBool enabled=wsDebugLog!=NULL;
	if (!enabled)
		{
		CWsTop::EnableLogging();
		if (!wsDebugLog)
			return;	//	Failed to enable logging so give up
		}
//
	_LIT(LogLine,"===========");
	_LIT(KWSERVStateLogWindowTree,"Window tree");
	TBuf<128> buf;
	wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,LogLine);
	wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,KWSERVStateLogWindowTree);
	wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,LogLine);

	CWsWindowBase *win=aRootWindow;
	TInt inset=0;
	_LIT(KWSERVStateInsetLevelValue,"%*p");
	FOREVER
		{
		buf.Format(KWSERVStateInsetLevelValue,Min(inset<<1,20));
		win->StatusDump(buf);
		wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,buf);
		if (win->BaseChild())
			{
			++inset;
			win=win->BaseChild();
			continue;
			}
		while(!win->NextSibling() && win!=aRootWindow)
			{
			win=win->BaseParent();
			--inset;
			}
		if (win==aRootWindow)
			break;		//Will break here if there is only the root window or the tree walk has returned to it
		win=win->NextSibling();
		}
	wsDebugLog->MiscMessage(CDebugLogBase::ELogImportant,LogLine);
//
	if (!enabled)
		CWsTop::DisableLogging();
	}

GLDEF_C void HeapDump()
	{
	TBool enabled=(wsDebugLog!=NULL);
	if (!enabled)
		CWsTop::EnableLogging();
	if (wsDebugLog)	// Just in case enable failed
		wsDebugLog->HeapDump();
	if (!enabled)
		CWsTop::DisableLogging();
	}

#if defined(_DEBUG)
//GLREF_C void ForceLog(TInt aPriority,const TDesC &aText,TInt aParam=0);
//ForceLog(CDebugLogBase::ELogImportant,_L("Value=%d"),345);
//TLogMessageText buf;
//_LIT(KLog,"Count=%d, Object=0x%x");
//buf.Format(KLog,Count(),this);
//ForceLog(CDebugLogBase::ELogImportant,buf);
GLDEF_C void ForceLog(TInt aPriority,const TDesC &aText,TInt aParam=0)
	{
	TBool enabled=(wsDebugLog!=NULL);
	if (!enabled)
		CWsTop::EnableLogging();
	if (wsDebugLog)	// Just in case enable failed
		{
		wsDebugLog->MiscMessage(aPriority,aText,aParam);
		if (!enabled)
			CWsTop::DisableLogging();
		}
	}
#endif

