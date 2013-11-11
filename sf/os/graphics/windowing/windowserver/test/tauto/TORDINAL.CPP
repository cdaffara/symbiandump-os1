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
// ORDINAL.CPP
// Test ordinal position and priority changes on windows
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TORDINAL.H"

_LIT(KLogNextSibling,"NextSibling of iChild%d is not iChild%d");
_LIT(KLogNoNextSibling,"iChild%d has a NextSibling when it shouldn't");
_LIT(KLogPrevSibling,"PrevSibling of iChild%d is not iChild%d");
_LIT(KLogNoPrevSibling,"iChild%d has a PrevSibling when it shouldn't");

CTOrdinal::CTOrdinal(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

COrdinalWindowBase::COrdinalWindowBase(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep) : iClient(aClient), iTest(aTest), iTestStep(aTestStep)
	{
	}

COrdinalWindow::COrdinalWindow(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep) : COrdinalWindowBase(aClient, aTest, aTestStep), iClientWin(aClient->iWs)
	{
	__DECLARE_NAME(_S("COrdinalWindow"));
	}

COrdinalWindowGroup::COrdinalWindowGroup(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep) : COrdinalWindowBase(aClient, aTest, aTestStep), iGroupWin(aClient->iWs)
	{
	__DECLARE_NAME(_S("COrdinalWindowGroup"));
	}

COrdinalWindowBase::~COrdinalWindowBase()
	{
	if (iWin)
		iWin->Close();
	}

void COrdinalWindowBase::Draw()
	{}

inline RWindowTreeNode* COrdinalWindowBase::WinTreeNode()
	{
	return(iWin);
	}

inline TUint32 COrdinalWindowBase::Handle()
	{
	return reinterpret_cast<TUint>(this);
	}

COrdinalWindowBase *COrdinalWindowGroup::NewL(CTClient *aClient, CTestBase* aTest, CTestStep* aTestStep)
	{
	COrdinalWindowGroup *oWin=new(ELeave) COrdinalWindowGroup(aClient, aTest, aTestStep);
	TInt err=oWin->iGroupWin.Construct((TUint32)oWin);
	if (err<0)
		{
		delete oWin;
		User::Leave(err);
		}
	oWin->iWin= &oWin->iGroupWin;
	return(oWin);
	}

COrdinalWindowBase *COrdinalWindow::NewL(CTClient* aClient, RWindowTreeNode* aParent, CTestBase* aTest, CTestStep* aTestStep)
	{
	COrdinalWindow *oWin=new(ELeave) COrdinalWindow(aClient, aTest, aTestStep);
	TInt err=oWin->iClientWin.Construct(*aParent,(TUint32)oWin);
	if (err!=KErrNone)
		{
		delete oWin;
		User::Leave(err);
		}
	oWin->iClientWin.Activate();
	oWin->iWin= &oWin->iClientWin;
	return(oWin);
	}

TInt COrdinalWindowBase::OrdinalPosition()
	{
	return(iWin->OrdinalPosition());
	}

void COrdinalWindowBase::SetOrdinalPosition(TInt aPos)
	{
	iWin->SetOrdinalPosition(aPos);
	}

void COrdinalWindowBase::SetOrdinalPosition(TInt aPos,TInt aPri)
	{
	iWin->SetOrdinalPosition(aPos,aPri);
	}

CTOrdinal::~CTOrdinal()
	{
	delete iClient;
	}

void DZ(COrdinalWindowBase * &aX)
	{
	delete aX;
	aX=NULL;
	}

void CTOrdinal::DestroyWindows()
	{
	DZ(iParent);
	DZ(iParent2);
	DZ(iParent3);
	for(TInt child=0;child<ENumChildren;++child)
		DZ(iChild[child]);
	}

void COrdinalWindowBase::TestOP(TInt aTestPos)
	{
	iTestStep->TEST(iWin->OrdinalPosition()==aTestPos);
	}

void COrdinalWindowBase::SetAndTestOP(TInt aPos,TInt aTestPos)
	{
	iWin->SetOrdinalPosition(aPos);
	iTestStep->TEST(iWin->OrdinalPosition()==aTestPos);
	}

void COrdinalWindowBase::SetAndTestOP(TInt aPos)
	{
	SetAndTestOP(aPos, aPos);
	}

TInt COrdinalWindowBase::SetToLastAndGetOP()
	{
	iWin->SetOrdinalPosition(-1);
	return(iWin->OrdinalPosition());
	}

TInt COrdinalWindowBase::SetToLastAndGetOPPri(TInt aPri)
	{
	iWin->SetOrdinalPosition(-1, aPri);
	return(iWin->OrdinalPosition());
	}

void COrdinalWindowBase::SetAndTestOPPri(TInt aPos,TInt aPri,TInt aTestPos)
	{
	iWin->SetOrdinalPosition(aPos,aPri);
	iTestStep->TEST(iWin->OrdinalPosition()==aTestPos);
	iTestStep->TEST(iWin->OrdinalPriority()==aPri);
	}

void COrdinalWindowBase::SetAndTestOPPri(TInt aPos,TInt aPri)
	{
	SetAndTestOPPri(aPos,aPri,aPos);
	}

inline COrdinalWindowBase* COrdinalWindowBase::NextSibling() const
	{
	return reinterpret_cast<COrdinalWindow*>(iWin->NextSibling());
	}

inline COrdinalWindowBase* COrdinalWindowBase::PrevSibling() const
	{
	return reinterpret_cast<COrdinalWindow*>(iWin->PrevSibling());
	}

void CTOrdinal::TestWindowOrderNext(TInt aBefore,TInt aAfter)
	{
	TInt retVal=(iChild[aBefore]->NextSibling()==iChild[aAfter]);
	TEST(retVal);
	if (!retVal)
		LOG_MESSAGE3(KLogNextSibling,aBefore,aAfter);
	}

void CTOrdinal::TestWindowOrderNext(TInt aLast)
	{
	TInt retVal=(iChild[aLast]->NextSibling()==0);
	TEST(retVal);
	if (!retVal)
		LOG_MESSAGE2(KLogNoNextSibling,aLast);
	}

void CTOrdinal::TestWindowOrderPrev(TInt aAfter,TInt aBefore)
	{
	TInt retVal=(iChild[aAfter]->PrevSibling()==iChild[aBefore]);
	TEST(retVal);
	if (!retVal)
		LOG_MESSAGE3(KLogPrevSibling,aAfter,aBefore);
	}

void CTOrdinal::TestWindowOrderPrev(TInt aFirst)
	{
	TInt retVal=(iChild[aFirst]->PrevSibling()==0);
	TEST(retVal);
	if (!retVal)
		LOG_MESSAGE2(KLogNoPrevSibling,aFirst);
	}
	
void CTOrdinal::OrdinalPos()
	{
	TInt last=iChild[0]->SetToLastAndGetOP();
	iChild[0]->SetAndTestOP(0);
	iChild[5]->TestOP(5);
	iChild[1]->SetAndTestOP(3);
	iChild[0]->SetAndTestOP(0);
	iChild[0]->SetAndTestOP(-1,last);
	iChild[0]->SetAndTestOP(-1000,last);
	iChild[0]->SetAndTestOP(2);
	iChild[0]->SetAndTestOP(-1000,last);
	iChild[0]->SetAndTestOP(0);
	for(TInt index=0;index<=5;index++)
		iChild[4]->SetAndTestOP(index,index);
	iChild[0]->SetAndTestOP(-1,last);
	iChild[1]->SetAndTestOP(-1,last);
	iChild[2]->SetAndTestOP(-1,last);
	iChild[3]->SetAndTestOP(-1,last);
	iChild[4]->SetAndTestOP(-1,last);
	iChild[5]->SetAndTestOP(-1,last);
	TInt child;
	for (child=0;child<ENumChildren-1;++child)
		TestWindowOrderNext(child,child+1);
	TestWindowOrderNext(5);
	}

void CTOrdinal::OrdinalPriority()
	{
	TBool retVal;
	TInt last12=iChild[2]->SetToLastAndGetOPPri(12);
	iChild[2]->TestOP(last12);
	TInt lastKMax=iChild[2]->SetToLastAndGetOPPri(KMaxTInt32);
	iChild[2]->TestOP(lastKMax);
	TInt last=iChild[2]->SetToLastAndGetOPPri(0);
	iChild[2]->TestOP(last);
	iChild[2]->SetAndTestOPPri(-1,12,last12);				// One and only pri 12 window
	iChild[3]->SetAndTestOPPri(-1,KMaxTInt32,lastKMax);	// One and only pri KMaxTInt32 window
	iChild[1]->SetAndTestOPPri(0,KMaxTInt32);
	iChild[5]->SetAndTestOP(-1,last-3);
	iChild[1]->TestOP(0);
	iChild[0]->SetAndTestOPPri(0,50);
	iChild[1]->SetAndTestOPPri(0,50);
	iChild[2]->SetAndTestOPPri(0,50);
	iChild[3]->SetAndTestOPPri(0,50);
	iChild[1]->TestOP(2);
	if (iGroupTest)
		{
		retVal=(reinterpret_cast<CBase*>(iChild[0]->NextSibling())==iClient->iGroup);
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"iChild0 NextSibling is not the main group window");
			LOG_MESSAGE(KLog);
			}
		retVal=(reinterpret_cast<CBase*>(iClient->iGroup->NextSibling())==iChild[4]);
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"NextSibling of main group window is not iChild4");
			LOG_MESSAGE(KLog);
			}
		}
	else
		TestWindowOrderNext(0,4);
	TestWindowOrderNext(4,5);
	TestWindowOrderNext(5);
	iChild[2]->SetAndTestOPPri(5,-1,0);
	iChild[3]->SetAndTestOPPri(5,-1,1);
	TestWindowOrderNext(5,2);
	iChild[0]->SetAndTestOPPri(100000,KMinTInt32,0);
	iChild[1]->SetAndTestOPPri(200000,KMinTInt32,1);
	iChild[2]->SetAndTestOPPri(300000,KMinTInt32,2);
	iChild[5]->SetAndTestOPPri(0,0,0);
	iChild[3]->TestOP(0);
	iChild[0]->SetAndTestOPPri(0,-1);
	iChild[1]->SetAndTestOPPri(0,-1);
	iChild[2]->SetAndTestOPPri(0,-1);
	iChild[3]->SetAndTestOPPri(0,1);
	iChild[4]->SetAndTestOPPri(0,1);
	iChild[5]->SetAndTestOPPri(0,1);
	TestWindowOrderPrev(0,1);
	TestWindowOrderPrev(1,2);
	if (iGroupTest)
		{
		retVal=(reinterpret_cast<CBase*>(iChild[2]->PrevSibling())==iClient->iGroup);
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"iChild2 PrevSibling is not the main group window");
			LOG_MESSAGE(KLog);
			}
		retVal=(reinterpret_cast<CBase*>(iClient->iGroup->PrevSibling())==iChild[3]);
		TEST(retVal);
		if (!retVal)
			{
			_LIT(KLog,"PrevSibling of main group window is not iChild3");
			LOG_MESSAGE(KLog);
			}
		}
	else
		TestWindowOrderPrev(2,3);
	TestWindowOrderPrev(3,4);
	TestWindowOrderPrev(4,5);
	TestWindowOrderPrev(5);
	}

void CTOrdinal::ConstructL()
	{
	iClient=new(ELeave) COrdinalClient();
	iClient->SetScreenNumber(iTest->iScreenNumber);
	iClient->ConstructL();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0446

@SYMCR				CR1164

@SYMTestCaseDesc	Test ClientHandle function returns right value

@SYMTestPriority	High

@SYMTestStatus		Implemented

@SYMTestActions		Call this function on windows at various times

@SYMTestExpectedResults The value set when window was created is returned
*/
void CTOrdinal::CreateWindowsL(TInt aMode)
	{
	RWindowTreeNode* parent=NULL;
	RWindowTreeNode* base=iClient->iGroup->WinTreeNode();
	TInt errors=0;

	if (base->Child()!=0)
		{
		_LIT(KErrText,"Main Group Window has children at start of test");
		LOG_MESSAGE(KErrText);
		AutoPanic(EAutoPanicGroupWinHasChild);
		}
	if (base->ClientHandle()!=reinterpret_cast<TUint32&>(iClient->iGroup))
		++errors;
	if (base->Parent()!=0)
		++errors;
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"Handles of Main Group Window are not as expected");
		LOG_MESSAGE2(KErrText,errors);
		}
	iGroupTest=EFalse;
	switch(aMode)
		{
		case 3:
			iParent=COrdinalWindow::NewL(iClient,base,iTest,iStep);
			parent=iParent->WinTreeNode();
			CheckHandlesOnNewWindow(iClient->iGroup,iParent);
			break;
		case 1:
			parent=iClient->iGroup->WinTreeNode();
			break;
		case 2:
			iParent=COrdinalWindow::NewL(iClient,base,iTest,iStep);
			CheckHandlesOnNewWindow(iClient->iGroup,iParent);
			iParent2=COrdinalWindow::NewL(iClient,iParent->WinTreeNode(),iTest,iStep);
			CheckHandlesOnNewWindow(iParent,iParent2);
			iParent3=COrdinalWindow::NewL(iClient,iParent2->WinTreeNode(),iTest,iStep);
			parent=iParent3->WinTreeNode();
			CheckHandlesOnNewWindow(iParent2,iParent3);
			break;
		case 0:
			iGroupTest=ETrue;
			for (TInt child=ENumChildren-1;child>=0;--child)
				iChild[child]=COrdinalWindowGroup::NewL(iClient,iTest,iStep);
			CheckHandles(0);
			return;
		}
	for (TInt child=ENumChildren-1;child>=0;--child)
		{
		iChild[child]=COrdinalWindow::NewL(iClient,parent,iTest,iStep);
		if (iChild[child]->Handle()!=parent->Child())
			++errors;
		}
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"%d windows were not the first child");
		LOG_MESSAGE2(KErrText,errors);
		}
	CheckHandles(parent->ClientHandle());
	}

void CTOrdinal::CheckHandlesOnNewWindow(CTWindowGroup* aParent,COrdinalWindowBase* aWin)
	{
	TInt errors=0;
	if (aParent->WinTreeNode()->Child()!=aWin->Handle())
		++errors;
	if (aWin->WinTreeNode()->Parent()!=reinterpret_cast<TUint32&>(aParent))
		++errors;
	CheckHandlesOnNewWindow(errors,aWin);
	}

void CTOrdinal::CheckHandlesOnNewWindow(COrdinalWindowBase* aParent,COrdinalWindowBase* aWin)
	{
	TInt errors=0;
	if (aParent->WinTreeNode()->Child()!=aWin->Handle())
		++errors;
	if (aWin->WinTreeNode()->Parent()!=aParent->Handle())
		++errors;
	CheckHandlesOnNewWindow(errors,aWin);
	}
	
void CTOrdinal::CheckHandlesOnNewWindow(TInt aErrors,COrdinalWindowBase* aWin)
	{
	RWindowTreeNode* win=aWin->WinTreeNode();
	if (win->ClientHandle()!=aWin->Handle())
		++aErrors;
	if (win->PrevSibling()!=0)
		++aErrors;
	if (win->NextSibling()!=0)
		++aErrors;
	if (win->Child()!=0)
		++aErrors;
	TEST(aErrors==0);
	if (aErrors>0)
		{
		_LIT(KErrText,"%d errors in handles of newly created window");
		LOG_MESSAGE2(KErrText,aErrors);
		}
	}

void CTOrdinal::CheckHandles(TUint aParent)
	{
	TInt errors=0;
	TInt child;
	for (child=0;child<ENumChildren;++child)
		{
		if (iChild[child]->WinTreeNode()->ClientHandle()!=iChild[child]->Handle())
			++errors;
		}
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"%d windows gave wrong client handle");
		LOG_MESSAGE2(KErrText,errors);
		errors=0;
		}
	for (child=0;child<ENumChildren;++child)
		{
		if (iChild[child]->WinTreeNode()->Parent()!=aParent)
			++errors;
		}
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"%d children gave wrong parent handle");
		LOG_MESSAGE2(KErrText,errors);
		errors=0;
		}
	for (child=1;child<ENumChildren;++child)
		{
		if (iChild[child-1]->WinTreeNode()->NextSibling()!=iChild[child]->Handle())
			++errors;
		}
	if (iChild[5]->WinTreeNode()->NextSibling()!=0 && !iGroupTest)
		++errors;
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"%d windows gave wrong next sibling handle");
		LOG_MESSAGE2(KErrText,errors);
		errors=0;
		}
	if (iChild[0]->WinTreeNode()->PrevSibling()!=0 && !iGroupTest)
		++errors;
	for (child=1;child<ENumChildren;++child)
		{
		if (iChild[child]->WinTreeNode()->PrevSibling()!=iChild[child-1]->Handle())
			++errors;
		}
	TEST(errors==0);
	if (errors>0)
		{
		_LIT(KErrText,"%d windows gave wrong prev sibling handle");
		LOG_MESSAGE2(KErrText,errors);
		}
	}

COrdinalClient::COrdinalClient()
	{
	}

void COrdinalClient::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	// change to correct screen
	//
	iScreen = new (ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreen->Construct(iScreenNumber));
		
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0,-2000000000);
	TheClient->iWs.Flush();
	iGroup=new(ELeave) CTWindowGroup(this);
	iGroup->ConstructL();
	}

COrdinalClient::~COrdinalClient()
	{
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0,0);
	}

void COrdinalClient::KeyL(const TKeyEvent &,const TTime &)
	{
	}

void CTOrdinal::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"Ordinal 1");
	((CTOrdinalStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0217

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test ordinal position and priority changes on windows

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Set different ordinal positions and priorities on a
					number of windows and check they have been set
					correctly

@SYMTestExpectedResults The positions and priorities are set correctly
*/
		case 1:
			((CTOrdinalStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0217"));
			iTest->LogSubTest(KTest1);
			{
			for(TInt index=0;index<4;index++)
				{
				((CTOrdinalStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0446"));
				CreateWindowsL(index);
				((CTOrdinalStep*)iStep)->RecordTestResultL();
				OrdinalPos();
				OrdinalPriority();
				DestroyWindows();
				}
			}
			((CTOrdinalStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0217"));
			if (!iStep->TestStepResult() == EPass)
				TEST(EFalse);
			break;
		default:
			((CTOrdinalStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTOrdinalStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		};
	((CTOrdinalStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(Ordinal)
