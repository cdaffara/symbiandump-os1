// @file TestTelephonySharedData.cpp
//
// Copyright (c) 2004-2007 Symbian Software Ltd. All rights reserved.
//
// This contains implementation of Shared Data

// User include
#include "TestLtsySharedData.h"

// Epoc include
#include <etelmm.h>

/*@{*/
/// Literal constants
_LIT(KSimTsyName,		"SIM");
_LIT(KPhoneName,		"SimulatorPhone");
_LIT(KDummyTsyName,		"DMM");
_LIT(KDMobile,			"DMobile");
/*@}*/

CTestLtsySharedData* CTestLtsySharedData::NewL()
	{
	CTestLtsySharedData*	self=new(ELeave) CTestLtsySharedData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTestLtsySharedData::CTestLtsySharedData()
:	CBase()
,	iCallId(CTelephony::EISVCall1)
,	iCallId1(CTelephony::EISVCall2)
,	iTelServerConnected(EFalse)
,	iPhoneModuleLoaded(EFalse)
,	iPhoneOpened(EFalse)
,	iPhoneInitialised(EFalse)
,	iLineOpened(EFalse)
,	iCallOpened(EFalse)
,	iTelephony(NULL)
,	iActiveScheduler(NULL)
/*
Constructor
*/
	{
	}

CTestLtsySharedData::~CTestLtsySharedData()
/*
Destructor
*/
	{
	ClosePhone();
	}

void CTestLtsySharedData::ConstructL()
	{
	}

TInt CTestLtsySharedData::OpenPhone(const TName& aTsyName, CTestStep& aTestStep)
/*
This function will load TSY depending on the comms db
settings and opens the phone for making a call.

@param - TSY to be loaded
@param - CTestStep instance
@leave - system wide error codes
*/
	{
	ClosePhone();

	// Load the phone module depending on the comms DB settings
	TInt	ret=KErrNone;
	if(aTsyName.Compare(KSimTsyName) == 0 )
		{
		iTSYName = aTsyName;
		}
	else if (aTsyName.Compare(KDummyTsyName) == 0 )
		{
		iTSYName = aTsyName;
		}
	else
		{
		ret=KErrArgument;
		}

	if ( ret==KErrNone )
		{
		ret = ReopenPhone();
		}

	if ( ret!=KErrNone )
		{
		aTestStep.ERR_PRINTF2(_L("Failed to open phone. Error code = %d"), ret);
		aTestStep.SetTestStepResult(EFail);
		}

	return ret;
	}

TInt CTestLtsySharedData::ReopenPhone()
/*
This function will load TSY depending on the comms db
settings and opens the phone for making a call.

@param - CTestStep instance
@leave - system wide error codes
*/
	{
	TRAPD(ret, iActiveScheduler=new (ELeave) CActiveScheduler());

	if ( ret==KErrNone )
		{
		CActiveScheduler::Install(iActiveScheduler);
		ret=iServer.Connect();
		}

	if ( ret==KErrNone )
		{
		iTelServerConnected = ETrue;
		ret=iServer.LoadPhoneModule(iTSYName);
		}

	if ( ret==KErrNone )
		{
		iPhoneModuleLoaded = ETrue;
		TRAP(ret, iTelephony=CTelephony::NewL());
		}

	if ( ret==KErrNone )
		{
		// Open the phone
		if ( iTSYName.Compare(KDummyTsyName) == 0 )
			{
			// Open the phone with the Dummy mobile phone name
			ret = iMobilePhone.Open(iServer, KDMobile);
			}
		else
			{
			// open the phone with the simuplator phone
			ret = iMobilePhone.Open(iServer, KPhoneName);
			}
		iPhoneOpened = ret==KErrNone;
		}

	return ret;
	}

void CTestLtsySharedData::ClosePhone()
/*
Closes the RTelServer, RmobilePhone,RMobileCall and RMobileLine opened sessions.
*/
	{
	if ( iCallOpened )
		{
		iMobileCall.Close();
		iCallOpened=EFalse;
		}

	if ( iLineOpened )
		{
		iMobileLine.Close();
		iLineOpened=EFalse;
		}

	if ( iPhoneOpened )
		{
		iMobilePhone.Close();
		iPhoneOpened = EFalse;
		}

	if ( iTelephony!= NULL )
		{
		delete iTelephony;
		iTelephony=NULL;
		}

	if ( iPhoneModuleLoaded )
		{
		iServer.UnloadPhoneModule(iTSYName);
		iPhoneModuleLoaded = EFalse;
		}

	if ( iTelServerConnected )
		{
		iServer.Close();
		iTelServerConnected = EFalse;
		}

	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
	iActiveScheduler=NULL;
	}
