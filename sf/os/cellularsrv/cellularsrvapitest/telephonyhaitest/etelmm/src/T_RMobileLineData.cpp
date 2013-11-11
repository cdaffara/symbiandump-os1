/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include <cdbcols.h>
#include <e32base.h>
#include <e32def.h>

//	User Includes
#include "t_rmobilelinedata.h"

_LIT( KLineName, 		"Line" );
_LIT( KCallName, 		"Call");
_LIT( KDefaultSection, 	"Default" );
_LIT( KMaxPhoneCalls, 	"MaxPhoneCalls");

_LIT(KMobilePhoneKey, 	"RMobilePhone");

/*@{*/
//LIT's for commands
_LIT(KCmdOpen, 					"Open");
_LIT(KCmdClose, 				"Close");
_LIT(KCmdNotifyIncomingCall, 	"NotifyIncomingCall");
/*}@*/

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RMobileLineData* CT_RMobileLineData::NewL()
	{
	CT_RMobileLineData*	ret=new (ELeave) CT_RMobileLineData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_RMobileLineData::CT_RMobileLineData()
:	iActiveCallback(NULL),
	iMobileLine(NULL)
	{
	}

/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
void CT_RMobileLineData::ConstructL()
	{
	iMobileLine = new (ELeave) RMobileLine();
	iActiveCallback = CActiveCallback::NewL(*this);
	}

/**
 * Third phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */

void CT_RMobileLineData::InitialiseL()
	{
	CDataWrapperBase::InitialiseL();
	GetIntFromConfig(KDefaultSection, KMaxPhoneCalls, iMaxPhoneCalls  );

	TName* callName = NULL;
    for (TInt i = 0; i < iMaxPhoneCalls; ++i)
        {
        // Call names
        callName = new (ELeave) TName();
        CleanupStack::PushL(callName);
        iCallNames.Append(*callName);
        CleanupStack::Pop(callName);
        }
	}
/**
 * Public destructor
 */
CT_RMobileLineData::~CT_RMobileLineData()
	{
	if(iMobileLine)
		{
		delete iMobileLine;
		iMobileLine = NULL;
		}
	
	if(iActiveCallback)
		{
		delete iActiveCallback;
	    iActiveCallback=NULL;
		}
	// Empty arrays and also delete objects whose pointers are contained within
    iCallNames.Reset();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RMobileLineData::GetObject()
	{
	return iMobileLine;
	}

void CT_RMobileLineData::RunL(CActive* aActive, TInt aIndex)
    {
    DecOutstanding(); // One of the async calls has completed 
    TInt err = aActive->iStatus.Int(); 
    if( err != KErrNone ) 
    	{ 
        ERR_PRINTF2(_L("RunL Error %d"), err); 
        SetAsyncError( aIndex, err ); 
        } 
    else 
        { 
        INFO_PRINTF1(_L("RunL  completed successfully")); 
        } 
    }

TBool CT_RMobileLineData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;
	
	if ( aCommand==KCmdOpen )
		{
		DoCmdOpen(aSection);
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdNotifyIncomingCall)
		{
		DoCmdNotifyIncomingCall(aSection, aAsyncErrorIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}
	return ret;
	}

void CT_RMobileLineData::DoCmdOpen(const TDesC& aSection)
	{
    INFO_PRINTF1(_L("*START*CT_RMobileLineData::OpenLine"));
    TInt error(0);
    TBool dataOk = ETrue;
    TPtrC resultLineName;
    if( !GetStringFromConfig(aSection, KLineName, resultLineName) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KLineName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    TPtrC mobilePhoneName;
    if( !GetStringFromConfig(aSection, KMobilePhoneKey, mobilePhoneName) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMobilePhoneKey);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    if(dataOk)
    	{
		RMobilePhone* mobilePhoneObject = static_cast<RMobilePhone*>(GetDataObjectL(mobilePhoneName));
		INFO_PRINTF1(_L("Opening Line"));
	    error = iMobileLine->Open(*mobilePhoneObject, resultLineName);
	    if(error != KErrNone)
	    	{
	    	ERR_PRINTF2(_L("Failed to open Line with error %d"), error);
	    	SetError(error);
	    	}
	    else
	    	{
	    	INFO_PRINTF1(_L("OpenLine succeeded"));
	    	}
    	}
    INFO_PRINTF1(_L("*END*CT_RMobileLineData::OpenLine"));
	}

void CT_RMobileLineData::DoCmdClose()
	{
    INFO_PRINTF1(_L("*START*CT_RMobileLineData::CloseLine"));
    iMobileLine->Close();
    INFO_PRINTF1(_L("CloseLine succeeded"));
	INFO_PRINTF1(_L("*END*CT_RMobileLineData::CloseLine"));
	}

void CT_RMobileLineData::DoCmdNotifyIncomingCall(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::DoCmdNotifyIncomingCall"));

	TInt callNameParameter;
	if( !GetIntFromConfig(aSection, KCallName, callNameParameter ) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KCallName);
    	SetBlockResult(EFail);
    	}
	else
		{
		TName* CallName = NULL;
		TRAPD(error, CallName = GetCallNameL(callNameParameter));
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Left while getting call name with error %d"), error);
			SetError(error);
			}
		else
			{
			INFO_PRINTF1(_L("Start NotifyIncomingCall Asynchronous call."));
			iMobileLine->NotifyIncomingCall(iActiveCallback->iStatus, *CallName);
		    iActiveCallback->Activate(aAsyncErrorIndex);
		    IncOutstanding();
			}
		}
    INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::DoCmdNotifyIncomingCall"));
	}

TName* CT_RMobileLineData::GetCallNameL(TInt aCall)
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::GetCallNameL"));
	
	INFO_PRINTF2(_L("Get TName: %d name"), aCall);

	// Check that over/under flow does not occur
	if (aCall < 0 || aCall >= iCallNames.Count())
		{
    	ERR_PRINTF2(_L("There is no such name as (%d)"), aCall);
    	User::Leave(KErrArgument);
		}
	
	INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::GetCallNameL"));
	return &iCallNames[aCall];
	}

