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
// Manual window server tests
// Note: Wherever possible tests should be put into the TAUTO automatic test
// code test should only go here when manual operation is essential
// 
//

#include <e32std.h>
#include "W32STD.H"
#include <e32svr.h>
#include "TMAN.H"

#define LOG_TESTS

class ErrorDialog : public CTDialog
	{
public:
	ErrorDialog(CTestBase *aTest);
	void ButtonPressL(TInt aButton);
private:
	CTestBase *iTest;
	};

class TestWindowGroup : public CTWindowGroup
	{
public:
	TestWindowGroup(CTClient *aClient);
	void ConstructL();
	void KeyL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyDownL(const TKeyEvent &aKey,const TTime &aTime);
	void KeyUpL(const TKeyEvent &aKey,const TTime &aTime);
	};

class CTManScheduler : public CActiveScheduler
	{
public:
	void SetClient(TestClient *aClient);
	void Error(TInt anError) const;
private:
	void doErrorL(TInt anError) const;
private:
	TestClient *iClient;
	};

typedef CTestBase *(*CTestBaseCreate)();

GLREF_C CTestBase *CreateScreenModeTest();
GLREF_C CTestBase *CreateScaleTest();
GLREF_C CTestBase *CreateTextTest();
GLREF_C CTestBase *CreateDummyTest();
GLREF_C CTestBase *CreateMultiConTest();
GLREF_C CTestBase *CreateMultiCaptureKeyTest();
GLREF_C CTestBase *CreateHotKeyTest();
GLREF_C CTestBase *CreatePointerTest();
GLREF_C CTestBase *CreatePointerCaptureTest();
GLREF_C CTestBase *CreatePointerBufferTest();
GLREF_C CTestBase *CreateModifiersChangedTest();
GLREF_C CTestBase *CreatePointerKeyTest();
GLREF_C CTestBase *CreatePasswordTest();
GLREF_C CTestBase *CreatePointerCursorTest();
GLREF_C CTestBase *CreateScreenModeTimes();

// Tests that have not been converted to tauto yet
CTestBaseCreate CreateTestClass[]={
	CreateScreenModeTimes,
	CreatePointerCursorTest,
	CreatePointerTest,			//Mostly converted
	CreatePointerKeyTest,		//Mostly converted
	CreateModifiersChangedTest,		//Partially Conerted
	CreatePointerBufferTest,
	CreateTextTest,
	CreateScaleTest,
	CreateDummyTest,
	};

LogWindow *LogWin;

void TManPanic(TInt aPanic)
	{
	User::Panic(_L("Auto"),aPanic);
	}

//
// Log window, logs testing //
//

LogWindow::LogWindow() : CTWin()
	{
	}

void LogWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	iTitleHeight=iFont->HeightInPixels()+4;
	}

void LogWindow::Draw()
	{
	iGc->SetPenColor(TRgb::Gray16(8));
	iGc->SetPenColor(TRgb::Gray16(0));
	DrawBorder();
	iGc->DrawLine(TPoint(0,iTitleHeight),TPoint(iSize.iWidth,iTitleHeight));
	iGc->DrawText(iTestTitle, TPoint((iSize.iWidth-iFont->TextWidthInPixels(iTestTitle))/2,iFont->AscentInPixels()+2));
	}

void LogWindow::LogTest(TDesC &aTitle,TInt aNum)
	{
	iTestTitle.Format(TRefByValue<const TDesC>(_L("Test %d,%S")),aNum,&aTitle);
	iWin.Invalidate();
	Client()->iWs.Flush();
	}

//
// Test window, simple window used to do test graphics in //
//
TestWindow::TestWindow() : CTWin()
	{
	}

void TestWindow::SetUpL(TPoint pos,TSize size,CTWinBase *parent, CWindowGc &aGc)
	{
	iBorderWin=new(ELeave) BorderWindow();
	iBorderWin->SetUpL(pos,size,parent,aGc);
	CTWin::ConstructExtLD(*iBorderWin,TPoint(2,2),TSize(size.iWidth-4,size.iHeight-4));
	Activate();
	AssignGC(aGc);
	}

void TestWindow::Draw()
	{
	iGc->Clear();
	}

//
BorderWindow::BorderWindow() : CTWin()
	{
	}

void BorderWindow::ConstructL(CTWinBase &parent)
	{
	CTWin::ConstructL(parent);
	}

void BorderWindow::Draw()
	{
	iGc->SetBrushColor(TRgb::Gray16(0));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->DrawRect(TRect(Size()));
	}

//

TestWindowGroup::TestWindowGroup(CTClient *aClient) : CTWindowGroup(aClient)
	{
	}

void TestWindowGroup::ConstructL()
	{
	CTWindowGroup::ConstructL();
	GroupWin()->EnableScreenChangeEvents();
	}

void TestWindowGroup::KeyDownL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (iCurWin)
		iCurWin->KeyDownL(aKey,aTime);
	}

void TestWindowGroup::KeyUpL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (iCurWin)
		iCurWin->KeyUpL(aKey,aTime);
	}

void TestWindowGroup::KeyL(const TKeyEvent &aKey,const TTime &aTime)
	{
	if (aKey.iModifiers&EModifierFunc)
		{
		switch(aKey.iCode)
			{
			case 'f':
				((TestClient *)Client())->Driver()->iTest->TriggerFail();
				break;
			}
		}
	else if (iCurWin)
		iCurWin->WinKeyL(aKey,aTime);
	}

//

TestClient::TestClient()
	{
	}

void TestClient::ConstructL()
	{
	CTClient::ConstructL();

	iGroup=new(ELeave) TestWindowGroup(this);
	iGroup->ConstructL();

	TSize screenSize=iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	LogWin=new(ELeave) LogWindow();
	LogWin->SetUpL(TPoint(5+winWidth*2,5),TSize(winWidth,winHeight),iGroup,*iGc);
	iDriver=new(ELeave) TestDriver(this);
	}

TestClient::~TestClient()
	{
	delete iDriver;
	}

TestDriver *TestClient::Driver()
	{
	return(iDriver);
	}

//
// Error dialog //
//

ErrorDialog::ErrorDialog(CTestBase *aTest) : CTDialog(), iTest(aTest)
	{}

void ErrorDialog::ButtonPressL(TInt aButton)
	{
	switch(aButton)
		{
		case 0:
			{
			CTestBase *test=iTest;
			CTDialog::Delete(this);
			if (test)
				test->Driver()->DestroyTest();
			CActiveScheduler::Stop();
			User::Leave(0);		// Signals RunL not to do another request
			}
			break;
		case 1:
			CTestBase *test=iTest;
			CTDialog::Delete(this);
			if (test)
				{
				test->Driver()->TestComplete();
				test->Driver()->DestroyTest();
				}
			break;
		}
	}

//
// TestDriver, drives the test code //
//

TestDriver::TestDriver(TestClient *aClient) : CActive(-10), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	Request();
	}

TestClient *TestDriver::Client()
	{
	return(iClient);
	}

TestDriver::~TestDriver()
	{
	Cancel();
	DestroyTest();
	}

void TestDriver::DoCancel()
	{
	TRequestStatus *pStat= &iStatus;
	RThread().RequestComplete(pStat,KErrCancel);
	}

void TestDriver::Request()
	{
	TRequestStatus *pStat= &iStatus;
	RThread().RequestComplete(pStat,KErrNone);
	SetActive();
	}

void TestDriver::RunL()
	{
	iTest=(*CreateTestClass[iTestNum])();
	iTest->StartTest(iTestNum,this);
	iTest->ConstructL();
	}

void TestDriver::TestComplete()
	{
	iTestNum++;
	if (iTestNum==sizeof(CreateTestClass)/sizeof(CreateTestClass[0]))
		{
		DisplayDialog(_L("Tests complete"),_L(""),_L(""));
		CActiveScheduler::Stop();
		}
	else
		Request();
	}

void TestDriver::DestroyTest()
	{
	delete iTest;
	iTest=NULL;
	}

// CTestBase //

CTestBase::CTestBase(const TDesC &aTitle) : CActive(-10)
	{
	CActiveScheduler::Add(this);
	iTitle.Copy(aTitle);
	}

CTestBase::~CTestBase()
	{
	User::Free(iBuf1);
	User::Free(iBuf2);
	Cancel();
	}

void CTestBase::DoCancel()
	{
	TRequestStatus *pStat= &iStatus;
	RThread().RequestComplete(pStat,KErrCancel);
	}

void CTestBase::Request()
	{
	TRequestStatus *pStat= &iStatus;
	RThread().RequestComplete(pStat,KErrNone);
	SetActive();
	}

void CTestBase::TriggerFail()
	{
	iFail=ETrue;
	}

TestClient *CTestBase::Client()
	{
	return(iDriver->Client());
	}

void CTestBase::LogLeave(TInt aErr)
	{
#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(KLeave,"MAN Left with error code %d in sub-test %d: ");
	buf.AppendFormat(KLeave,aErr,iSubTestNum);
	buf.Append(iSubTitle);
	Client()->LogMessage(buf);
#else
	aErr=KErrNone;		//To stop a warning
#endif
	}

void CTestBase::RunL()
	{
	TInt ret=EInvalid;
	TRAPD(err,ret=DoTestL());
	if (err!=KErrNone)
		{
		LogLeave(err);
		if (err!=ETestFailed)
			{
			User::Leave(err);
			}
		}
	else if (ret==EFinished)
		{
		iDriver->TestComplete();
		iDriver->DestroyTest();
		}
	else if (ret==ENext)
		Request();
	}

void CTestBase::StartTest(TInt aNum, TestDriver *aDriver)
	{
	iDriver=aDriver;
	iTestNum=aNum;
	LogWin->LogTest(iTitle,aNum);
#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(ManNewTest,"MAN New Test %d: ");
	buf.AppendFormat(ManNewTest,aNum);
	buf.Append(iTitle);
	Client()->LogMessage(buf);
#endif
	Request();
	}

void CTestBase::LogSubTest(const TDesC &aSubTitle,TInt aNum)
	{
	iSubTestNum=aNum;
	iSubTitle=aSubTitle;
#if defined(LOG_TESTS)
	TLogMessageText buf;
	_LIT(ManSubTest,"MAN SubTest %d: ");
	buf.AppendFormat(ManSubTest,iSubTestNum);
	buf.Append(iSubTitle);
	Client()->LogMessage(buf);
#endif
	}

void CTestBase::AbortL()
	{
	TestDriver *driver=iDriver;			
	iDriver->DestroyTest();
	driver->TestComplete();
	User::Leave(ETestFailed);
	}

void CTestBase::TestL(TInt aCondition)
	{
	if (!aCondition || iFail)
		{
		iFail=EFalse;
  		ErrorDialog *dialog=new ErrorDialog(this);
		if (dialog)
			{
			dialog->SetTitle(_L("Test failed"));
			dialog->SetNumButtons(2);
			dialog->SetButtonText(0,_L("Abort all tests"));
			dialog->SetButtonText(1,_L("Continue other tests"));
			dialog->ConstructLD(*Client()->iGroup,*Client()->iGc);
			dialog->Display();
			}
		User::Leave(ETestFailed);
		}
	}

void CTManScheduler::SetClient(TestClient *aClient)
	{
	iClient=aClient;
	}

void CTManScheduler::Error(TInt aError) const
	{
	TRAP_IGNORE(doErrorL(aError));
	CActiveScheduler::Stop();
	}

void CTManScheduler::doErrorL(TInt aError) const
	{
	CTDialog *dialog=new(ELeave) CTDialog();
	if (dialog)
		{
		TWindowTitle title;
		title.Format(TRefByValue<const TDesC>(_L("Error %d")),aError);
		dialog->SetTitle(title);
		dialog->SetNumButtons(1);
		dialog->SetButtonText(0,_L("Abort tests"));
		dialog->ConstructLD(*iClient->iGroup,*iClient->iGc);
		dialog->Display();
		}
	}

//

GLDEF_C CTClient *CreateClientL()
	{
	return(new(ELeave) TestClient());
	}


GLDEF_C TInt E32Main()
{
return(TestLibStartUp(CreateClientL));
}
