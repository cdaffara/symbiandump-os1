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

LOCAL_C TDisplayMode DisplayMode(TBool aIsColor,TInt aNumColors)
	{

	if (aIsColor)
		{
		switch(aNumColors)
			{
		case 16:
			return EColor16;
		case 256:
			return EColor256;
		case 4096:
			return EColor4K;
		case 65536:
			return EColor64K;
		case 16777216:
			return CFbsDevice::DisplayMode16M();
		default:
			return ENone;
			}
		}
	else
		{
		switch(aNumColors)
			{
		case 2:
			return EGray2;
		case 4:
			return EGray4;
		case 16:
			return EGray16;
		case 256:
			return EGray256;
		default:
			return ENone;
			}
		}
	}

GLDEF_C TDisplayMode ParseDisplayMode(const TDesC& aModeName)
	{
	// Not using _LIT because we only want the string temporarily, not permanently on the heap.
	if (!aModeName.CompareF(_L("EColor16MAP")))
		return EColor16MAP;
	else if (!aModeName.CompareF(_L("EColor16MA")))
		return EColor16MA;
	else if (!aModeName.CompareF(_L("EColor16MU")))
		return EColor16MU;
	else if (!aModeName.CompareF(_L("EColor64K")))
		return EColor64K;
	else if (!aModeName.CompareF(_L("EColor4K")))
		return EColor4K;
	else if (!aModeName.CompareF(_L("EColor256")))
		return EColor256;
	else if (!aModeName.CompareF(_L("EColor16")))
		return EColor16;
	else if (!aModeName.CompareF(_L("EGray256")))
		return EGray256;
	else if (!aModeName.CompareF(_L("EGray16")))
		return EGray16;
	else if (!aModeName.CompareF(_L("EGray4")))
		return EGray4;
	else if (!aModeName.CompareF(_L("EGray2")))
		return EGray2;
	else	// old wserv magic
		{	// this code supports the old "ColorXXX" and "GrayXXX" formats etc
		TPtrC ptr(aModeName);
		TBool isColor=EFalse;

		if (ptr[0]=='C' || ptr[0]=='c')
			isColor=ETrue;
		else if (ptr[0]=='A' || ptr[0]=='a')
			return EColor16MA;
		else if (ptr[0]=='P' || ptr[0]=='p')
			return EColor16MAP;
		else if (ptr[0]!='G' && ptr[0]!='g')
			return ENone;
		
		TInt pos=0;
		while (!TChar(ptr[++pos]).IsDigit())
			{}
		TLex lex(ptr.Mid(pos));
		TInt numCols;
		if (lex.Val(numCols)!=KErrNone)
			return ENone;
		lex.SkipSpace();
		TChar units=lex.Get();
		if (units=='K' || units=='k')
			numCols<<=10;
		else if (units=='M' || units=='m')
			numCols<<=20;
		TDisplayMode mode = DisplayMode(isColor,numCols);

		#if defined(__WINS__)
			//This is a special case for the emulator since the default
			//screen packing can be overridden by the wsini.ini setting

			//This code does nothing is iDefaultDisplayMode is not
			//EColor16M.- see graphics/screendriver/swins/scnew.cpp
			//function CFbsDrawDevice::DisplayMode16M().
			if(mode==EColor16M)
				{
				TChar packed=lex.Get();
				if ((packed=='U')||(packed=='u'))
					{
					mode=EColor16MU;
					}
				}

			//However setting this will give four shades of grey since
			//the code does not support 16MU and 16M at the same time.
		#endif
		return mode;
		}
	}

