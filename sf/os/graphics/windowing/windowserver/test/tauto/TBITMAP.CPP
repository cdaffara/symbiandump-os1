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
// Test the window server bitmap class
// Test the window server bitmap class
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TBITMAP.H"

//

CTBitmap::CTBitmap(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTBitmap::~CTBitmap()
	{
	delete iBitmap1;
	delete iBitmap2;
	}

void CTBitmap::ConstructL()
	{
	}

void CTBitmap::BasicTestsL()
	{
	iBitmap1=new(ELeave) CWsBitmap(TheClient->iWs);
	iBitmap2=new(ELeave) CWsBitmap(TheClient->iWs);
	User::LeaveIfError(iBitmap1->Create(TSize(20,10),EGray4));
// Re-create should close previous Create
	User::LeaveIfError(iBitmap1->Create(TSize(20,10),EGray4));
	User::LeaveIfError(iBitmap2->Duplicate(iBitmap1->Handle()));
	TEST(iBitmap1->SizeInPixels()==iBitmap2->SizeInPixels());
		
	TInt err=iBitmap2->Load(_L("NOTEXIST.XXX"),0);
	TEST(err==KErrNotFound || err==KErrPathNotFound);
	if (err!=KErrNotFound && err!=KErrPathNotFound)
		INFO_PRINTF4(_L("Bitmap2->Load(_L(NOTEXIST.XXX),0) return value - Expected: %d or %d , Actual: %d"), KErrNotFound, KErrPathNotFound, err);		
	
	TInt retVal = iBitmap2->Handle();
	TEST(retVal==0);
	if (retVal!=0)
		INFO_PRINTF3(_L("iBitmap2->Handle() return value - Expected: %d , Actual: %d"), 0, retVal);		

	RFs fs;
	User::LeaveIfError(fs.Connect());
	fs.SetNotifyUser(EFalse);
	TInt ret=fs.MkDir(SAVE_BITMAP_NAME);
	if (ret!=KErrNone && ret!=KErrAlreadyExists && ret!=KErrPathNotFound)
		User::Leave(ret);
	fs.Close();
//
	retVal = iBitmap1->Save(SAVE_BITMAP_NAME);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iBitmap1->Save(SAVE_BITMAP_NAME) return value - Expected: %d , Actual: %d"), KErrNone, retVal);		
	
	retVal = iBitmap2->Load(SAVE_BITMAP_NAME,0);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("iBitmap2->Load(SAVE_BITMAP_NAME,0) return value - Expected: %d , Actual: %d"), KErrNone, retVal);		
	
	TEST(iBitmap1->SizeInPixels()==iBitmap2->SizeInPixels());
	}
	
	
void CTBitmap::TestFixForPDEF098889L()
	{
	TRequestStatus status;
	TThreadParams params;
	params.iScreenNr = TheClient->iWs.GetFocusScreen();

	RThread thread1;
	params.iFunction = Function1PDEF098889L;
	TEST(thread1.Create(_L("PDEF098889_1"), ThreadFunction, 0x1000, NULL, &params)==KErrNone);
	thread1.Logon(status);
	thread1.Resume();
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(thread1.ExitType()==EExitKill);
	thread1.Close();

	RThread thread2;
	params.iFunction = Function2PDEF098889L;
	TEST(thread2.Create(_L("PDEF098889_2"), ThreadFunction, 0x1000, NULL, &params)==KErrNone);
	thread2.Logon(status);
	thread2.Resume();
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(thread2.ExitType()==EExitKill);
	thread2.Close();
	}

TInt CTBitmap::ThreadFunction(TAny* aParams)
	{
	CTrapCleanup *trap = CTrapCleanup::New();
	__ASSERT_ALWAYS(trap, User::Invariant());
	RWsSession session;
	TInt err = session.Connect();
	if (err == KErrNone)
		{
		CWsScreenDevice *device = new CWsScreenDevice(session);
		if (device)
			{
			err = device->Construct(static_cast<TThreadParams*>(aParams)->iScreenNr);
			if (err == KErrNone)
				{
				TRAP(err, static_cast<TThreadParams*>(aParams)->iFunction(session, *device));	
				}
			delete device;
			}
		else
			err = KErrNoMemory;
		session.Close();
		}
	delete trap;
	return err;
	}

void CTBitmap::Function1PDEF098889L(RWsSession& aSession, CWsScreenDevice& aDevice)
	{
	CWindowGc *gc;
	User::LeaveIfError(aDevice.CreateContext(gc));
	CleanupStack::PushL(gc);
	RWindowGroup group(aSession);
	User::LeaveIfError(group.Construct(1, EFalse));
	CWsBitmap *bitmap = new(ELeave) CWsBitmap(aSession);
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(32, 32), EGray256));
	RWindow window(aSession);
	User::LeaveIfError(window.Construct(group, 2));
	window.SetExtent(TPoint(0,0), TSize(32, 32));
	User::LeaveIfError(window.SetRequiredDisplayMode(EColor64K));
	window.Activate();
	window.BeginRedraw();
	gc->Activate(window);
	gc->BitBlt(TPoint(0, 0), bitmap);
	gc->Deactivate();
	window.EndRedraw();
	CleanupStack::PopAndDestroy(bitmap);
	aSession.Flush();
	CleanupStack::PopAndDestroy(gc);
	}

void CTBitmap::Function2PDEF098889L(RWsSession& aSession, CWsScreenDevice& /*aDevice*/) 
	{
	RWindowGroup group(aSession);
	User::LeaveIfError(group.Construct(1, EFalse));
	CWsBitmap *bitmap = new(ELeave) CWsBitmap(aSession);
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(32, 32), EGray256));
	RWindow window(aSession);
	User::LeaveIfError(window.Construct(group, 2));
	window.SetExtent(TPoint(0,0), TSize(32, 32));
	User::LeaveIfError(window.SetRequiredDisplayMode(EColor64K));
	User::LeaveIfError(window.SetTransparencyWsBitmap(*bitmap));
	window.Activate();
	CleanupStack::PopAndDestroy(bitmap);
	aSession.Flush();
	}

void CTBitmap::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0201

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test basic bitmap functionality

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Test that bitmaps can be loaded, copied and saved
					correctly

@SYMTestExpectedResults The bitmaps are manipulated withotu error
*/	 
		case 1:
			((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0201"));
			iTest->LogSubTest(_L("Bitmap1"));
			BasicTestsL();
			break;
		case 2:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0532
*/
			((CTBitmapStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0532"));
			if (TransparencySupportedL()==KErrNotSupported)
				return;
			iTest->LogSubTest(_L("Test fix for PDEF098889"));
			TestFixForPDEF098889L();
			break; 
		default:
			((CTBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTBitmapStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTBitmapStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(Bitmap)
