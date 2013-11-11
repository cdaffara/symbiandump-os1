// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "twdp.h"
#include "utils.h"

namespace t_fbservwdptest
	{
	_LIT(KT_WDPTestCaseNumber,"CaseNumber");
	_LIT(KT_TestPanicTxt, "t_fbservwdptest");
	}

using namespace t_fbservwdptest;

CTWDP::CTWDP(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL)
	{
	}

CTWDP::~CTWDP()
	{
	iSharedHeapChunk.Close();
	iBitmapChunk.Close();
	RFbsSession::Disconnect();
	}

void CTWDP::ConstructL()
	{
	User::LeaveIfError(RFbsSession::Connect());
	iFbs = RFbsSession::GetSession();
	}

void CTWDP::RunTestCaseL(TInt aCurTestCase)
	{
	TBool res = iStep->GetIntFromConfig(iStep->ConfigSection(), KT_WDPTestCaseNumber, aCurTestCase);	
	if(!res)
		{
		User::Leave(KErrNotFound);
		}
	
	switch(aCurTestCase)
		{
	case 1:
		TestFbservPaged();
		break;
	case 2:
		TestFbservUnPaged();
		break;
	case 3:
		TestBitmapDataPagedL();
		break;
	case 4:
		TestBitmapDataAndSharedHeapPagedL();
		break;
	default:
		User::Panic(KT_TestPanicTxt, KErrNotFound);  //Unexpected value!
		break;
		}
	
	TestComplete();	
	}

void CTWDP::TestFbservPaged()
	{
	TBool ret=DefaultDataPaged();
	if (ret)
		{
		INFO_PRINTF1(_L("fbserv.DefaultDataPaged() returned --> 1, Expected --> 1"));
		}
	else
		{
		INFO_PRINTF1(_L("fbserv.DefaultDataPaged() returned --> 0, Expected --> 1"));		
		}			
	TEST(ret);
	}

void CTWDP::TestFbservUnPaged()
	{
	TBool ret=DefaultDataPaged();
	if (!ret)
		{
		INFO_PRINTF1(_L("fbserv.DefaultDataPaged() returned --> 0, Expected --> 0"));
		}
	else
		{
		INFO_PRINTF1(_L("fbserv.DefaultDataPaged() returned --> 1, Expected --> 0"));
		}					
	TEST(!ret);	
	}

void CTWDP::TestBitmapDataPagedL()
	{
	TEST(BitmapDataPagedL());
	}

void CTWDP::TestBitmapDataAndSharedHeapPagedL()
	{
	TBool ret = BitmapDataPagedL();
	if (ret)
		{
		User::LeaveIfError(iSharedHeapChunk.OpenGlobal(KFBSERVSharedChunkName,ETrue));
		ret = iSharedHeapChunk.IsPaged();
		if (ret)
			{
			INFO_PRINTF1(_L("iSharedHeapChunk.IsPaged() returned --> 1, Expected --> 1"));
			}
		else
			{
			INFO_PRINTF1(_L("iSharedHeapChunk.IsPaged() returned --> 0, Expected --> 1"));		
			}
		}	
	TEST(ret);	
	}

TBool CTWDP::DefaultDataPaged()
	{
	RProcess proc = RProcess();
	_LIT(KPattern, "*fbserv*");
	TFindProcess findProc(KPattern);
	TFullName fullName;
	TInt err=KErrNone;
		
	INFO_PRINTF2(_L("TFindProcess: Find a process whose name match the pattern %S "), &KPattern());
	if(findProc.Next(fullName)!= KErrNone)
		{
		INFO_PRINTF1(_L("Error: Process whose name match the above pattern not found"));
		User::LeaveIfError(err);
		}
	else
		{
		INFO_PRINTF2(_L("TFindProcess.Next() found process --> %S "), &fullName );
		}
	
	if(findProc.Next(fullName) != KErrNotFound)
		{
		INFO_PRINTF2(_L("Error: TFindProcess found more than 1 process matching the pattern --> %S "), &fullName );
		User::Leave(KErrGeneral);
		}

	if( (err=proc.Open(fullName)) != KErrNone)
		{
		INFO_PRINTF2(_L("Error: RProcess.Open() returned --> %d"), err);
		User::LeaveIfError(err);
		}
	
	TBool ret = proc.DefaultDataPaged();
	proc.Close();
	return ret;	
	 }

TBool CTWDP::BitmapDataPagedL()
	{
	User::LeaveIfError(iBitmapChunk.OpenGlobal(KFBSERVLargeChunkName,ETrue));
	TBool ret=iBitmapChunk.IsPaged();
	if (ret)
		{
		INFO_PRINTF1(_L("iBitmapChunk.IsPaged() returned --> 1, Expected --> 1"));
		}
	else
		{
		INFO_PRINTF1(_L("iBitmapChunk.IsPaged() returned --> 0, Expected --> 1"));		
		}	
	return ret;		
	}

//--------------
__CONSTRUCT_STEP__(WDP)
