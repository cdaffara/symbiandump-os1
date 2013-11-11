// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// GWHANDLE.CPP
// Test group window handle functions
// 
//

/**
 @file
 @test
*/

#include "TGWHANDLE.H"


CTGwHandle::CTGwHandle(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

void CTGwHandle::ConstructL()
	{
	}

CTGwHandle::~CTGwHandle()
	{
	}

TInt CTGwHandle::NumberOfWndGroupsWithZeroPriority(TInt aScreenNumber)
	{
	__ASSERT_ALWAYS(aScreenNumber <= 1, User::Invariant());
	return TheClient->iWs.NumWindowGroups(aScreenNumber,0);
	}

void CTGwHandle::GetGwListL(CArrayFixFlat<TInt> *aWindowHandles)
	{
	User::LeaveIfError(TheClient->iWs.WindowGroupList(aWindowHandles));
	}

void CTGwHandle::GetGwListL(CArrayFixFlat<TInt> *aWindowHandles, TInt aScreenNumber)
	{
	User::LeaveIfError(TheClient->iWs.WindowGroupList(aWindowHandles, aScreenNumber));
	}

void CTGwHandle::GetGwListL(TInt aPriority, CArrayFixFlat<TInt> *aWindowHandles)
	{
	User::LeaveIfError(TheClient->iWs.WindowGroupList(aPriority,aWindowHandles));
	}

void CTGwHandle::GetGwListL(RArray<RWsSession::TWindowGroupChainInfo>* aWindowHandles)
	{
	User::LeaveIfError(TheClient->iWs.WindowGroupList(aWindowHandles));
	}

void CTGwHandle::GetGwListL(TInt aPriority, RArray<RWsSession::TWindowGroupChainInfo>* aWindowHandles)
	{
	User::LeaveIfError(TheClient->iWs.WindowGroupList(aPriority, aWindowHandles));
	}

void CTGwHandle::GwNamesL()
	{
	enum {EConnectHandle1=123456};
	enum {EWindowGroupHandle1=234567};
	enum {EWindowGroupHandle2=666666};
//
	TInt foreground;
	User::LeaveIfError(foreground=TheClient->iWs.GetFocusWindowGroup());
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(EWindowGroupHandle1));
	gw1.SetOwningWindowGroup(foreground);
	RWindowGroup gw2(ws1);
	User::LeaveIfError(gw2.Construct(EWindowGroupHandle2));
	gw2.SetOwningWindowGroup(foreground);
//
	TBuf<8> getName;
//
// Check default name is simply a null string
//
	TInt retVal = gw1.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("gw1.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==_L(""));
	if (getName!=_L(""))
		INFO_PRINTF3(_L("getName==_L() - Expected: %d, Actual: %d"), ETrue, EFalse);
//
	TBuf<8> name1(_L("A_Name1"));
	TBuf<8> name2(_L("A_Name2"));
//
	retVal = gw1.SetName(name1);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("gw1.SetName(name1) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw2.SetName(name2);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.SetName(name2) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw1.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw1.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==name1);
	if (getName!=name1)
		INFO_PRINTF3(_L("getName==name1 - Expected: %d, Actual: %d"), ETrue, EFalse);
//
	retVal = gw2.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==name2);
	if (getName!=name2)
		INFO_PRINTF3(_L("getName==name2 - Expected: %d, Actual: %d"), ETrue, EFalse);

//
// A few weird characters in the name should work fine
//
	retVal = gw2.SetName(_L("xx*"));
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.SetName(_L(xx*)) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw2.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==_L("xx*"));
	if (getName!=_L("xx*"))
		INFO_PRINTF3(_L("getName==_L(xx*) - Expected: %d, Actual: %d"), ETrue, EFalse);

//
	retVal = gw2.SetName(_L(":"));
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.SetName(_L(:)) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw2.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("(gw2.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==_L(":"));
	if (getName!=_L(":"))
		INFO_PRINTF3(_L("getName==_L(:) - Expected: %d, Actual: %d"), ETrue, EFalse);

//
// Setting the name back to a null name
//
	retVal = gw2.SetName(_L(""));
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("gw2.SetName(_L()) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw2.Name(getName);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("gw2.Name(getName) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(getName==_L(""));
	if (getName!=_L(""))
		INFO_PRINTF3(_L("getName==_L() - Expected: %d, Actual: %d"), ETrue, EFalse);

//
// Getting a name into a too short buffer should fill thew whole buffer from the left hand part
// of the window name and return KErrOverflow
//
	gw2.SetName(name2);
	TBuf<4> shortBuf;
	retVal = gw2.Name(shortBuf);
	TEST(retVal==KErrOverflow);
	if (retVal!=KErrOverflow)
		INFO_PRINTF3(_L("gw2.Name(shortBuf) return value - Expected: %d, Actual: %d"), KErrOverflow, retVal);

	TEST(shortBuf==name2.Left(4));
	if (shortBuf!=name2.Left(4))
		INFO_PRINTF3(_L("shortBuf==name2.Left(4) - Expected: %d, Actual: %d"), ETrue, EFalse);

//
// passing a 0 length TPtr pointing to NULL should be safe, simply returning KErrOverflow
//
	TPtr nullPtr(NULL,0);
	retVal = gw2.Name(nullPtr);
	TEST(retVal==KErrOverflow);
	if (retVal!=KErrOverflow)
		INFO_PRINTF3(_L("gw2.Name(nullPtr) return value - Expected: %d, Actual: %d"), KErrOverflow, retVal);

	TEST(nullPtr==_L(""));
	if (nullPtr!=_L(""))
		INFO_PRINTF3(_L("nullPtr==_L() - Expected: %d, Actual: %d"), ETrue, EFalse);

//
// Getting a null name into a null descriptor
//
	gw2.SetName(_L(""));
	TPtr nullPtr2(NULL,0);
	retVal = gw2.Name(nullPtr);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("gw2.Name(nullPtr) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	TEST(nullPtr2==_L(""));
	if (nullPtr2!=_L(""))
		INFO_PRINTF3(_L("nullPtr2==_L() - Expected: %d, Actual: %d"), ETrue, EFalse);
//
	gw1.Close();
	gw2.Close();
	CleanupStack::PopAndDestroy(screen);
	ws1.Close();
	}

void CTGwHandle::GwIdentifierTestL()
	{
	enum {EWindowGroupHandle1=234567};
	enum {EWindowGroupHandle2=666666};
	enum {EWindowGroupHandle3=999};
//
	CArrayFixFlat<TInt>* windowHandles=new(ELeave) CArrayFixFlat<TInt>(5);
	CleanupStack::PushL(windowHandles);
	GetGwListL(0, windowHandles);
	TInt retVal = TheClient->iWs.GetFocusWindowGroup();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("TheClient->iWs.GetFocusWindowGroup() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);

//
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());
	CleanupClosePushL(ws1);

	// assign to the correct screen
	CWsScreenDevice* screen1 = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen1);
	User::LeaveIfError(screen1->Construct(iTest->iScreenNumber));

	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(EWindowGroupHandle1));
	gw1.SetOwningWindowGroup((*windowHandles)[0]);
	RWsSession ws2;
	User::LeaveIfError(ws2.Connect());
	CleanupClosePushL(ws2);

	// assign to the correct screen
	CWsScreenDevice* screen2 = new (ELeave) CWsScreenDevice(ws2);
	CleanupStack::PushL(screen2);
	User::LeaveIfError(screen2->Construct(iTest->iScreenNumber));

	RWindowGroup gw2(ws2);
	User::LeaveIfError(gw2.Construct(EWindowGroupHandle2));
//
	GetGwListL(0, windowHandles);
//

	retVal = gw2.Identifier();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("gw2.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]);
	TEST(retVal==EWindowGroupHandle2);
	if (retVal!=EWindowGroupHandle2)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle2, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]);
	TEST(retVal==EWindowGroupHandle1);
	if (retVal!=EWindowGroupHandle1)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle1, retVal);
//
	retVal = TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[1]);
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[0]);
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), 0, retVal);

//
	TThreadId threadId;
	TheClient->iWs.GetWindowGroupClientThreadId((*windowHandles)[0],threadId);

	TUint64 ret = RThread().Id();
	TEST(ret==threadId);
	if (ret!=threadId)
		INFO_PRINTF3(_L("RThread().Id() return value - Expected: %d, Actual: %d"), (TInt)threadId, (TInt)ret);

	TheClient->iWs.GetWindowGroupClientThreadId((*windowHandles)[1],threadId);

	TUint64 retVal2 = RThread().Id();
	TEST(retVal2==threadId);
	if (retVal2!=threadId)
		INFO_PRINTF3(_L("RThread().Id() return value - Expected: %d, Actual: %d"), (TInt)threadId, (TInt)retVal2);
//
	retVal = gw1.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("gw1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = gw2.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("gw2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[1],0);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[1],0) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw1.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("gw1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = gw2.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("gw2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);
//
	retVal = TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[1],1);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[1],1) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	retVal = gw1.OrdinalPosition();
	TEST(retVal==1);
	if (retVal!=1)
		INFO_PRINTF3(_L("gw1.OrdinalPosition() return value - Expected: %d, Actual: %d"), 1, retVal);

	retVal = gw2.OrdinalPosition();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("gw2.OrdinalPosition() return value - Expected: %d, Actual: %d"), 0, retVal);

	retVal = TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[0],1);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[0],1) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	GetGwListL(0, windowHandles);
	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]);
	TEST(retVal==EWindowGroupHandle1);
	if (retVal!=EWindowGroupHandle1)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle1, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]);
	TEST(retVal==EWindowGroupHandle2);
	if (retVal!=EWindowGroupHandle2)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle2, retVal);
//
	RWindowGroup gw3(ws2);
	User::LeaveIfError(gw3.Construct(EWindowGroupHandle3));
	gw1.SetOrdinalPosition(0,100000);	// Assume nothing else in the system is higher, else test will fail
	gw3.SetOrdinalPosition(0,100000);
	ws1.Flush();
	ws2.Flush();
//
	GetGwListL(100000, windowHandles);
	retVal = windowHandles->Count();
	TEST(retVal==2);
	if (retVal!=2)
		INFO_PRINTF3(_L("windowHandles->Count() return value - Expected: %d, Actual: %d"), 2, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]);
	TEST(retVal==EWindowGroupHandle3);
	if (retVal!=EWindowGroupHandle3)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle3, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]);
	TEST(retVal==EWindowGroupHandle1);
	if (retVal!=EWindowGroupHandle1)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle1, retVal);

//
	retVal = TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[0]);
	TEST(retVal==100000);
	if (retVal!=100000)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), 100000, retVal);

	retVal = TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[1]);
	TEST(retVal==100000);
	if (retVal!=100000)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupOrdinalPriority((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), 100000, retVal);
//
	GetGwListL(-12453, windowHandles);	// Test 0 count, Assumes no window with priority
	retVal = windowHandles->Count();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("windowHandles->Count() return value - Expected: %d, Actual: %d"), 0, retVal);

//
	GetGwListL(windowHandles, iTest->iScreenNumber);
	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]);
	TEST(retVal==EWindowGroupHandle3);
	if (retVal!=EWindowGroupHandle3)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[0]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle3, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]);
	TEST(retVal==EWindowGroupHandle1);
	if (retVal!=EWindowGroupHandle1)
		INFO_PRINTF3(_L("TheClient->iWs.GetWindowGroupHandle((*windowHandles)[1]) return value - Expected: %d, Actual: %d"), EWindowGroupHandle1, retVal);
//
// Check passing bad identifiers
//
	retVal = TheClient->iWs.SetWindowGroupOrdinalPosition(-1,0);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition(-1,0) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

	retVal = TheClient->iWs.GetWindowGroupHandle(-3);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("TTheClient->iWs.GetWindowGroupHandle(-3) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

//
	gw1.Close();
	gw2.Close();
	gw3.Close();
	
	CleanupStack::PopAndDestroy(5, windowHandles);
	}

void CTGwHandle::OwnerToForegroundL()
	{
	enum {EWindowGroupHandle1=234567};
	enum {EWindowGroupHandle2a=666666};
	enum {EWindowGroupHandle2b=666667};
//
	CArrayFixFlat<TInt> *windowHandles=new(ELeave) CArrayFixFlat<TInt>(5);
	CleanupStack::PushL(windowHandles);
	GetGwListL(0, windowHandles);
	TInt oldForeground=(*windowHandles)[0];
//
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());
	CleanupClosePushL(ws1);

	// assign to the correct screen
	CWsScreenDevice* screen1 = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen1);
	User::LeaveIfError(screen1->Construct(iTest->iScreenNumber));

	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(EWindowGroupHandle1));
	CleanupClosePushL(gw1);
	RWsSession ws2;
	User::LeaveIfError(ws2.Connect());
	CleanupClosePushL(ws2);

	// assign to the correct screen
	CWsScreenDevice* screen2 = new (ELeave) CWsScreenDevice(ws2);
	CleanupStack::PushL(screen2);
	User::LeaveIfError(screen2->Construct(iTest->iScreenNumber));

	RWindowGroup gw2a(ws2);
	User::LeaveIfError(gw2a.Construct(EWindowGroupHandle2a));
	CleanupClosePushL(gw2a);
	RWindowGroup gw2b(ws2);
	User::LeaveIfError(gw2b.Construct(EWindowGroupHandle2b));
	CleanupClosePushL(gw2b);
	gw1.SetOrdinalPosition(1);
//
// Turn auto flushing on as we're using multiple connections and it would be easy to forget to flush one
//
	ws1.SetAutoFlush(ETrue);
	ws2.SetAutoFlush(ETrue);
	TheClient->iWs.SetAutoFlush(ETrue);
//
	GetGwListL(0, windowHandles);
//
	TInt  retVal = gw2b.Identifier();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("gw2b.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

	retVal = gw2a.Identifier();
	TEST(retVal==(*windowHandles)[2]);
	if (retVal!=(*windowHandles)[2])
		INFO_PRINTF3(_L("gw2a.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[2], retVal);

	retVal = oldForeground;
	TEST(retVal==(*windowHandles)[3]);
	if (retVal!=(*windowHandles)[3])
		INFO_PRINTF3(_L("oldForeground - Expected: %d, Actual: %d"), (*windowHandles)[3], oldForeground);
//
	CleanupStack::PopAndDestroy(&gw2b);
	GetGwListL(0, windowHandles);

	retVal = gw2a.Identifier();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("gw2a.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);
	// Check 2a jumped over 1 in the ordinal list

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

	retVal = oldForeground;
	TEST(retVal==(*windowHandles)[2]);
	if (retVal!=(*windowHandles)[2])
		INFO_PRINTF3(_L("oldForeground - Expected: %d, Actual: %d"), (*windowHandles)[2], oldForeground);

//
	User::LeaveIfError(gw2b.Construct(EWindowGroupHandle2b));
	CleanupClosePushL(gw2b);
	gw2b.SetOwningWindowGroup(oldForeground);
	CleanupStack::PopAndDestroy(&gw2b);
	GetGwListL(0, windowHandles);
	retVal = oldForeground;
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("oldForeground - Expected: %d, Actual: %d"), (*windowHandles)[0], oldForeground);
	// Check old foreground bought to foreground

	retVal = gw2a.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw2a.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[2]);
	if (retVal!=(*windowHandles)[2])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[2], retVal);

	TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[0],2);
//
	User::LeaveIfError(gw2b.Construct(EWindowGroupHandle2b));
	CleanupClosePushL(gw2b);
	gw2b.SetOwningWindowGroup((*windowHandles)[1]);
	gw2b.SetOrdinalPosition(KOrdinalPositionSwitchToOwningWindow);	// Should bring 2a to foreground
	GetGwListL(0, windowHandles);

	retVal = gw2a.Identifier();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("gw2a.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);

	retVal = gw2b.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw2b.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[2]);
	if (retVal!=(*windowHandles)[2])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[2], retVal);
//
	CleanupStack::PopAndDestroy(&gw2b);
	GetGwListL(0, windowHandles);
	retVal = gw2a.Identifier();
	TEST(retVal==(*windowHandles)[0]);
	if (retVal!=(*windowHandles)[0])
		INFO_PRINTF3(_L("gw2a.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[0], retVal);

	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[1]);
	if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);
//
	CleanupStack::PopAndDestroy(&gw2a);		// Should bring shell to foreground
	GetGwListL(0, windowHandles);

// check whether the shell is on
	TThreadId threadId;
	ws2.GetWindowGroupClientThreadId((*windowHandles)[0],threadId);
	TBool shellRunning=ETrue;

	TFullName shellName;
	// apply to primary screen only where SHELL is running
	TFindThread findThread(iTest->iScreenNumber==KDefaultScreen?_L("WSHELL*::*"):_L("WxSHELL*::*"));
	if(findThread.Next(shellName)==KErrNone)
		{
		RThread shell;
		User::LeaveIfError(shell.Open(shellName));
		TEST(shell.Id()==threadId);
		if (retVal!=(*windowHandles)[1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[1], retVal);

		shell.Close();
		}
	else
		{
		shellRunning=EFalse;
		}


	// Check gw1 second
	retVal = gw1.Identifier();
	TEST(retVal==(*windowHandles)[shellRunning ? 1 : 0]);
	if (retVal!=(*windowHandles)[shellRunning ? 1 : 0])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[shellRunning ? 1 : 0], retVal);

	retVal = oldForeground;
	TEST(retVal==(*windowHandles)[shellRunning ? 2 : 1]);
	if (retVal!=(*windowHandles)[shellRunning ? 2 : 1])
		INFO_PRINTF3(_L("gw1.Identifier() return value - Expected: %d, Actual: %d"), (*windowHandles)[shellRunning ? 2 : 1], retVal);
	// and oldforeground second

//
	CleanupStack::PopAndDestroy(5, &ws1);
//
// Kick shell back into background, 'cause that's where it belongs
//
	TheClient->iWs.SetWindowGroupOrdinalPosition((*windowHandles)[0],-1);
	TheClient->iWs.SetAutoFlush(EFalse);
//
	CleanupStack::PopAndDestroy(windowHandles);
	}

void CTGwHandle::FindWindowGroupIdentifierTestL()
	{
	enum {EWindowGroupHandle1=234567};
	enum {EWindowGroupHandle2=666666};
	enum {EWindowGroupHandle3=123};
	TInt foreground=TheClient->iWs.GetFocusWindowGroup();
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));
	CleanupStack::Pop(screen);

	CleanupClosePushL(ws1);
//
	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(EWindowGroupHandle1));
	CleanupClosePushL(gw1);
	gw1.SetOwningWindowGroup(foreground);
	RWindowGroup gw2(ws1);
	User::LeaveIfError(gw2.Construct(EWindowGroupHandle2));
	CleanupClosePushL(gw2);
	gw1.SetOwningWindowGroup(foreground);
	RWindowGroup gw3(ws1);
	User::LeaveIfError(gw3.Construct(EWindowGroupHandle3));
	CleanupClosePushL(gw3);
	gw3.SetOwningWindowGroup(foreground);
//
	TInt id1=gw1.Identifier();
	TInt id2=gw2.Identifier();
	TInt id3=gw3.Identifier();
//
	gw1.SetName(_L("qwerty123abcd"));
	gw2.SetName(_L("123"));
	gw3.SetName(_L("qqqabcdxxx123"));
//
	TInt retVal = ws1.FindWindowGroupIdentifier(0,_L("123"),0);
	TEST(retVal==id2);
	if (retVal!=id2)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(123),0) return value - Expected: %d, Actual: %d"), id2, retVal);

//
	retVal = ws1.FindWindowGroupIdentifier(0,_L("*123*"),0);
	TEST(retVal==id3);
	if (retVal!=id3)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(*123*),0) return value - Expected: %d, Actual: %d"), id3, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id3,_L("*123*"),0);
	TEST(retVal==id2);
	if (retVal!=id2)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id3,_L(*123*),0) return value - Expected: %d, Actual: %d"), id2, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id2,_L("*123*"),0);
	TEST(retVal==id1);
	if (retVal!=id1)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id2,_L(*123*),0) return value - Expected: %d, Actual: %d"), id1, retVal);

//
	retVal = ws1.FindWindowGroupIdentifier(0,_L("*abcd*"),0);
	TEST(retVal==id3);
	if (retVal!=id3)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(*abcd*),0) return value - Expected: %d, Actual: %d"), id3, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id3,_L("*abcd*"),0);
	TEST(retVal==id1);
	if (retVal!=id1)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id3,_L(*abcd*),0)return value - Expected: %d, Actual: %d"), id1, retVal);
//
	retVal = ws1.FindWindowGroupIdentifier(0,_L("*123*"),7);
	TEST(retVal==id3);
	if (retVal!=id3)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(*123*),7)return value - Expected: %d, Actual: %d"), id3, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id3,_L("*123*"),7);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id3,_L(*123*),7) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

	retVal = ws1.FindWindowGroupIdentifier(0,_L("*abc*"),4);
	TEST(retVal==id1);
	if (retVal!=id1)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(*abc*),4) return value - Expected: %d, Actual: %d"), id1, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id1,_L("*abc*"),4);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id1,_L(*abc*),4) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

	retVal = ws1.FindWindowGroupIdentifier(0,_L("xxx*"),7);
	TEST(retVal==id3);
	if (retVal!=id3)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(xxx*),7) return value - Expected: %d, Actual: %d"), id3, retVal);

	retVal = ws1.FindWindowGroupIdentifier(0,_L("xxx*"),8);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(0,_L(xxx*),8) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);
//
	retVal = ws1.FindWindowGroupIdentifier(id1,_L("12"),0);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id1,_L(12),0) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id1,_L("qqq"),0);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id1,_L(qqq),0) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);

	retVal = ws1.FindWindowGroupIdentifier(id1,_L("abcd"),3);
	TEST(retVal==KErrNotFound);
	if (retVal!=KErrNotFound)
		INFO_PRINTF3(_L("ws1.FindWindowGroupIdentifier(id1,_L(abcd),3) return value - Expected: %d, Actual: %d"), KErrNotFound, retVal);
//
	delete screen;
	CleanupStack::PopAndDestroy(4, &ws1);
	}

void CTGwHandle::DefaultOwningWindowL()
	{
	TInt foreground=TheClient->iWs.GetFocusWindowGroup();
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

//
	INFO_PRINTF1(_L("DefaultOwningWindow1"));
	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(1));
	gw1.SetOwningWindowGroup(foreground);
	RWindowGroup gw2(ws1);
	User::LeaveIfError(gw2.Construct(2));
	gw2.SetOwningWindowGroup(foreground);
	RWindowGroup gw3(ws1);
	User::LeaveIfError(gw3.Construct(3));
	gw3.SetOwningWindowGroup(foreground);
	RWindowGroup gw4(ws1);
	User::LeaveIfError(gw4.Construct(4));
	gw4.SetOwningWindowGroup(foreground);
	INFO_PRINTF1(_L(" Created Group Window"));
//
	TInt prevOwningGroup=ws1.GetDefaultOwningWindow();
	gw1.DefaultOwningWindow();
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	gw2.DefaultOwningWindow();
	TEST(gw2.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw2.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw2.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw2.Identifier());

	gw2.Close();
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	gw3.DefaultOwningWindow();
	gw4.DefaultOwningWindow();
	gw3.Close();
	TEST(gw4.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw4.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw4.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw4.Identifier());

	gw4.Close();
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	gw1.Close();
	TEST(prevOwningGroup==ws1.GetDefaultOwningWindow());
	if (prevOwningGroup!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("prevOwningGroup==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), prevOwningGroup);

	INFO_PRINTF1(_L(" Closed Group Window"));

	User::LeaveIfError(gw1.Construct(1));
	gw1.SetOwningWindowGroup(foreground);
	User::LeaveIfError(gw2.Construct(2));
	gw2.SetOwningWindowGroup(foreground);
	User::LeaveIfError(gw3.Construct(3));
	gw3.SetOwningWindowGroup(foreground);
	INFO_PRINTF1(_L(" ReCreated Group Window"));
	gw1.DefaultOwningWindow();
	gw1.DefaultOwningWindow();
 	gw1.Close();
	gw2.DefaultOwningWindow();
	gw2.DefaultOwningWindow();
	gw3.Close();
	gw2.Close();
	INFO_PRINTF1(_L(" Re-Closed Group Window"));

	User::LeaveIfError(gw1.Construct(1));
	gw1.SetOwningWindowGroup(foreground);
	User::LeaveIfError(gw2.Construct(2));
	gw2.SetOwningWindowGroup(foreground);
	User::LeaveIfError(gw3.Construct(3));
	gw3.SetOwningWindowGroup(foreground);
	User::LeaveIfError(gw4.Construct(4));
	gw4.SetOwningWindowGroup(foreground);
	INFO_PRINTF1(_L(" ReCreated Group Window Again"));
	gw1.DefaultOwningWindow();
	gw2.DefaultOwningWindow();
	gw1.DefaultOwningWindow();
 	gw1.Close();
	gw2.Close();
	gw3.DefaultOwningWindow();
	gw4.DefaultOwningWindow();
	gw3.DefaultOwningWindow();
 	gw4.Close();
	gw3.Close();
	INFO_PRINTF1(_L(" Re-Closed Group Window Again"));

	User::LeaveIfError(gw1.Construct(1));
	gw1.DefaultOwningWindow();
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	gw1.DefaultOwningWindow();
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	User::LeaveIfError(gw2.Construct(2));
	TEST(gw1.Identifier()==ws1.GetDefaultOwningWindow());
	if (gw1.Identifier()!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1.Identifier()==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1.Identifier());

	gw2.Close();
	gw1.Close();

	CleanupStack::PopAndDestroy(screen);
	ws1.Close();
	}

void CTGwHandle::DefaultOwningWindow2L()
//This test originally resulted in a crash, while the above test originally resulted in lockups.
	{
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen1 = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen1);
	User::LeaveIfError(screen1->Construct(iTest->iScreenNumber));

	RWsSession ws2;
	User::LeaveIfError(ws2.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen2 = new (ELeave) CWsScreenDevice(ws2);
	CleanupStack::PushL(screen2);
	User::LeaveIfError(screen2->Construct(iTest->iScreenNumber));

//
	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(1));
	RWindowGroup gw2(ws2);
	User::LeaveIfError(gw2.Construct(2));
	RWindowGroup gw3(ws1);
	User::LeaveIfError(gw3.Construct(3));
//
	const TInt gw1Id=gw1.Identifier();
	gw1.DefaultOwningWindow();
	gw2.DefaultOwningWindow();
	/*const TInt gw2Id=*/gw2.Identifier();
	gw1.DefaultOwningWindow();
	TEST(gw1Id==ws1.GetDefaultOwningWindow());
	if (gw1Id!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1Id==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1Id);

	gw3.DefaultOwningWindow();
	gw3.Close();
	TEST(gw1Id==ws1.GetDefaultOwningWindow());
	if (gw1Id!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw1Id==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw1Id);

	gw1.Close();
	TInt gw2Id=gw2.Identifier();
	TEST(gw2Id==ws1.GetDefaultOwningWindow());
	if (gw2Id!=ws1.GetDefaultOwningWindow())
		INFO_PRINTF3(_L("gw2Id==ws1.GetDefaultOwningWindow() - Expected: %d, Actual: %d"), ws1.GetDefaultOwningWindow(), gw2Id);

	gw2.SetOrdinalPosition(0);
	gw2.Close();
	ws2.Flush();
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EKeyDown,32);
	TheClient->iWs.SimulateRawEvent(rawEvent);
	TheClient->iWs.Flush();
//
	CleanupStack::PopAndDestroy(2,screen1);
	ws1.Close();
	ws2.Close();
	}

#define FREQ 500
void CTGwHandle::IdentifierWrapAroundTestL()
	{
	TInt report=-1;
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(1));
	gw1.EnableReceiptOfFocus(EFalse);
	TInt id1=gw1.Identifier();
	RWindowGroup gw2(ws1);
	User::LeaveIfError(gw2.Construct(2));
	gw2.EnableReceiptOfFocus(EFalse);
	TInt id2=gw2.Identifier();
	gw2.Close();	// Allow gw2 id to be re-used
	TInt id3;

	do
		{
		RWindowGroup gw3(ws1);
		User::LeaveIfError(gw3.Construct(2));
		gw3.EnableReceiptOfFocus(EFalse);
		id3=gw3.Identifier();
		if ((id3/FREQ)!=(report/FREQ))	// || id3<=10)
			{
			report=id3;
			iTest->LogMessage(id3);
			}
		TEST(id3!=id1);	// Check id1 is not re-used as the window hasn't been freed yet
		if (id3==id1)
			{
			_LIT(KErr,"Window Group Id Reused!");
			LOG_MESSAGE(KErr);
			}
		gw3.Close();
		} while(id3!=id2);

	CleanupStack::PopAndDestroy(screen);
	ws1.Close();
	}

void GwHandleDestroyWindow(TAny* aWindow)
	{
	static_cast<RWindowTreeNode*>(aWindow)->Destroy();
	}

RWindowGroup* CTGwHandle::CreateWindowGroupLC(RWsSession& aWs,TUint32 aClientHandle,TInt aParentId/*=0*/)
	{
	RWindowGroup* gw=new(ELeave) RWindowGroup(aWs);
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,gw));
	if (aParentId>0)
		{
		User::LeaveIfError(gw->ConstructChildApp(aParentId,aClientHandle,EFalse));
		}
	else
		{
		User::LeaveIfError(gw->Construct(aClientHandle,EFalse));
		}
	return gw;
	}

void CTGwHandle::CreateGroupWindowsLC(RWsSession& aWs)
	{
	for(TInt ii=ENumGroups-1;ii>=0;--ii)
		iGroups[ii]=CreateWindowGroupLC(aWs,ii);
	}

void CTGwHandle::CreateChainedGroupWindowsLC(RWsSession& aWs,TBool aSecondChain)
	{
	TInt parentId=0;
	for(TInt ii=ENumChained-1;ii>=0;--ii)
		{
		iChained[ii]=CreateWindowGroupLC(aWs,ENumGroups+ii,parentId);
		parentId=iChained[ii]->Identifier();
		}
	if(aSecondChain)
		{
		parentId =0;
		for(TInt ii=ENumChained-1;ii>=0;--ii)
			{
			iChained2[ii]=CreateWindowGroupLC(aWs,ENumGroups+ENumGroups+ii,parentId);
			parentId=iChained2[ii]->Identifier();
			}
		}
	}

void CTGwHandle::MoveGroups(RWindowGroup** aGroups,TInt aGp,TInt aNum,TInt aPos,TInt aInc/*=2*/)
	{
	TInt ii;
	for(ii=aNum;ii>0;--ii)
		{
		aGroups[aGp]->SetOrdinalPosition(aPos);
		aGp+=(aInc>0?1:-1);
		aPos+=aInc;
		}
	}

void CTGwHandle::TestGroups(TInt aPos)
	{
	TInt retVal;
	TInt ii = 0;
	for(ii=0;ii<ENumChained;++ii)
		{
		retVal = iChained[ii]->OrdinalPosition();
		TEST(retVal==aPos+ii);
		if (retVal!=aPos+ii)
			INFO_PRINTF3(_L("iChained[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), aPos+ii, retVal);
		}

	for(ii=0;ii<ENumGroups;++ii)
		{
		retVal = iGroups[ii]->OrdinalPosition();
		TInt ret = (ii<aPos?ii:ii+ENumChained);
		TEST(retVal==ret);
		if (retVal!=ret)
			INFO_PRINTF3(_L("iGroups[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), ret, retVal);
		}
	}

void CTGwHandle::TestGroupsBefore(TInt aPos)
	{
	TInt ii;
	TInt retVal;
	if (aPos>0)
		{
		for (ii=0;ii<aPos;++ii)
			{
			retVal = iGroups[ii]->OrdinalPosition();
			TEST(retVal==ii);
			if (retVal!=ii)
				INFO_PRINTF3(_L("iGroups[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), ii, retVal);
			}
		}
	for (ii=0;ii<ENumChained;++ii)
		{
		retVal = iChained[ii]->OrdinalPosition();
		TEST(retVal==aPos+2*ii);
		if (retVal!=aPos+2*ii)
			INFO_PRINTF3(_L("iChained[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), aPos+2*ii, retVal);
		}
	for (ii=0;ii<ENumChained-1;++ii)
		{
		retVal = iGroups[aPos+ii]->OrdinalPosition();
		TEST(retVal==aPos+1+2*ii);
		if (retVal!=aPos+1+2*ii)
			INFO_PRINTF3(_L("iGroups[aPos+ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), aPos+1+2*ii, retVal);
		}
	for (ii=aPos+ENumChained-1;ii<ENumGroups;++ii)
		{
		retVal = iGroups[ii]->OrdinalPosition();
		TEST(retVal==ii+ENumChained);
		if (retVal!=ii+ENumChained)
			INFO_PRINTF3(_L("iGroups[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), ii+ENumChained, retVal);
		}
	}

void CTGwHandle::WindowGroupChaining()
	{
	RWsSession& ws=TheClient->iWs;
	CreateGroupWindowsLC(ws);
	CreateChainedGroupWindowsLC(ws);
	TestGroups(0);
	TInt start,target,moveGp;
	for(moveGp=0;moveGp<ENumChained;++moveGp)
		{
		for(target=0;target<ENumGroups+1;++target)
			{
			for(start=0;start<ENumGroups-ENumChained+2;++start)
				{
				if (target>start+1)
					MoveGroups(iGroups,target-1,target-start-1,ENumChained+target-1,-1);
				if (target<start && start>0)
					MoveGroups(iGroups,0,start,0,1);
				MoveGroups(iGroups,start,ENumChained-1,start+1);
				TestGroupsBefore(start);
				iChained[moveGp]->SetOrdinalPosition(target+moveGp);
				TestGroups(target);
				}
			}
		iChained[0]->SetOrdinalPosition(0);
		TestGroups(0);
		}
	CleanupStack::PopAndDestroy(ENumGroups+ENumChained,iGroups[ENumGroups-1]);
	}

void CTGwHandle::WindowGroupChaining2()
	{
	RWsSession& ws=TheClient->iWs;
	RWindowGroup* chain1=CreateWindowGroupLC(ws,1);
	chain1->SetOrdinalPosition(0,1);
	RWindowGroup* chain2=CreateWindowGroupLC(ws,2,chain1->Identifier());
	TInt retVal = chain2->OrdinalPriority();
	TEST(retVal==1);
	if (retVal!=1)
			INFO_PRINTF3(_L("chain2->OrdinalPriority() return value - Expected: %d, Actual: %d"), 1, retVal);

	chain1->SetOrdinalPosition(0,2);
	retVal = chain2->OrdinalPriority();
	TEST(retVal==2);
	if (retVal!=2)
			INFO_PRINTF3(_L("chain2->OrdinalPriority() return value - Expected: %d, Actual: %d"), 2, retVal);

	chain2->SetOrdinalPosition(0,3);
	retVal = chain1->OrdinalPriority();
	TEST(retVal==3);
	if (retVal!=3)
			INFO_PRINTF3(_L("chain1->OrdinalPriority() return value - Expected: %d, Actual: %d"), 3, retVal);

	CleanupStack::PopAndDestroy(2,chain1);
	
	//Test that two different sessions can't chain window-groups without permisson.
	//Both sessions must use the same screen.
	RWsSession ws2;
	User::LeaveIfError(ws2.Connect());
	CleanupClosePushL(ws2);
	chain1=CreateWindowGroupLC(ws2,1);
	TInt id1=chain1->Identifier();
	
	RWsSession ws3;
	User::LeaveIfError(ws3.Connect());
	CleanupClosePushL(ws3);
	RWindowGroup chain(ws3);
	
	RDebug::Print(KPlatsecBegin);
	retVal = chain.ConstructChildApp(id1,2);
	TEST(retVal==KErrPermissionDenied);
	if (retVal!=KErrPermissionDenied)
			INFO_PRINTF3(_L("chain.ConstructChildApp(id1,2) return value - Expected: %d, Actual: %d"), KErrPermissionDenied, retVal);

	RDebug::Print(KPlatsecEnd);
	chain1->AllowProcessToCreateChildWindowGroups(TUid::Uid(0x10205152));	//Secure ID
	retVal = chain.ConstructChildApp(id1,2);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
			INFO_PRINTF3(_L("chain.ConstructChildApp(id1,2) return value - Expected: %d, Actual: %d"), KErrNone, retVal);

	chain.Close();
	CleanupStack::PopAndDestroy(3,&ws2);
	}

void CTGwHandle::UnchainWindowGroupsL()
	{
	RArray<RWsSession::TWindowGroupChainInfo>* windowHandles=new(ELeave) RArray<RWsSession::TWindowGroupChainInfo>;
	CleanupStack::PushL(windowHandles);
	CleanupClosePushL(*windowHandles);
	GetGwListL(0,windowHandles);
	TInt shellWindow=windowHandles->Count();
	//Creating chained window groups.
	CreateChainedGroupWindowsLC(TheClient->iWs);
	GetGwListL(0,windowHandles);
	//Test total window groups in the list
	TInt retVal = windowHandles->Count();
	TEST(retVal==ENumChained+shellWindow);
	if (retVal!=ENumChained+shellWindow)
			INFO_PRINTF3(_L("windowHandles->Count() return value - Expected: %d, Actual: %d"), ENumChained+shellWindow, retVal);
	//Two unchained group windows

	//Test Parent Id
	TInt parentId=0;
	for(TInt ii=ENumChained-1;ii>=0;ii--)
		{
		RWsSession::TWindowGroupChainInfo window;
		window.iId=iChained[ii]->Identifier();
		TInt loc=windowHandles->Find(window);
		TEST(loc!=KErrNotFound);
		if (loc==KErrNotFound)
			INFO_PRINTF3(_L("windowHandles->Find(window) return value - Expected: %d, Actual: %d"), KErrNotFound, loc);

		if(loc!=KErrNotFound)
			{
			TEST((*windowHandles)[loc].iParentId==parentId);
			if ((*windowHandles)[loc].iParentId!=parentId)
				INFO_PRINTF3(_L("(*windowHandles)[loc].iParentId==parentId - Expected: %d, Actual: %d"), parentId, (*windowHandles)[loc].iParentId);

			parentId=window.iId;
			}
		}
	//Test unchaining of chained window groups
	for(TInt count=ENumChained-1;count>=0;count--)
		{
		iChained[count]->Close();
		GetGwListL(0,windowHandles);
		TEST(windowHandles->Count()== count+shellWindow);
		if (windowHandles->Count()!= count+shellWindow)
				INFO_PRINTF3(_L("windowHandles->Count()==count+shellWindow - Expected: %d, Actual: %d"), count+shellWindow, windowHandles->Count());

		}
	CleanupStack::PopAndDestroy(ENumChained,iChained[ENumChained-1]);

	//delete middle windowgroup of chain which should kill all the child windows
	CreateChainedGroupWindowsLC(TheClient->iWs);
	TInt windowPos=2;
	CleanupStack::Pop(2,iChained[1]);	//Pop first 2 items iChained[0] and iChained[1]
	iChained[windowPos]->Close();//Close the 3rd WindowGroup in the chain.should unchain 4 and 5 window group.
	CleanupStack::PopAndDestroy(iChained[windowPos]);
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[1]));
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[0]));
	GetGwListL(0,windowHandles);
	retVal = ChainedWindowCount(windowHandles);
	TEST(retVal==ENumChained-windowPos-1);
	if (retVal!=ENumChained-windowPos-1)
				INFO_PRINTF3(_L("ChainedWindowCount(windowHandles) return value - Expected: %d, Actual: %d"), ENumChained-windowPos-1, retVal);

//	CleanupStack::PopAndDestroy(ENumChained-1,iChained[ENumChained-1]);

	windowPos=3;
	CleanupStack::Pop(2,iChained[1]);
	iChained[windowPos]->Close();//Close the 2nd WindowGroup in the chain.
								//should unchain all .
	CleanupStack::PopAndDestroy(iChained[windowPos]);
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[1]));
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[0]));
	GetGwListL(0,windowHandles);
	retVal = ChainedWindowCount(windowHandles);
	TEST(retVal==ENumChained-windowPos-2);
	if (retVal!=ENumChained-windowPos-2)
				INFO_PRINTF3(_L("ChainedWindowCount(windowHandles) return value - Expected: %d, Actual: %d"), ENumChained-windowPos-2, retVal);

	windowPos=4;
	CleanupStack::Pop(2,iChained[1]);
	iChained[windowPos]->Close();//Closing parent window group in chain. There should be no chain
	CleanupStack::PopAndDestroy(iChained[windowPos]);
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[1]));
	CleanupStack::PushL(TCleanupItem(&GwHandleDestroyWindow,iChained[0]));
	GetGwListL(0,windowHandles);
	retVal = ChainedWindowCount(windowHandles);
	TEST(retVal==ENumChained-windowPos-1);
	if (retVal!=ENumChained-windowPos-1)
				INFO_PRINTF3(_L("ChainedWindowCount(windowHandles) return value - Expected: %d, Actual: %d"), ENumChained-windowPos-1, retVal);

	CleanupStack::PopAndDestroy(ENumChained-3,iChained[ENumChained-4]);
	windowHandles->Close();
	CleanupStack::PopAndDestroy(windowHandles); // causes windowsHandles->Close() called
	CleanupStack::PopAndDestroy(windowHandles); // causes delete windowsHandles	called
	}

TInt CTGwHandle::ChainedWindowCount(RArray<RWsSession::TWindowGroupChainInfo>* aWindowHandles)
	{
	TInt wndGrpCount=0;
	for(TInt ii=0;ii<aWindowHandles->Count();ii++)
		{
		if((*aWindowHandles)[ii].iParentId!=-1)
			wndGrpCount++;
		}
	return wndGrpCount;
	}

void CTGwHandle::TestOrdinalPositionL()
	{
	TInt noOfGroupWnds = NumberOfWndGroupsWithZeroPriority(iTest->iScreenNumber);
	//Creating the chained window groups
	CreateChainedGroupWindowsLC(TheClient->iWs);
	TestOrdinalPos(0);
	//Test the ordinal position of all chained windows by setting the WndPos window
	//to chainPos position
	for(TInt wndPos=0;wndPos<ENumChained;wndPos++)
		{
		for(TInt chainPos=0;chainPos<15;chainPos++)
			{
			iChained[wndPos]->SetOrdinalPosition(chainPos);
			TInt dung = chainPos-wndPos;
			if(dung > noOfGroupWnds)
				{
				dung = noOfGroupWnds;
				}
			dung = Max(dung, 0);
			TestOrdinalPos(chainPos - wndPos > noOfGroupWnds ? noOfGroupWnds : Max(chainPos - wndPos, 0));
			}
		}
	//Test for negative ordinal number
	iChained[0]->SetOrdinalPosition(-2);
	TestOrdinalPos(noOfGroupWnds);
	CleanupStack::PopAndDestroy(ENumChained,iChained[ENumChained-1]);
	}

void CTGwHandle::TestOrdinalPositionNoDebugL()
	{
	TInt noOfGroupWnds = NumberOfWndGroupsWithZeroPriority(iTest->iScreenNumber);
	//Creating the chained window groups
	CreateChainedGroupWindowsLC(TheClient->iWs);
	//Test the ordinal position of all chained windows
	TestOrdinalPosNoDebug(0);
	//Test the ordinal position of all chained windows by setting the WndPos window
	//to chainPos position
	for(TInt wndPos=0;wndPos<ENumChained;wndPos++)
		{
		for(TInt chainPos=0;chainPos<15;chainPos++)
			{
			iChained[wndPos]->SetOrdinalPosition(chainPos);
			TestOrdinalPosNoDebug(chainPos - wndPos > noOfGroupWnds ? noOfGroupWnds : Max(chainPos - wndPos, 0));
			}
		}
	//Test for negative ordinal number
	iChained[0]->SetOrdinalPosition(-2);
	TestOrdinalPosNoDebug(noOfGroupWnds);
	CleanupStack::PopAndDestroy(ENumChained,iChained[ENumChained-1]);
	}

void CTGwHandle::TestOrdinalPos(TInt aPos)
	{
	for(TInt ii=0;ii<ENumChained;ii++)
		{
		TInt iCha = iChained[ii]->OrdinalPosition();
		TEST(iCha==aPos++);
		if (iCha!=aPos-1)
			{
			INFO_PRINTF3(_L("iChained[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), aPos-1, iCha);
			}
		}
	}

void CTGwHandle::TestOrdinalPosNoDebug(TInt aPos)
	{
	for(TInt ii=0;ii<ENumChained;ii++)
		{
		TInt iCha = iChained[ii]->OrdinalPosition();
		TEST(iCha==aPos++);
		if (iCha!=aPos-1)
			INFO_PRINTF3(_L("iChained[ii]->OrdinalPosition() return value - Expected: %d, Actual: %d"), aPos-1, iCha);

		}
	}

void CTGwHandle:: TestclosingclientL()
	{
	RWsSession ws1;
	User::LeaveIfError(ws1.Connect());
	CleanupClosePushL(ws1);

	// assign to the correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws1);
	CleanupStack::PushL(screen);
	User::LeaveIfError(screen->Construct(iTest->iScreenNumber));

	// Creating first Window Group
	RWindowGroup gw1(ws1);
	User::LeaveIfError(gw1.Construct(ENullWsHandle));
	ws1.SetWindowGroupOrdinalPosition(gw1.Identifier(),0);

	// Creating second Window Group
	RWindowGroup gw2(ws1);
	User::LeaveIfError(gw2.Construct(ENullWsHandle));
	gw2.SetOwningWindowGroup(gw1.Identifier());
	gw2.SetOrdinalPosition(1);
	CleanupStack::PopAndDestroy(screen);
	CleanupStack::PopAndDestroy(&ws1);
	}

void CTGwHandle::TestClearingAndSettingChildGroupsL()
	{
	RArray<RWsSession::TWindowGroupChainInfo> windowHandles;
	CleanupClosePushL(windowHandles);
	//Creating a chain of 5 window groups.
	CreateChainedGroupWindowsLC(TheClient->iWs);
	TInt ret=KErrNone;
	// Want to test clearing and setting of window groups for chain lengths of 5 down to 2
	for(TInt jj=ENumChained-1;jj>0;jj--)
		{
		// For each window group in the chain, test clearing and setting a child group
		for(TInt ii=jj;ii>0;ii--)
			{
			RWsSession::TWindowGroupChainInfo window1;
			window1.iId=iChained[ii]->Identifier();
			GetGwListL(0,&windowHandles);
			TInt loc=windowHandles.Find(window1);
			TEST(loc!=KErrNotFound);
			if(loc!=KErrNotFound)
				{
				RWsSession::TWindowGroupChainInfo window2;
				window2.iId=iChained[ii-1]->Identifier();
				loc = windowHandles.Find(window2);
				TEST(loc!=KErrNotFound);
				// First test that the parent of the next window in the chain is the previous in the chain
				if(loc!=KErrNotFound)
					{
					TEST(windowHandles[loc].iParentId==window1.iId);
					}
				// Then clear and test that the parent of the next window in the chain
				// is no longer the previous window in the chain
				ret=iChained[ii]->ClearChildGroup();
				TEST(ret==KErrNone);
				// Try to clear a child group when it's already cleared - should return KErrArgument
				ret=iChained[ii]->ClearChildGroup();
				TEST(ret==KErrArgument);
				GetGwListL(0,&windowHandles);
				loc = windowHandles.Find(window2);
				TEST(loc!=KErrNotFound);
				// Test that it has no parent (0) or is unchained (-1)
				if(loc!=KErrNotFound)
					{
					if(ii==1) // last in chain therefore unchained, should return -1
						TEST(windowHandles[loc].iParentId==-1);
					else // otherwise should return 0 signifying in a chain but with no parent
						TEST(windowHandles[loc].iParentId==0);
					}
				// Reset the former child group as a child to the current group
				ret=iChained[ii]->SetChildGroup(window2.iId);
				TEST(ret==KErrNone);
				// Test that it has now got it's parent back
				GetGwListL(0,&windowHandles);
				loc = windowHandles.Find(window2);
				TEST(loc!=KErrNotFound);
				if(loc!=KErrNotFound)
					{
					TEST(windowHandles[loc].iParentId==window1.iId);
					}
				}
			}
			// Reduce chain length by 1
			ret=iChained[jj]->ClearChildGroup();
			TEST(ret==KErrNone);
		}
	// Try to clear the child group of a window that's not in a queue - returns KErrArgument
	ret=iChained[0]->ClearChildGroup();
	TEST(ret==KErrArgument);
	// Test calling set on a non-existent window group - fictional id, larger than what exists
	ret=iChained[0]->SetChildGroup(9999);
	TEST(ret==KErrArgument);
	// Cleanup
	CleanupStack::PopAndDestroy(ENumChained,iChained[ENumChained-1]);
	CleanupStack::PopAndDestroy(&windowHandles);
	}

void CTGwHandle::TestAgainstLoopsWhenAddingChildGroupsL()
	{
	//Creating two chains of 5 window groups.
	CreateChainedGroupWindowsLC(TheClient->iWs,ETrue);
	TInt ret=KErrNone;
	
	//Try to append any of element of this chain to the lastone (itself included)
	//All these attempts should fail (otherwise a infinite loop will be set up)
	for(TInt ii=ENumChained-1;ii>=0;ii--)
		{
		TInt windowId = iChained[ii]->Identifier();
		// Try to append it to the last one
		ret=iChained[0]->SetChildGroup(windowId);
		TEST(ret==KErrArgument);
		if(ret!=KErrArgument)
			{
			_LIT(KLog,"Error: SetChildGroup with a GpWin belonging to the same queue succeeded, return value :%d");
			LOG_MESSAGE2(KLog,ret);
			}
		}
	//Try to append any elements belonging to another chain to the last one of this chain
	//All these trials should fail (otherwise a infinite loop will be set up)
	//Except for the parent of the second chain.
	for(TInt ii=0;ii<ENumChained;ii++)
		{
		TInt windowId = iChained2[ii]->Identifier();
		// Try to append it to the last one
		ret=iChained[0]->SetChildGroup(windowId);
		if(ii != ENumChained-1)
			{
			TEST(ret==KErrArgument);
			if(ret!=KErrArgument)
				{
				_LIT(KLog,"Error: SetChildGroup with a GpWin which already has a parent succeeded, return value :%d");
				LOG_MESSAGE2(KLog,ret);
				}
			iChained[0]->ClearChildGroup();
			}
		else
			{
			TEST(ret==KErrNone);
			if(ret!=KErrNone)
				{
				_LIT(KLog,"Error: SetChildGroup with a GpWin with no parent failed, return value :%d");
				LOG_MESSAGE2(KLog,ret);
				}
			}
		}
	TInt windowId = iChained2[ENumChained-1]->Identifier();
	ret=iChained[ENumChained-1]->SetChildGroup(windowId);
	TEST(ret==KErrArgument);
	if(ret!=KErrArgument)
		{
		_LIT(KLog,"Error: SetChildGroup successful on a GpWin which already has a child, return value :%d");
		LOG_MESSAGE2(KLog,ret);
		}
	//Now take a window group not belonging to any queue
	//just take the parent's chain and pop it 
	//and try to append it to itself
	ret=iChained[ENumChained-1]->ClearChildGroup();
	TEST(ret==KErrNone);
	ret=iChained[ENumChained-1]->SetChildGroup(iChained[ENumChained-1]->Identifier());
	TEST(ret==KErrArgument);
	if(ret!=KErrArgument)
		{
		_LIT(KLog,"Error: Setting GpWin as a child of itself succeeded, return value :%d");
		LOG_MESSAGE2(KLog,ret);
		}
    CleanupStack::PopAndDestroy(ENumChained+ENumChained,iChained[ENumChained-1]);
	}
void CTGwHandle::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest0,"Window group names");
	_LIT(KTest1,"Window group identifiers");
	_LIT(KTest2,"Owner to foreground");
	_LIT(KTest3,"FindWindowGroupIdentifier test");
	_LIT(KTest4,"IdentifierWrapAround test");
	_LIT(KTest5,"DefaultOwningWindow test");
	_LIT(KTest6,"DefaultOwningWindow2 test");
	_LIT(KTest7,"Window Group Chaining");
	_LIT(KTest8,"Window Group Chaining2");
	_LIT(KTest9,"Unchaining window group");
	_LIT(KTest10,"Ordinal Postion Test");
	_LIT(KTest11,"CrashTest on closing client");
	_LIT(KTest12,"Setting and Clearing Child Groups");
	_LIT(KTest13,"No Infinite Loops in Setting Child Groups");
	((CTGwHandleStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0316

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test setting window group names

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set the name of a window group to different values and check the
  					value is set correctly

  @SYMTestExpectedResults The window group name is set correctly

*/
		case 1:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0316"));
			//iState=9; break;
			iTest->LogSubTest(KTest0);
			GwNamesL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0317

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test setting window group identifiers

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set the identifiers of a window groups. manipulate the window groups and
  					check the identifiers remain correct

  @SYMTestExpectedResults The identifiers are correct for each window group after
  							manipulation

*/
		case 2:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0317"));
			iTest->LogSubTest(KTest1);
			GwIdentifierTestL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0318

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test moving different window groups to the foreground and bringing
  					back the backgorund

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create some window groups and move them in turn to the foreground before
  					restoring the background

  @SYMTestExpectedResults Window groups move to and from the foreground with error

*/
		case 3:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0318"));
			iTest->LogSubTest(KTest2);
			OwnerToForegroundL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0319

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test returning different window groups identifiers by searching
  					on the name

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create some window groups and return their identifiers by searching on
  					their names

  @SYMTestExpectedResults The correct identifiers are returned for each name

*/
		case 4:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0319"));
			iTest->LogSubTest(KTest3);
			FindWindowGroupIdentifierTestL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0320

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that a windows group identifier is not used twice

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Assign a window group identifier to one window group and check
  					the same identifier can not be used for a second group

  @SYMTestExpectedResults Window group identifier can not be reused

*/
		case 5:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0320"));
			iTest->LogSubTest(KTest4);
			IdentifierWrapAroundTestL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0321

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that the default owning window for window groups is set
  					correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set different default owning windows for different window groups.
  					Check the windows have been set correctly.

  @SYMTestExpectedResults The default owning windows for the groups are set correctly

*/
		case 6:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0321"));
			iTest->LogSubTest(KTest5);
			DefaultOwningWindowL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0322

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that the default owning window for window groups is set
  					correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Set different default owning windows for different window groups.
  					Check the windows have been set correctly.

  @SYMTestExpectedResults The default owning windows for the groups are set correctly

*/
		case 7:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0322"));
			iTest->LogSubTest(KTest6);
			DefaultOwningWindow2L();
			TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0323

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that window groups can be chained correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create a chain of window groups and check that the chain of groups
  					can be manipluated correctly

  @SYMTestExpectedResults The chain can be manipulated correctly

*/
		case 8:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0323"));
			iTest->LogSubTest(KTest7);
			WindowGroupChaining();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0324

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that window groups can be chained correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create a chain of window groups and check that the chain of groups
  					can be manipluated correctly

  @SYMTestExpectedResults The chain can be manipulated correctly

*/
		case 9:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0324"));
			iTest->LogSubTest(KTest8);
			WindowGroupChaining2();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0325

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that window groups can be unchained correctly

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create chains of window groups and check that the window groups can be
  					unchained correctly

  @SYMTestExpectedResults The chain can be unchained correctly

*/
		case 10:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0325"));
			iTest->LogSubTest(KTest9);
			UnchainWindowGroupsL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0326

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test the ordinal position of chained window groups

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create chains of window groups and check their ordinal position is correct
  					for different scenarios

  @SYMTestExpectedResults Ordinal position of chained window group is correct

*/
		case 11:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0326"));
			iTest->LogSubTest(KTest10);
			TestOrdinalPositionL();
			break;
		case 12:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0326"));
			iTest->LogSubTest(KTest10);
			TestOrdinalPositionNoDebugL();
			break;
/**

  @SYMTestCaseID GRAPHICS-WSERV-0327

  @SYMDEF             DEF081259

  @SYMTestCaseDesc Test that window groups are dealt with correctly if their client is closed

  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Create window groups and then close there client.

  @SYMTestExpectedResults The window groups of the closed client are handled without error

*/
		case 13:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0327"));
			iTest->LogSubTest(KTest11);
			TestclosingclientL();
			break;
		case 14:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0327"));
			iTest->LogSubTest(KTest12);
			TestClearingAndSettingChildGroupsL();
			break;
		case 15:
			((CTGwHandleStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0327"));
			iTest->LogSubTest(KTest13);
			TestAgainstLoopsWhenAddingChildGroupsL();
			break;
		default:
			((CTGwHandleStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTGwHandleStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTGwHandleStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(GwHandle)
