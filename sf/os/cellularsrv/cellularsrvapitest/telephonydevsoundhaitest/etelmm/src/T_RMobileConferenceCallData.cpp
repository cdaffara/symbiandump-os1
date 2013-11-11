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

#include "T_RMobileConferenceCallData.h"
#include <e32property.h>


_LIT( KNumOfCalls,   	"NumOfCalls" );

_LIT(KMobilePhoneKey, 	"RMobilePhone");

_LIT(KCmdClose, 					"Close");
_LIT(KCmdCreateConference, 			"CreateConference");
_LIT(KCmdEnumerateCalls, 			"EnumerateCalls");
_LIT(KCmdOpen, 						"Open");

/**
 * Two phase constructor
 *
 * @leave	system wide error
 */
CT_RMobileConferenceCallData* CT_RMobileConferenceCallData::NewL()
	{
	CT_RMobileConferenceCallData*	ret=new (ELeave) CT_RMobileConferenceCallData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Protected constructor. First phase construction
 */
CT_RMobileConferenceCallData::CT_RMobileConferenceCallData()
:	iActiveCallback(NULL),
	iMobileConferenceCall(NULL)
	{
	}

void CT_RMobileConferenceCallData::ConstructL()
	{
	iMobileConferenceCall = new (ELeave) RMobileConferenceCall();
	iActiveCallback = CActiveCallback::NewL(*this);
	}

/**
 * Public destructor
 */
CT_RMobileConferenceCallData::~CT_RMobileConferenceCallData()
	{
	delete iMobileConferenceCall;
	iMobileConferenceCall = NULL;
	delete iActiveCallback;
    iActiveCallback=NULL;
	}

TAny* CT_RMobileConferenceCallData::GetObject()	
	{
	return iMobileConferenceCall;
	}

void CT_RMobileConferenceCallData::RunL(CActive* aActive, TInt aIndex)
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

TBool CT_RMobileConferenceCallData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;
	
	if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdCreateConference)
		{
		DoCmdCreateConference(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdEnumerateCalls)
		{
		DoCmdEnumerateCalls(aSection);
		}
	else if ( aCommand==KCmdOpen)
		{
		DoCmdOpen(aSection);
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret = EFalse;
		}
	
	return ret;
	}



void CT_RMobileConferenceCallData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::DoCmdCloseConferenceCall"));
	
	iMobileConferenceCall->Close();

    INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::DoCmdCloseConferenceCall"));
	}


void CT_RMobileConferenceCallData::DoCmdCreateConference(const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::DoCmdCreateConferenceCall"));
	
    iMobileConferenceCall->CreateConference(iActiveCallback->iStatus);
	iActiveCallback->Activate(aAsyncErrorIndex);
    IncOutstanding();
    
	INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::DoCmdCreateConferenceCall"));
	}


void CT_RMobileConferenceCallData::DoCmdEnumerateCalls(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::DoCmdEnumerateCallsOnConference"));

    TInt expectedCalls; 
    if( !GetIntFromConfig(aSection, KNumOfCalls, expectedCalls) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KNumOfCalls);
    	SetBlockResult(EFail);
    	}
    else
    	{
	    INFO_PRINTF2(_L("Expecting conference call to contain [%d] calls"), expectedCalls);
	    TInt calls(0);
	    TRAPD( error, iMobileConferenceCall->EnumerateCalls(calls) );
	    if(error != KErrNone)
	    	{
	    	ERR_PRINTF2(_L("Failed to enumerate calls with error %d"), error);
	    	SetError(error);
	    	}
	    else
	    	{
		    INFO_PRINTF2(_L("Conference contained [%d] calls"), calls);
		    if (expectedCalls != calls)
		        {
		        ERR_PRINTF3(_L("Obtained number of calls %d is different from expected number of calls %d"), calls, expectedCalls);
		    	SetBlockResult(EFail);
		        }
		    else
		    	{
		    	INFO_PRINTF1(_L("DoCmdEnumerateCallsOnConference succeeded"));
		    	}
	    	}
    	}
    INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::DoCmdEnumerateCallsOnConference"));
	}

void CT_RMobileConferenceCallData::DoCmdOpen(const TTEFFunction& aSection)
	{
	INFO_PRINTF1(_L("*START*CT_VoiceCallDriverData::DoCmdOpenConferenceCall"));

	TPtrC mobilePhoneName;
	if( !GetStringFromConfig(aSection, KMobilePhoneKey, mobilePhoneName))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KMobilePhoneKey);
    	SetBlockResult(EFail);
    	}
	else
		{
		RMobilePhone* MobilePhoneObject = static_cast<RMobilePhone*>(GetDataObjectL(mobilePhoneName));
		TRAPD( error, iMobileConferenceCall->Open(*MobilePhoneObject) );
	    if (error != KErrNone)
	        {
	        ERR_PRINTF2(_L("Conference call open failed [%d]"), error);
	    	SetError(error);
	        }
	    else
	    	{
	    	INFO_PRINTF1(_L("DoCmdOpenConferenceCall succeeded"));
	    	}
		}
    INFO_PRINTF1(_L("*END*CT_VoiceCallDriverData::DoCmdOpenConferenceCall"));
	}


