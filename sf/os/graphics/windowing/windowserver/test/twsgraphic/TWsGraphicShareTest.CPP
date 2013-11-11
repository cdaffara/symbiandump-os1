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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <w32stdgraphic.h>

// bitmap to load for comparison
#define MY_TEST_BITMAP _L("Z:\\WSTEST\\MYTEST.MBM")

const TUint32 ENullWsHandle=0xFFFFFFFF;
const TInt KErrTestExeFailure = -666;

// class to check if a shared CWsGraphic can be drawn correctly
class CWsGraphicShareBase : public CBase
	{
public:
	CWsGraphicShareBase();
	~CWsGraphicShareBase();
	void ConstructL();
	void DoTestDrawGraphicCompareL(TPtrC aShare);
private :
	void Test(TInt aCondition);
	
	
private :
	TInt iScreenNumber;
	CWindowGc *iGc;
	RWsSession iWs;
	RWindowGroup *iGroupWin;
	CWsScreenDevice *iScreen;
	RWindow *iWin;	
	};

CWsGraphicShareBase::CWsGraphicShareBase() 
	{
		iScreenNumber = 0;
	}
	
CWsGraphicShareBase::~CWsGraphicShareBase() 
	{
		iWin->Close();
		delete iWin;
		delete iScreen;
		delete iGc;
		delete iGroupWin;
		iWs.Close();
	}
	
void CWsGraphicShareBase::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScreen=new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreen->Construct(iScreenNumber));
	iGc=new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	iGroupWin=new(ELeave) RWindowGroup(iWs);
	iGroupWin->Construct(1);
		
	iWin=new(ELeave) RWindow(iWs);
	iWin->Construct(*iGroupWin,ENullWsHandle);
	iWin->SetRequiredDisplayMode(EColor256);
	iWin->SetExtent(TPoint(0,0),iScreen->SizeInPixels());
	iWin->Activate();
	iWin->BeginRedraw();
	iWin->EndRedraw();
	iWs.Flush();
	}	

// Checks that the shared graphic is drawn or not. This is done by creating a new graphic in this process
// which looks the same as the shared graphic. The new graphic is then drawn to the screen followed by an 
// attempt to draw the shared graphic. The two graphics are then compared. In cases where the shared graphic
// should be drawn the two graphics should compare exactly. In cases where the shared graphic should not be 
// drawn the comparison will fail.
   
void CWsGraphicShareBase::DoTestDrawGraphicCompareL(TPtrC aShare)
	{
	// UID of the shared graphic
	TUid uid1 = {0x12000021};
	TWsGraphicId twsGraphicId1(uid1);

	_LIT8(KTestData,"HelloWorld");
	
	CFbsBitmap bitmap1;
	CFbsBitmap mask1;
	
	TSize screenSize = iScreen->SizeInPixels();
	User::LeaveIfError(bitmap1.Load(MY_TEST_BITMAP,0));
	mask1.Create(bitmap1.SizeInPixels(),iScreen->DisplayMode());
	
		CWsGraphicBitmap* bTest = CWsGraphicBitmap::NewL(&bitmap1,&mask1);
	
	// divide the screen into two equal rectangles
	TRect position1(0,0,screenSize.iWidth/2,screenSize.iHeight);
	TRect position2(screenSize.iWidth/2,0,screenSize.iWidth,screenSize.iHeight);
	
	// draw the new graphic and attempt to draw the shared graphic
	iGc->Activate(*iWin);
	iWin->Invalidate();
	iWin->BeginRedraw();
	iGc->Clear(position1);
	iGc->Clear(position2);

	iGc->DrawWsGraphic(bTest->Id(),position1,KTestData);
	iGc->DrawWsGraphic(twsGraphicId1.Uid(),position2,KTestData);

	iGc->Deactivate();	
	iWin->EndRedraw();
	
	iWs.Flush();
	iWs.Finish();
	// compare the graphic in both positions
	if (aShare==_L("false"))
		Test(!iScreen->RectCompare(position1,position2));
	else
		Test(iScreen->RectCompare(position1,position2));	
		
	delete bTest;
	}

// Failures are written to WSERV.log
void CWsGraphicShareBase::Test(TInt aCondition)
	{
	if(!aCondition)
		{
		TLogMessageText buf;
		_LIT(Fail,"AUTO Failed in WsGraphics Test : DrawSharedGraphic");
		buf.Format(Fail);
		iWs.LogMessage(buf);
		iWs.Flush();
		RProcess().Terminate(KErrTestExeFailure); // terminate this process immediately. Expect TWsGraph test step (TAutoServer.exe) to capture this
		}
	}
	
void MainL()
	{
	// read the argument from the command line of whether the graphic should be shared or not
	TBuf<256> commandLine;
    User::CommandLine(commandLine);
    TLex lex(commandLine);
    TPtrC toShare = lex.NextToken();    
    
    CActiveScheduler* activeScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);
	CleanupStack::PushL(activeScheduler);

    CWsGraphicShareBase testBase;
    testBase.ConstructL();
       
    testBase.DoTestDrawGraphicCompareL(toShare);
    
	CleanupStack::PopAndDestroy(activeScheduler);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanUpStack=CTrapCleanup::New();
	if(cleanUpStack==NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL())
	delete cleanUpStack;
	
	return(KErrNone);
	}
