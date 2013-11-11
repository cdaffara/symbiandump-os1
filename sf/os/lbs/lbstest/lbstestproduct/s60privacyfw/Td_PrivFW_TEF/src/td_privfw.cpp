// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include <e32base.h>
//#include <utfw.h>
#include "LcfFIPluginController.h"
#include <EPos_CPosNetworkPrivacy.h>
#include <EPos_CPosGSMPrivacyRequestInfo.h>
#include <EPos_CPosSUPLPrivacyRequestInfo.h>
#include <e32cmn.h>
#include <epos_privacy.h>
#include <EPos_RPosRequestorStack.h>
#include <EPos_PrivacyNotifier.hrh>

#include "td_privfw.h"
#include "tc_timer.h"
#include "td_rprivacyserver.h"

// CONSTANTS

// error messages
_LIT( KErrCreateGSMRequest,		"Error, GSM request creation did not succeed, code %d" );
//_LIT( KErrCreateCDMARequest, 	"Error, CDMA request creation did not succeed, code %d" );
_LIT( KErrCreateSUPLRequest, 	"Error, SUPL request creation did not succeed, code %d" );
_LIT( KErrRequestNotSupported,	"Error, issuing a request failed with code %d" );
_LIT( KErrSetRequestor,			"Error when setting requestor, code %d" );
_LIT( KErrSetLCSClient,			"Error when setting LCS client, code %d" );
_LIT( KErrCreateNetPrivacy,		"Error when creating Network Privacy object, code: %d" );
_LIT( KErrExpectedVerification,         "Error: expected verification");
_LIT( KErrExpectedNotification,         "Error: expected notification");
_LIT( KErrWrongExitReason,         "Error: wrong exit reason supplied with dialog");
_LIT( KErrReqIDIncorrectPhone, "Requestor id passed incorrectly (phone)");
_LIT( KErrReqIDIncorrectEmail, "Requestor id passed incorrectly (email)");
_LIT( KErrReqIDIncorrectURL,   "Requestor id passed incorrectly (url)");
_LIT( KErrReqIDIncorrectName,  "Requestor id passed incorrectly (name)");
_LIT( KErrReqIDIncorrectSIPURL,"Requestor id passed incorrectly (SIPURL)");
_LIT( KErrReqIDIncorrectIMS,   "Requestor id passed incorrectly (IMS)");
_LIT( KErrReqIDIncorrectMIN,   "Requestor id passed incorrectly (MIN)");
_LIT( KErrReqIDIncorrectMDN,   "Requestor id passed incorrectly (MDN)");

_LIT( KErrLcsIDIncorrectName,   "LCS client id passed incorrectly (name)");
_LIT( KErrLcsIDIncorrectPhone,  "LCS client id passed incorrectly (phone)");
_LIT( KErrLcsIDIncorrectUrl,    "LCS client id passed incorrectly (url)");
_LIT( KErrLcsIDIncorrectEmail,  "LCS client id passed incorrectly (email)");
_LIT( KErrLcsIDIncorrectSIPURL, "LCS client id passed incorrectly (SIPURL)");
_LIT( KErrLcsIDIncorrectIMS,    "LCS client id passed incorrectly (IMS)");
//_LIT( KErrLcsIDIncorrectMIN,    "LCS client id passed incorrectly (MIN)");
//_LIT( KErrLcsIDIncorrectMDN,    "LCS client id passed incorrectly (MDN)");

_LIT(KErrLcsOrReqIdIncorrect, "LCS client or requestor id incorrect");

// log messages
//_LIT( KTxtGsmRequestorTypeName, 		"Requestor identity successfully set (Logical name)" );
_LIT( KTxtGsmRequestorTypePhone, 		"Requestor identity successfully set(Phone number)" );
_LIT( KTxtGsmRequestorTypeURL, 			"Requestor identity successfully set (URL)" );
_LIT( KTxtGsmRequestorTypeEmail, 		"Requestor identity successfully set (E-mail)" );
_LIT( KTxtGsmRequestorTypeSIPURL, 		"Requestor identity successfully set (SIPURL)" );
_LIT( KTxtGsmRequestorTypeIMS,  		"Requestor identity successfully set (IMS)" );

_LIT( KTxtGsmLCSClientTypeName, 		"LCS client successfully set (Logical name)" );
_LIT( KTxtGsmLCSClientTypePhone, 		"LCS client successfully set(Phone number)" );
_LIT( KTxtGsmLCSClientTypeURL, 			"LCS client successfully set (URL)" );
_LIT( KTxtGsmLCSClientTypeEmail, 		"LCS client successfully set (E-mail)" );
_LIT( KTxtGsmLCSClientTypeSIPURL, 		"LCS client successfully set (SIPURL)" );
_LIT( KTxtGsmLCSClientTypeIMS , 		"LCS client successfully set (IMS)" );

//_LIT( KTxtCDMARequestorTypeMIN, 		"Requestor identity successfully set (MIN)" );
//_LIT( KTxtCDMARequestorTypeMDN, 		"Requestor identity successfully set (MDN)" );

//_LIT( KTxtCDMALCSClientTypeMIN, 		"LCS client successfully set (MIN)" );
//_LIT( KTxtCDMALCSClientTypeMDN, 		"LCS client successfully set(MDN)" );

_LIT( KTxtSUPLRequestorTypeName, 		"Requestor identity successfully set (Logical name)" );
_LIT( KTxtSUPLRequestorTypePhone, 		"Requestor identity successfully set(Phone number)" );
_LIT( KTxtSUPLRequestorTypeURL, 		"Requestor identity successfully set (URL)" );
_LIT( KTxtSUPLRequestorTypeEmail, 		"Requestor identity successfully set (E-mail)" );
_LIT( KTxtSUPLRequestorTypeSIPURL, 		"Requestor identity successfully set (SIPURL)" );
_LIT( KTxtSUPLRequestorTypeIMS,  		"Requestor identity successfully set (IMS)" );
_LIT( KTxtSUPLRequestorTypeMIN, 		"Requestor identity successfully set (MIN)" );
_LIT( KTxtSUPLRequestorTypeMDN, 		"Requestor identity successfully set (MDN)" );

_LIT( KTxtSUPLLCSClientTypeName, 		"LCS client successfully set (Logical name)" );
_LIT( KTxtSUPLLCSClientTypePhone, 		"LCS client successfully set(Phone number)" );
_LIT( KTxtSUPLLCSClientTypeURL, 		"LCS client successfully set (URL)" );
_LIT( KTxtSUPLLCSClientTypeEmail, 		"LCS client successfully set (E-mail)" );
_LIT( KTxtSUPLLCSClientTypeSIPURL, 		"LCS client successfully set (SIPURL)" );
_LIT( KTxtSUPLLCSClientTypeIMS , 		"LCS client successfully set (IMS)" );
_LIT( KTxtSUPLLCSClientTypeMIN, 		"LCS client successfully set (MIN)" );
_LIT( KTxtSUPLLCSClientTypeMDN, 		"LCS client successfully set(MDN)" );


_LIT( KTxtVerificationIssued, 			"Verification request issued, ID: %d");
_LIT( KTxtNotificationIssued, 			"Notification request issued, ID: %d");

_LIT( KTxtGsmRequestorCreated, 			"GSM requestor succesfully created");
//_LIT( KTxtGSMNotificationSupported, 	"GSM notification request supported");

//_LIT( KTxtCDMARequestCreated, 			"CDMA requestor succesfully created");
//_LIT( KTxtCDMANotificationSupported,	"CDMA notification request supported");

_LIT( KTxtSUPLRequestorCreated, 		"SUPL requestor succesfully created");
//_LIT( KTxtSUPLNotificationSupported, 	"SUPL notification request supported");

// requestor constants
_LIT( KTxtRequestorName,	"Test driver Td_PrivFW" );
_LIT( KTxtRequestorURL,		"www.nokia.com" );
_LIT( KTxtRequestorPhone,	"0501231231" );
_LIT( KTxtRequestorEmail,	"bili@just.se" );
_LIT( KTxtRequestorSIPURL,	"James@Hill.se" );
_LIT( KTxtRequestorIMS,	    "5665656656" );
_LIT( KTxtRequestorMIN,	    "9999999999" );
_LIT( KTxtRequestorMDN,	    "5665656656" );

//_LIT( KTestCaseId,	    "tc_id" );


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CTdPrivFW::CTdPrivFW(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStep(aParent),
	 		iResult(KErrNone), iLogged(EFalse)
{
	SetTestStepName(KLbsPrivFwStep);
}

// Two-phased constructor.
CTdPrivFW* CTdPrivFW::NewL(CT_LbsPrivFwServer& aParent)
{
    CTdPrivFW* self = new (ELeave) CTdPrivFW(aParent);
    CleanupStack::PushL( self ); 
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// Destructor
CTdPrivFW::~CTdPrivFW()
{
    iArray.Close();
    iResultArray.Close();
	delete iPluginManager;
	delete iScheduler;
}


// EPOC default constructor can leave.
void CTdPrivFW::ConstructL()
{
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iPluginManager = CLcfNotifierManager::NewL();
	iPluginManager->ClearAllInputAndOutputL();
}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CTdPrivFW::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMTLRPrivNotifierStep_Main::doTestStepL()"));
	ASSERT(TestStepResult() == EPass);

	TInt testCaseId;
	if (!GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
		{
		INFO_PRINTF1(_L("Unrecognised test case"));
		ASSERT(FALSE);
		}
	switch (testCaseId)
		{
		case 1:	// Test case LBS-PRIVFW-NOKIA-0001
			TCPFW001L();
			break;
		
		case 2:	// Test case LBS-PRIVFW-NOKIA-0002
			TCPFW002L();
			break;
		
		case 3:	// Test case LBS-PRIVFW-NOKIA-0003
			TCPFW003L();
			break;
		
		case 4:	// Test case LBS-PRIVFW-NOKIA-0004
			TCPFW004L();
			break;
		
		case 5:	// Test case LBS-PRIVFW-NOKIA-0005
			TCPFW005L();
			break;
		
		//case 6:	// Test case LBS-PRIVFW-NOKIA-0006
			//TCPFW006L();
		//	break;
		
		//case 7:	// Test case LBS-PRIVFW-NOKIA-0007
		//	TCPFW007L();
		//	break;
		
		//case 8:	// Test case LBS-PRIVFW-NOKIA-0008
		//	TCPFW008L();
		//	break;
		
		case 9:	// Test case LBS-PRIVFW-NOKIA-0009
			TCPFW009L();
			break;
		
		case 10:	// Test case LBS-PRIVFW-NOKIA-0010
			TCPFW010L();
			break;
		
		case 11:	// Test case LBS-PRIVFW-NOKIA-0011
			TCPFW011L();
			break;
		
		case 12:	// Test case LBS-PRIVFW-NOKIA-0012
			TCPFW012L();
			break;
		
		case 13:	// Test case LBS-PRIVFW-NOKIA-0013
			TCPFW013L();
			break;
		
		//case 14:	// Test case LBS-PRIVFW-NOKIA-0014
		//	TCPFW014L();
		//	break;
		
		//case 15:	// Test case LBS-PRIVFW-NOKIA-0015
		//	TCPFW015L();
		//	break;
		
		case 17:	// Test case LBS-PRIVFW-NOKIA-0017
			TCPFW017L();
			break;
		
		case 18:	// Test case LBS-PRIVFW-NOKIA-0018
			TCPFW018L();
			break;
		
		case 20:	// Test case LBS-PRIVFW-NOKIA-0020
			TCPFW020L();
			break;
		
		//case 21:	// Test case LBS-PRIVFW-NOKIA-0021
		//	TCPFW021L();
		//	break;
		
		case 22:	// Test case LBS-PRIVFW-NOKIA-0022
			TCPFW022L();
			break;
		
		case 23:	// Test case LBS-PRIVFW-NOKIA-0023
			TCPFW023L();
			break;
		
		case 24:	// Test case LBS-PRIVFW-NOKIA-0024
			TCPFW024L();
			break;
		
		case 25:	// Test case LBS-PRIVFW-NOKIA-0025
			TCPFW025L();
			break;

        	case 26:    // Test case LBS-PRIVFW-NOKIA-0026
            		TCPFW026L();
            		break;	
            
        	case 27:    // Test case LBS-PRIVFW-NOKIA-0027
            		TCPFW027L();
            		break;
            
        	case 28:    // Test case LBS-PRIVFW-NOKIA-0028
            		TCPFW028L();
            		break;

        	case 29:    // Test case LBS-PRIVFW-NOKIA-0029
            		TCPFW029L();
            		break;

		default:
			{
			INFO_PRINTF2(_L("Unrecognised test case id %d"), testCaseId);
			ASSERT(FALSE);	
			}
			break;		
		}
	return TestStepResult();
	}

// ---------------------------------------------------------
// Ctd3Test::AssertTrueSecL
// copy from CPosTp79::AssertTrueSecL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CTdPrivFW::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
{
    if (!aCondition)
    {
        TBuf<100> buf;
        buf.Format(aErrorMsg, aErrorCode);
  //      LogErrorAndLeave(buf);
    }
}


CPosNetworkPrivacy* CTdPrivFW::ConnectToNetworkPrivacyAPIL()
	{
    TBuf<80> errorMessage;
    TInt err = KErrNone;

	CPosNetworkPrivacy* api = NULL;
	
	TRAP( err, api = CPosNetworkPrivacy::NewL() );
    if( err != KErrNone )
    	{
        errorMessage.Format( KErrCreateNetPrivacy, err );
        INFO_PRINTF1(errorMessage);
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
    	}
    return api;
	}


void CTdPrivFW::HandleVerifyComplete( TInt aRequestId, TInt aCompletionCode )
{
    _LIT( KVerifyComplete, "HandleVerifyComplete, Id: %d, completion code: %d" );
    TBuf<80> message;

    message.Format( KVerifyComplete, aRequestId, aCompletionCode );
    INFO_PRINTF1(message);

    TInt index = iArray.Find( aRequestId );
    if( index < KErrNone )
    {
        INFO_PRINTF1(_L( "Verification id not found" ));
    }
    else
    {
        iArray.Remove( index );
    }

   	iResult = aCompletionCode;

    iResultArray.Append( aCompletionCode );
    
    if (!AnyCompletionWaiting())
    	{
    	StopWaiting();
    	}
}

TBool CTdPrivFW::CheckQNAmounts( CLcfNotifierManager* iPluginManager, TInt aVerifies, TInt aNotifies )
{

    TInt verifications = KErrNone;
    TInt notifications = KErrNone;
    _LIT( KErrGetDiagnostics, "Get diagnostic info leaved with code %d" );
    _LIT( KErrTotalNQAmount, "Error, wrong verifications + notifications total, wanted: %d, got %d" );
    _LIT( KErrIndVerificationAmount, "Error, wrong number of verifications, wanted: %d, got %d" );
    _LIT( KErrIndNotificationAmount, "Error, wrong number of notifications, wanted: %d, got %d" );
    TInt err = KErrNone;
    TBool retVal = ETrue;
    TBuf<80> errorMessage;

    // Get diagnostics
    CLcfPluginDiagnostics* diagnostics = NULL;
    
    TInt nb = iPluginManager->NbOfDiagnostics();
    
    if( nb != aVerifies + aNotifies )
    {
        errorMessage.Format( KErrTotalNQAmount, aVerifies + aNotifies, nb );
        INFO_PRINTF1(errorMessage);
        retVal = EFalse;
    }

    TInt i = 0;

    for( i = 0; i < nb; i++ )
    {
        TRAP( err, iPluginManager->GetDiagnosticsL(i, diagnostics) );
        if( err != KErrNone )
        {
            retVal = EFalse;
            errorMessage.Format( KErrGetDiagnostics, err );
        	INFO_PRINTF1(errorMessage);
            continue;
        }
        
        CleanupStack::PushL(diagnostics);
        
        if( diagnostics->VerificationRequest() )
        {
            verifications++;
        }
        else
        {
            notifications++;
        }
        
        CleanupStack::PopAndDestroy(); // diagnostics
    }
    
    if( verifications != aVerifies )
    {
        errorMessage.Format( KErrIndVerificationAmount, aVerifies, verifications );
        INFO_PRINTF1(errorMessage);
        retVal = EFalse;
    }
    
    if( notifications != aNotifies )
    {
        errorMessage.Format( KErrIndNotificationAmount, aNotifies, notifications );
        INFO_PRINTF1(errorMessage);
        retVal = EFalse;
    }

    return retVal;
}





// This function uses Active Scheduler so that events can be received.
// When this functions is called, all events that are pending are
// handled.
//
void CTdPrivFW::WaitForModuleEventsL()
{    
    // Execution will stop until the moment when iWait.AsyncStop()
    // is called, Active Objects are still running.
    iWait.Start();
    
    INFO_PRINTF1(_L("WaitForModuleEventsL() exited"));
    
}

// This function uses Active Scheduler so that events can be received.
// When this functions is called, all events that are pending are
// handled.
//
void CTdPrivFW::WaitForTimeL(const TTimeIntervalMicroSeconds32& aTime)
{
    // Create CIdle. Idle will call IdleCallback when
    // no other asynchonous events are ready.
    CIdle* idle = CIdle::NewL(CActive::EPriorityIdle);
	iWaitTime = aTime;
	    
    // Start idle
    idle->Start(TCallBack(IdleCallbackTime, this));
    
    // Execution will stop until the moment when iWait.AsyncStop()
    // is called, Active Objects are still running.
    iWait.Start();
    
    // We will get here only after CIdle::RunL will be executed.
    INFO_PRINTF1(_L("Wait stopped"));
    
    // We will get here only after CIdle::RunL will be executed.
    delete idle;
    
    INFO_PRINTF1(_L("WaitForModuleEventsL() exited"));
    
}

TBool CTdPrivFW::AnyCompletionWaiting()
{
    if ( iArray.Count() == 0 )
    {
        return EFalse;
    }
    else
    {
        return ETrue;
    }
}

// This function is called when no ohter active object is ready
// to run. In this case this means when all module events
// are received.
//
TInt CTdPrivFW::IdleCallback(TAny *aThisObj)
{
    CTdPrivFW* thisPtr = (CTdPrivFW*)aThisObj;
    
    if ( thisPtr->AnyCompletionWaiting() ) 
    {
        return 1;
    }
    else
    {
        thisPtr->StopWaiting();        
        // 0 means that callback will not be called again
        return 0;
    }
}

// This function is called when no ohter active object is ready
// to run.
//
TInt CTdPrivFW::IdleCallbackTime(TAny *aThisObj)
{
    CTdPrivFW* thisPtr = (CTdPrivFW*)aThisObj;
    CTcTimer* timer = new (ELeave) CTcTimer( *thisPtr );
	CleanupStack::PushL(timer);
    timer->ConstructL();
    CleanupStack::Pop();
    
    timer->After(thisPtr->iWaitTime);
    
    
    // 0 means that callback will not be called again
    return 0;
}

void CTdPrivFW::StopWaiting()
{
    iWait.AsyncStop();
}

CPosGSMPrivacyRequestInfo* CTdPrivFW::CreateGSMRequestInfoL()
	{
	TInt err;
	CPosGSMPrivacyRequestInfo* reqInfo = NULL;
		
    TRAP( err, reqInfo = CPosGSMPrivacyRequestInfo::NewL() );
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrCreateGSMRequest, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorCreated);
    	}	
    return reqInfo;
	}

#if 0
CPosCDMAPrivacyRequestInfo* CTdPrivFW::CreateCDMARequestInfoL()
	{
	TInt err;
	CPosCDMAPrivacyRequestInfo* reqInfo = NULL;
		
    TRAP( err, reqInfo = CPosCDMAPrivacyRequestInfo::NewL() );
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrCreateCDMARequest, err );
        INFO_PRINTF1(errorMessage);
       User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtCDMARequestCreated);
    	}

    return reqInfo;
	}
#endif


CPosSUPLPrivacyRequestInfo* CTdPrivFW::CreateSUPLRequestInfoL()
	{
	TInt err;
	CPosSUPLPrivacyRequestInfo* reqInfo = NULL;
		
    TRAP( err, reqInfo = CPosSUPLPrivacyRequestInfo::NewL() );
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrCreateSUPLRequest, err );
        INFO_PRINTF1(errorMessage);
	    User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorCreated);
    	}

    return reqInfo;
	}

	
void CTdPrivFW::MakeVerificationL(CPosNetworkPrivacy& aApi,
							 const CPosNetworkPrivacyRequestInfo& aReq,
							 TInt& aId,
							 const CPosNetworkPrivacy::TRequestDecision& aTimeoutStrategy)
	{
	TInt err;

	TRAP( err, aApi.VerifyLocationRequestL( aReq, aId, *this, aTimeoutStrategy ));
   	if( err == KErrNone )
   		{
		
		errorMessage.Format( KTxtVerificationIssued, aId);
        INFO_PRINTF1(errorMessage);
   		}
   	else
   		{
       	errorMessage.Format( KErrRequestNotSupported, err );
        INFO_PRINTF1(errorMessage);
		User::Leave(err);
   		}
	}

void CTdPrivFW::MakeNotificationL(CPosNetworkPrivacy& aApi,
							 const CPosNetworkPrivacyRequestInfo& aReq,
							 TInt& aId)
	{
	TInt err;

	TRAP( err, aApi.NotifyLocationRequestL( aReq, aId));
   	if( err == KErrNone )
   		{
		errorMessage.Format( KTxtNotificationIssued, aId);
        INFO_PRINTF1(errorMessage);
   		}
   	else
   		{
       	errorMessage.Format( KErrRequestNotSupported, err );
        INFO_PRINTF1(errorMessage);
		User::Leave(err);
   		}
	}

void CTdPrivFW::ResetAndDestroy(TAny* aPointerArray)
    {
    (static_cast< RPointerArray<CPosNetworkPrivacyRequestInfo>* > (aPointerArray))->ResetAndDestroy();
    }
	
	
#if 0
void CTdPrivFW::LogEnvironment()
	{
	RProcess process;
	
	errorMessage.Format(_L("Process ID (hex): %x"), process.Id());
    iLog->Put( errorMessage );

	errorMessage.Format(_L("Process SecureID (hex): %x"), process.SecureId());
    iLog->Put( errorMessage );

	errorMessage.Format(_L("Running with following capabilities:"));
    iLog->Put( errorMessage );
		
	if (process.HasCapability(ECapabilityTCB))
		{
		errorMessage.Format(_L("TCB"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityCommDD))
		{
		errorMessage.Format(_L("CommDD"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityPowerMgmt))
		{
		errorMessage.Format(_L("PowerMgmt"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityMultimediaDD))
		{
		errorMessage.Format(_L("MultimediaDD"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityReadDeviceData))
		{
		errorMessage.Format(_L("ReadDeviceData"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityWriteDeviceData))
		{
		errorMessage.Format(_L("WriteDeviceData"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityDRM))
		{
		errorMessage.Format(_L("DRM"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityTrustedUI))
		{
		errorMessage.Format(_L("TrustedUI"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityProtServ))
		{
		errorMessage.Format(_L("ProtServ"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityDiskAdmin))
		{
		errorMessage.Format(_L("DiskAdmin"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityNetworkControl))
		{
		errorMessage.Format(_L("NetworkControl"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityAllFiles))
		{
		errorMessage.Format(_L("AllFiles"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilitySwEvent))
		{
		errorMessage.Format(_L("SwEvent"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityNetworkServices))
		{
		errorMessage.Format(_L("NetworkServices"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityLocalServices))
		{
		errorMessage.Format(_L("LocalServices"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityReadUserData))
		{
		errorMessage.Format(_L("ReadUserData"));
    	iLog->Put( errorMessage );
		}

    if (process.HasCapability(ECapabilityWriteUserData))
		{
		errorMessage.Format(_L("WriteUserData"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityLocation))
		{
		errorMessage.Format(_L("Location"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilitySurroundingsDD))
		{
		errorMessage.Format(_L("SurroundingsDD"));
    	iLog->Put( errorMessage );
		}

	if (process.HasCapability(ECapabilityUserEnvironment))
		{
		errorMessage.Format(_L("UserEnvironment"));
    	iLog->Put( errorMessage );
		}

	errorMessage.Zero();
    iLog->Put( errorMessage );

	}
#endif




void CTdPrivFW::SetRequestorNameL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aName)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(errorMessage);
    	}   

	}

void CTdPrivFW::SetRequestorEmailL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aEmail)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aEmail, CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorTypeEmail);
    	}   

	}

void CTdPrivFW::SetRequestorURLL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aUrl, CPosGSMPrivacyRequestInfo::EIdTypeURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorTypeURL);
    	}   

	}

void CTdPrivFW::SetRequestorPhoneL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosGSMPrivacyRequestInfo::EIdTypeMSISDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorTypePhone);
    	}   

	}



void CTdPrivFW::SetRequestorSIPURLL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aSipUrl, CPosGSMPrivacyRequestInfo::EIdTypeSIPURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorTypeSIPURL);
    	}   

	}


void CTdPrivFW::SetRequestorIMSL(CPosGSMPrivacyRequestInfo& aInfo,
									const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aIdentity, CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmRequestorTypeIMS);
    	}   

	}

#if 0 //CDMA Test not in scope
void CTdPrivFW::SetRequestorMINL(CPosCDMAPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosCDMAPrivacyRequestInfo::EIdTypeMIN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtCDMARequestorTypeMIN);
    	}   

	}

void CTdPrivFW::SetRequestorMDNL(CPosCDMAPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosCDMAPrivacyRequestInfo::EIdTypeMDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtCDMARequestorTypeMDN);
    	}   

	}
#endif

void CTdPrivFW::SetRequestorNameL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aName)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aName, CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeName);
    	}   

	}

void CTdPrivFW::SetRequestorEmailL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aEmail)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aEmail, CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeEmail);
    	}   

	}

void CTdPrivFW::SetRequestorURLL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aUrl, CPosSUPLPrivacyRequestInfo::EIdTypeURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeURL);
    	}   

	}


void CTdPrivFW::SetRequestorPhoneL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypePhone);
    	}   

	}


 void CTdPrivFW::SetRequestorSIPURLL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aSipUrl, CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeSIPURL);
    	}   

	}


void CTdPrivFW::SetRequestorIMSL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aIdentity, CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeIMS);
    	}   

	}


void CTdPrivFW::SetRequestorMINL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosSUPLPrivacyRequestInfo::EIdTypeMIN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeMIN);
    	}   

	}

void CTdPrivFW::SetRequestorMDNL(CPosSUPLPrivacyRequestInfo& aInfo,
									const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetRequestorL(aNumber, CPosSUPLPrivacyRequestInfo::EIdTypeMDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetRequestor, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLRequestorTypeMDN);
    	}   

	}

 


void CTdPrivFW::SetServiceNameL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aName)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmLCSClientTypeName);
    	}   
	}

void CTdPrivFW::SetServiceEmailL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aEmail, CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmLCSClientTypeEmail);
    	}   
	}

void CTdPrivFW::SetServiceURLL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aUrl, CPosGSMPrivacyRequestInfo::EIdTypeURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmLCSClientTypeURL);
    	}   
	}
	
void CTdPrivFW::SetServicePhoneL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aNumber, CPosGSMPrivacyRequestInfo::EIdTypeMSISDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtGsmLCSClientTypePhone);
    	}   
	}


void CTdPrivFW::SetServiceSIPURLL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aSipUrl, CPosGSMPrivacyRequestInfo::EIdTypeSIPURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
         INFO_PRINTF1(KTxtGsmLCSClientTypeSIPURL);
    	}   
	}
	
void CTdPrivFW::SetServiceIMSL(CPosGSMPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aIdentity, CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
         INFO_PRINTF1(KTxtGsmLCSClientTypeIMS);
    	}   
	}
	
#if 0	
void CTdPrivFW::SetServiceMINL(CPosCDMAPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aSipUrl, CPosCDMAPrivacyRequestInfo::EIdTypeMIN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtCDMALCSClientTypeMIN);
    	}   
	}
	
void CTdPrivFW::SetServiceMDNL(CPosCDMAPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aIdentity, CPosCDMAPrivacyRequestInfo::EIdTypeMDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
         INFO_PRINTF1(KTxtCDMALCSClientTypeMDN);
    	}   
	}	
#endif

 void CTdPrivFW::SetServiceNameL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aName)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aName,CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeName);
    	}   
	}

void CTdPrivFW::SetServiceEmailL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aEmail)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aEmail, CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeEmail);
    	}   
	}

void CTdPrivFW::SetServiceURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aUrl, CPosSUPLPrivacyRequestInfo::EIdTypeURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeURL);
    	}   
	}
	
void CTdPrivFW::SetServicePhoneL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aNumber)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aNumber, CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypePhone);
    	}   
	}


void CTdPrivFW::SetServiceSIPURLL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aSipUrl, CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeSIPURL);
    	}   
	}
	
void CTdPrivFW::SetServiceIMSL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aIdentity, CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeIMS);
    	}   
	}
	
	
void CTdPrivFW::SetServiceMINL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aSipUrl)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aSipUrl, CPosSUPLPrivacyRequestInfo::EIdTypeMIN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeMIN);
    	}   
	}
	
void CTdPrivFW::SetServiceMDNL(CPosSUPLPrivacyRequestInfo& aInfo,
   						const TDesC& aIdentity)
	{
	TInt err;
	
    TRAP(err, aInfo.SetLCSClientL(aIdentity, CPosSUPLPrivacyRequestInfo::EIdTypeMDN));
	if(err != KErrNone)
    	{
        errorMessage.Format( KErrSetLCSClient, err );
        INFO_PRINTF1(errorMessage);
        User::Leave(err);
		}
	else
    	{        
        INFO_PRINTF1(KTxtSUPLLCSClientTypeMDN);
    	}   
	}	

//
// TEST CASE FUNCTIONS
//

void CTdPrivFW::TCPFW001L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	TBuf<300> requestor;
	
    CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.5f);

    // Make network privacy type requestorinfo identified by name
    CPosGSMPrivacyRequestInfo* reqInfo1 = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo1 );
	SetRequestorNameL(*reqInfo1, KTxtRequestorName);
	
	// issue a request
    TInt reqId = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo1, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();
	
	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	CLcfRequestor* req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorName)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;
	
	// check that result is conveyed back right
	if(iResult == KErrNone)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	    
	
	iPluginManager->SetNextVerificationAnswerL(ELcfReject, 0.5f);

	
	CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo2 );
    SetRequestorSIPURLL(*reqInfo2, KTxtRequestorSIPURL); 
	
	MakeVerificationL(*netPrivacy, *reqInfo2, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();

	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	req = diagnostics->RequestorLC(0);
	 
	 if(req->iIdString == KTxtRequestorSIPURL)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;
	
	
	// check that result is conveyed back right
	if(iResult == KErrAccessDenied)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	
	TBool amount = CheckQNAmounts(iPluginManager, 2, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	
	CleanupStack::PopAndDestroy(3);	
	
	}

void CTdPrivFW::TCPFW002L()
	{
    errorMessage.Zero();
	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 3.0f);
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestor infos
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);

    CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo2 );
	SetRequestorEmailL(*reqInfo2, KTxtRequestorEmail);

    CPosGSMPrivacyRequestInfo* reqInfo3 = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo3 );
	SetRequestorURLL(*reqInfo3, KTxtRequestorURL);

    //iLog->Put( _L("Step 1, cancel before UI is notified") );
   	INFO_PRINTF1(_L("Step 1, cancel before UI is notified"));

	// issue first verification request
    TInt id = KErrNone;
    MakeVerificationL(*netPrivacy, *reqInfo, id);
    iArray.Append(id);

	// issue second verification request (will be accepted by the network on timeout)
    TInt id2 = KErrNone;
    MakeVerificationL(*netPrivacy, *reqInfo2, id2, CPosNetworkPrivacy::EDecisionAccepted);

	// issue third verification request
    MakeVerificationL(*netPrivacy, *reqInfo3, id);
    iArray.Append(id);

	// cancel (ie 'timeout') the second one
	netPrivacy->CancelVerifyLocationRequest(id2, CPosNetworkPrivacy::ECancelReasonTimeout);
	errorMessage.Format( _L("Canceled request ID: %d"), id2);
   	INFO_PRINTF1(errorMessage);

	WaitForModuleEventsL();

	// allow UI to dismiss the dialog
	User::After(1000000);

	// we are expecting two verification (accept) requests and a notifications ('accept' due to verification timeout)
	TBool amount = CheckQNAmounts(iPluginManager, 2, 1);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(4);
	}
	
void CTdPrivFW::TCPFW003L()
	{
    errorMessage.Zero();
	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 15.0f);
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestor infos
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);

	TInt id;
	// issue request
	MakeVerificationL(*netPrivacy, *reqInfo, id);  /* be default last param is fCPosNetworkPrivacy::EDecisionRejected */	

	// wait for dialog ui-plugin to be notified
	User::After(500000);

	// cancel the request
	netPrivacy->CancelVerifyLocationRequest(id, CPosNetworkPrivacy::ECancelReasonTimeout);
	errorMessage.Format( _L("Canceled request ID: %d"), id);
   	INFO_PRINTF1(errorMessage);
	
	// wait for another second before checking results.
	User::After(1000000);

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetDiagnosticsL(0, diagnostics);
	CleanupStack::PushL(diagnostics);
	
	TLcfPluginExitReason reason = diagnostics->ExitReason();
	if (reason == ELcfExitCancelledByEPos)
		{
		errorMessage.Format( _L("Plug-in was notified about cancellation") );
   		INFO_PRINTF1(errorMessage);
		}
	else
		{
		errorMessage.Format( _L("Plug-in was not notified about cancellation") );
   		INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	// we are expecting 1 verification requests and a notification (rejected on timeout)
	TBool amount = CheckQNAmounts(iPluginManager, 1, 1);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(3);
	}

void CTdPrivFW::TCPFW004L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.1f);
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestor infos
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);

	// issue a verification request
    TInt id = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo, id);
	iArray.Append( id );

	WaitForModuleEventsL();
	
	// notify of differing network timeout decision for previously completed verification request
	netPrivacy->NotifyVerificationTimeoutL(*reqInfo, id, CPosNetworkPrivacy::EDecisionRejected);

	// allow UI to dismiss the dialog
	User::After(1000000);
	
	// we are expecting one request verification and one notification
	TBool amount = CheckQNAmounts(iPluginManager, 1, 1);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}
		
	CleanupStack::PopAndDestroy(2);	
	}



void CTdPrivFW::TCPFW005L()
	{	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    		
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorIMSL(*reqInfo, KTxtRequestorIMS);
	// issue a notification request with 'accept' as default timeout treatment
    TInt id = KErrNone;
	MakeNotificationL(*netPrivacy, *reqInfo, id);
	
	User::After(500000);

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	CLcfRequestor* req = diagnostics->RequestorLC(0);
	
	if(req->iIdString == KTxtRequestorIMS) 
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	// we are expecting no request verifications and one notification
	TBool amount = CheckQNAmounts(iPluginManager, 0, 1);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(4);	
	}


#if 0 //CDMA TEST not in scope
void CTdPrivFW::TCPFW006L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();

    CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 1.5f);

    // Make network privacy type requestorinfo identified by MIN
    CPosCDMAPrivacyRequestInfo* reqInfo = CreateCDMARequestInfoL();
    CleanupStack::PushL( reqInfo );

    SetRequestorMINL(*reqInfo, KTxtRequestorMIN); 
	
	// issue a request
    TInt reqId = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	CLcfRequestor* req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorMIN)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	
	
	CleanupStack::PopAndDestroy(2); // req, diagnostics;
	
	
	// check that result is conveyed back right
	if(iResult == KErrNone)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
		
    iPluginManager->SetNextVerificationAnswerL(ELcfReject, 0.5f);

	MakeVerificationL(*netPrivacy, *reqInfo, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();


	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorMIN)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;

	
	
	// check that result is conveyed back right
	if(iResult == KErrAccessDenied)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	TBool amount = CheckQNAmounts(iPluginManager, 2, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(2);	
	}


void CTdPrivFW::TCPFW007L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 3.0f);

    // Make network privacy type requestor infos
    CPosCDMAPrivacyRequestInfo* reqInfo = CreateCDMARequestInfoL();
    CleanupStack::PushL( reqInfo );

    
    SetRequestorMINL(*reqInfo, KTxtRequestorMIN);
    SetServiceMDNL(*reqInfo, KTxtRequestorMDN);
	
	
	TInt id;
	// issue first request
	MakeVerificationL(*netPrivacy, *reqInfo, id);	

	// wait for dialog ui-plugin to be notified
	User::After(500000);

	// cancel the first one
	netPrivacy->CancelVerifyLocationRequest(id, CPosNetworkPrivacy::ECancelReasonTimeout);
	errorMessage.Format( _L("Canceled request ID: %d"), id);
	INFO_PRINTF1(errorMessage);
	
	// wait for another second before checking results.
	User::After(1000000);

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	
	
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	
	
	TLcfPluginExitReason reason = diagnostics->ExitReason();
	if (reason == ELcfExitCancelledByEPos)
		{
		errorMessage.Format( _L("Plug-in was notified about cancellation") );
   		INFO_PRINTF1(errorMessage);
		}
	else
		{
		errorMessage.Format( _L("Plug-in was not notified about cancellation") );
   		INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	// we are expecting one verification requests and no notifications
	TBool amount = CheckQNAmounts(iPluginManager, 1, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	  CleanupStack::PopAndDestroy(5);
	
	}


void CTdPrivFW::TCPFW008L()
	{
    
	TInt err = 0;
	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestorinfos
    CPosCDMAPrivacyRequestInfo* reqInfo = CreateCDMARequestInfoL();
   	CleanupStack::PushL(reqInfo);

    SetRequestorMDNL(*reqInfo, KTxtRequestorMDN);
    
	// issue a notification request with 'accept' as default timeout treatment
    TInt id = KErrNone;
	MakeNotificationL(*netPrivacy, *reqInfo, id);
	
	User::After(500000);
	
	
	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	CLcfRequestor* req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorMDN)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;
	
	
	
	// we are expecting no request verifications and one notification
	TBool amount = CheckQNAmounts(iPluginManager, 0, 1);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(2);	
	}
#endif


void CTdPrivFW::TCPFW023L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();

    CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 1.5f);

    // Make network privacy type requestorinfo identified by name
    CPosSUPLPrivacyRequestInfo* reqInfo1 = CreateSUPLRequestInfoL();
    CleanupStack::PushL( reqInfo1 );

    SetRequestorNameL(*reqInfo1, KTxtRequestorName); 
	
	// issue a request
    TInt reqId = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo1, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();

	
	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	CLcfRequestor* req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorName)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;
	
	
	// check that result is conveyed back right
	if(iResult == KErrNone)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
		
    iPluginManager->SetNextVerificationAnswerL(ELcfReject, 0.5f);

	CPosSUPLPrivacyRequestInfo* reqInfo2 = CreateSUPLRequestInfoL();
    CleanupStack::PushL( reqInfo2 );

    SetRequestorPhoneL(*reqInfo2, KTxtRequestorPhone); 
	
	MakeVerificationL(*netPrivacy, *reqInfo2, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();

	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	req = diagnostics->RequestorLC(0);
	 
	if(req->iIdString == KTxtRequestorPhone)
		{
       	errorMessage.Format( _L("Requestor conveyed correctly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Requestor conveyed incorrectly to notifier plugin") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(2); // req, diagnostics;

	
	
	// check that result is conveyed back right
	if(iResult == KErrAccessDenied)
		{
       	errorMessage.Format( _L("Result returned correctly") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Result returned incorrectly") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	TBool amount = CheckQNAmounts(iPluginManager, 2, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(3);	
	}


void CTdPrivFW::TCPFW024L()
	{
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 3.0f);    // response from UI delayed for 3s, then 'accepted'
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 1.0f);    // response from UI delayed for 1s, then 'accepted'
    
    // Make network privacy type requestor infos
    CPosSUPLPrivacyRequestInfo* reqInfo = CreateSUPLRequestInfoL();
    CleanupStack::PushL( reqInfo );

    SetRequestorEmailL(*reqInfo, KTxtRequestorEmail);
    SetServiceURLL(*reqInfo, KTxtRequestorURL);
    
	TInt id;
	// issue first request
	MakeVerificationL(*netPrivacy, *reqInfo, id);	// rejected on timeout by default

	// wait for dialog ui-plugin to be notified
	User::After(500000);

	// cancel the verification with 'timeout' while it's displayed
	netPrivacy->CancelVerifyLocationRequest(id, CPosNetworkPrivacy::ECancelReasonTimeout);
	errorMessage.Format( _L("Canceled request ID: %d"), id);
	INFO_PRINTF1(errorMessage);
	
	// wait before checking results, to allow notification to be dismissed.
	User::After(2000000);

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetDiagnosticsL(0, diagnostics);
	CleanupStack::PushL(diagnostics);
	
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorEmail &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorURL ) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	TLcfPluginExitReason reason = diagnostics->ExitReason();
	if (reason == ELcfExitCancelledByEPos)
		{
		errorMessage.Format( _L("Plug-in was notified about cancellation") );
		INFO_PRINTF1(errorMessage);
		}
	else
		{
		errorMessage.Format( _L("Plug-in was not notified about cancellation") );
   		INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	// we are expecting one verification request and a notification (of verification rejected on timeout)
	TBool amount = CheckQNAmounts(iPluginManager, 1, 1);
	if(!amount)
		{
		errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(5);
	}

void CTdPrivFW::TCPFW025L()
	{	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestorinfos
    CPosSUPLPrivacyRequestInfo* reqInfo1 = CreateSUPLRequestInfoL();
   	CleanupStack::PushL(reqInfo1);

    SetRequestorSIPURLL(*reqInfo1, KTxtRequestorSIPURL);
    SetServiceIMSL(*reqInfo1, KTxtRequestorIMS);
    
	// issue a notification request with 'accept' as default timeout treatment
    TInt id = KErrNone;
	MakeNotificationL(*netPrivacy, *reqInfo1, id);
	
	User::After(500000);
	
	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	
    if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorIMS ) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(3); // req, diagnostics;
	
	// set the timings for notifiers
    iPluginManager->SetNextNotificationAnswerL(ELcfReject, 0.1f);

    // Make network privacy type requestorinfos
    CPosSUPLPrivacyRequestInfo* reqInfo2 = CreateSUPLRequestInfoL();
   	CleanupStack::PushL(reqInfo2);

    SetRequestorMINL(*reqInfo2, KTxtRequestorMIN);
    SetServiceMDNL(*reqInfo2, KTxtRequestorMDN);
    
	MakeNotificationL(*netPrivacy, *reqInfo2, id);
	
	User::After(500000);
	
	iPluginManager->GetLatestDiagnosticsL(diagnostics);
	CleanupStack::PushL(diagnostics);
	
    if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN ) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(3); // req, diagnostics;
	
	// we are expecting no request verifications and two notifications
	TBool amount = CheckQNAmounts(iPluginManager, 0, 2);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(3);	
	}
 
  




void CTdPrivFW::TCPFW009L()
{
    TInt err = KErrNone;

    // initialise plugin manager
	iPluginManager->ClearAllInputAndOutputL();
	
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.1f);

    // create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    TTimeIntervalMicroSeconds32 timeout(0);
    TTimeIntervalMicroSeconds32 defaultTimeOutTime(0);

    defaultTimeOutTime = netPrivacy->ConnectionTimeout();
	timeout = 10000000;

	TTimeIntervalMicroSeconds32 firstCheck(timeout.Int() - 500000);

	// make second check five seconds after the server should have shutdown
	TTimeIntervalMicroSeconds32 secondCheck(timeout.Int() + 5000000);
	
    errorMessage.Format( _L("Default timeout is %d"), defaultTimeOutTime.Int() );
    INFO_PRINTF1(errorMessage);

    // Make network privacy type request info
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);
	
    errorMessage.Format( _L("Setting timeout to %d milliseconds"), timeout.Int() );
    INFO_PRINTF1(errorMessage);
	netPrivacy->SetConnectionTimeout(timeout);
	        
    TInt id = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo, id);
	iArray.Append( id );
	
	// wait for verification to complete	
	WaitForModuleEventsL();
	
    errorMessage.Format( _L("Going to sleep for %d milliseconds."), firstCheck.Int() );
    INFO_PRINTF1(errorMessage);

	// wait a bit less than time out
	WaitForTimeL(firstCheck);

	// check that privacy server can be found
	_LIT(KServerName, "lbsnetgateway*");
	TFindProcess fp(KServerName);
	TFullName fullname;
	TBool serverFound(EFalse);

	while (err = fp.Next(fullname), err == KErrNone)
		{
		RProcess process;
		TInt processOpenErr = process.Open(fp);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// Found a running instance of the process,
				// so return without starting another instance.
				process.Close();
				serverFound = ETrue;
				}
			}
			process.Close();		
		}

	if(serverFound)
		{
			// great, a process was found
    		errorMessage.Zero();
    		errorMessage.Append( _L("A process by name ") );
    		errorMessage.Append( fullname );
    		errorMessage.Append( _L(" was found.") );
    		INFO_PRINTF1(errorMessage);
		}
		else
		{
		// we cannot have this, a process was not found
    	errorMessage.Format( _L("A privacy server process cannot be found."));
    	INFO_PRINTF1(errorMessage);
    	User::Leave(err);
		}

    errorMessage.Format( _L("Going to sleep for another %d milliseconds."),
    	secondCheck.Int() - firstCheck.Int());

    INFO_PRINTF1(errorMessage);

	// wait before second check
	WaitForTimeL( secondCheck.Int() - firstCheck.Int() + 500000 );

	// check that any process by that name cannot be found
	TFindProcess fp2(KServerName);
	TFullName fullname2;
	serverFound = EFalse;

	while (err = fp2.Next(fullname2), err == KErrNone)
		{
		RProcess process;
		TInt processOpenErr = process.Open(fp2);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// Found a running instance of the process,
				// so return without starting another instance.
				process.Close();
				serverFound = ETrue;
				}
			}
			process.Close();		
		}

	if(serverFound)
		{
			// we cannot have this, a process was found
    		errorMessage.Zero();
    		errorMessage.Append( _L("A process by name ") );
    		errorMessage.Append( fullname2 );
    		errorMessage.Append( _L(" was found.") );
    		INFO_PRINTF1(errorMessage);

			// return timeout to its original value
			netPrivacy->SetConnectionTimeout(defaultTimeOutTime);
    		User::Leave(KErrGeneral);
		}
		else
			{
			// right on, privacy server has shutdown
    		errorMessage.Format( _L("A privacy server process cannot be found."));
    		INFO_PRINTF1(errorMessage);
			}

	// check that server wakes up from sleep automatically
	MakeVerificationL(*netPrivacy, *reqInfo, id);
	iArray.Append( id );

	WaitForModuleEventsL();
		
	// return timeout to its original value
	netPrivacy->SetConnectionTimeout(defaultTimeOutTime);
	
	TInt amount = CheckQNAmounts(iPluginManager, 2, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

    CleanupStack::PopAndDestroy(2); // reqInfo, netPrivacy

}


void CTdPrivFW::TCPFW010L()
	{	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
	// for notifications, use 0.1 seconds
	iPluginManager->SetDefaultVerificationAnswerL(ELcfReject, 0.1f);
	TReal notTimingReal = 0.1f;
	
	iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, notTimingReal);
	
    // Make network privacy type requestorinfos
  	RPointerArray<CPosGSMPrivacyRequestInfo>* requestArray;
    requestArray = new (ELeave) RPointerArray<CPosGSMPrivacyRequestInfo>();
	CleanupStack::PushL(requestArray);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArray));
	
	for(TInt i=0; i<18; ++i)
		{
		requestArray->AppendL(CreateGSMRequestInfoL());
			
		}

	
	
	// set requestor identities for the first six ones
	SetRequestorNameL(*(*requestArray)[0], KTxtRequestorName);
	SetRequestorPhoneL(*(*requestArray)[1], KTxtRequestorPhone);
	SetRequestorURLL(*(*requestArray)[2], KTxtRequestorURL);
	SetRequestorEmailL(*(*requestArray)[3], KTxtRequestorEmail);
	

	SetRequestorSIPURLL(*(*requestArray)[4], KTxtRequestorSIPURL);
	SetRequestorIMSL(*(*requestArray)[5], KTxtRequestorIMS);
    

	// set LCS clients for the next six
	SetServiceEmailL(*(*requestArray)[6], KTxtRequestorEmail);
	SetServiceNameL(*(*requestArray)[7], KTxtRequestorName);
	SetServiceURLL(*(*requestArray)[8], KTxtRequestorURL);
	SetServicePhoneL(*(*requestArray)[9], KTxtRequestorPhone);
	
	
	SetServiceSIPURLL(*(*requestArray)[10], KTxtRequestorSIPURL);
	SetServiceIMSL(*(*requestArray)[11], KTxtRequestorIMS);
    

	// set both for the six last ones
	SetRequestorNameL(*(*requestArray)[12], KTxtRequestorName);
	SetServiceEmailL(*(*requestArray)[12], KTxtRequestorEmail);

	SetRequestorPhoneL(*(*requestArray)[13], KTxtRequestorPhone);
	SetServiceNameL(*(*requestArray)[13], KTxtRequestorName);

	SetRequestorSIPURLL(*(*requestArray)[14], KTxtRequestorSIPURL);
	SetServiceURLL(*(*requestArray)[14], KTxtRequestorURL);

	SetRequestorEmailL(*(*requestArray)[15], KTxtRequestorEmail);
	SetServicePhoneL(*(*requestArray)[15], KTxtRequestorPhone);

    
    SetRequestorSIPURLL(*(*requestArray)[16], KTxtRequestorSIPURL);
	SetServiceIMSL(*(*requestArray)[16], KTxtRequestorIMS);
	
	SetRequestorIMSL(*(*requestArray)[17], KTxtRequestorIMS);
	SetServiceSIPURLL(*(*requestArray)[17], KTxtRequestorSIPURL);
	
	
	
	
	TInt id;

    INFO_PRINTF1(errorMessage);
	
	for(TInt i=0; i<requestArray->Count(); ++i)
		{
			// create some variation to answers

			if(i % 2 == 1)
				{
				iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}

			MakeVerificationL(*netPrivacy, *(*requestArray)[i], id, CPosNetworkPrivacy::EDecisionAccepted);
			iArray.Append( id );			
		
		    WaitForModuleEventsL(); 
		 }
	



	// check that ids are correct
	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetDiagnosticsL(0, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(1, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(2, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(3, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics


	iPluginManager->GetDiagnosticsL(4, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
 
     	
    iPluginManager->GetDiagnosticsL(5, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   		
	
	
	// check that lcs client names are correct
	iPluginManager->GetDiagnosticsL(6, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrLcsIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(7, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrLcsIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(8, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrLcsIDIncorrectUrl );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(9, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrLcsIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

   
    iPluginManager->GetDiagnosticsL(10, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrLcsIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

    iPluginManager->GetDiagnosticsL(11, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrLcsIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   
   
   	// check that both identities are correct
	iPluginManager->GetDiagnosticsL(12, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorName &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorEmail) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(13, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorPhone &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorName) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(14, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(15, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorEmail &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorPhone) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(16, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorIMS) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

    iPluginManager->GetDiagnosticsL(17, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorIMS &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorSIPURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
    
	
		
	id = KErrNone;


	for(TInt i=0; i<requestArray->Count(); ++i)
		{
			MakeNotificationL(*netPrivacy, *(*requestArray)[i], id);
		}
		
	// wait one second longer than what it is required for notifications
	TInt waitTime = (requestArray->Count() * notTimingReal * 1000000) + 1000000;
	User::After(waitTime);

	// check that ids are correct
	iPluginManager->GetDiagnosticsL(18, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(19, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(20, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(21, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

    
    iPluginManager->GetDiagnosticsL(22, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(23, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	
	
	// check that lcs client names are correct
	iPluginManager->GetDiagnosticsL(24, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrLcsIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(25, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrLcsIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(26, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrLcsIDIncorrectUrl );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(27, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrLcsIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

    
    iPluginManager->GetDiagnosticsL(28, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrLcsIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(29, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrLcsIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, 
	
	
	
	// check that both identities are correct
	iPluginManager->GetDiagnosticsL(30, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorName &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorEmail) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(31, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorPhone &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorName) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(32, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(33, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorEmail &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorPhone) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

    
    iPluginManager->GetDiagnosticsL(34, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorIMS) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

    iPluginManager->GetDiagnosticsL(35, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorIMS &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorSIPURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
    
    

	// we are expecting 18 request verifications and 18 notifications
		
    TBool amount = CheckQNAmounts(iPluginManager, 18, 18); 
	
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}



#if 0 //CDMA not in scope of testing - Added GSM instead to avoid changing complete test

// Make network privacy type requestorinfos
  	RPointerArray<CPosCDMAPrivacyRequestInfo>* requestArrayCDMA;
    requestArrayCDMA = new (ELeave) RPointerArray<CPosCDMAPrivacyRequestInfo>();
	CleanupStack::PushL(requestArrayCDMA);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArrayCDMA));
	
	for(TInt i=0; i<6; ++i)
		{
		requestArrayCDMA->AppendL(CreateCDMARequestInfoL());
			
		}
    

	// set requestor identities for the first two ones
	
	SetRequestorMINL(*(*requestArrayCDMA)[0], KTxtRequestorMIN);
	SetRequestorMDNL(*(*requestArrayCDMA)[1], KTxtRequestorMDN);
    

	// set LCS clients for the next two
	SetServiceMINL(*(*requestArrayCDMA)[2], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArrayCDMA)[3], KTxtRequestorMDN);
	
	
	// set both for the two last ones
	
	SetRequestorMINL(*(*requestArrayCDMA)[4], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArrayCDMA)[4], KTxtRequestorMDN);
	
	SetRequestorMDNL(*(*requestArrayCDMA)[5], KTxtRequestorMDN);
	SetServiceMINL(*(*requestArrayCDMA)[5], KTxtRequestorMIN);
	
	
		

    INFO_PRINTF1(errorMessage);
	
	for(TInt i=0; i<requestArrayCDMA->Count(); ++i)
		{
			// create some variation to answers

			if(i % 2 == 1)
				{
				iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}

			MakeVerificationL(*netPrivacy, *(*requestArrayCDMA)[i], id, CPosNetworkPrivacy::EDecisionAccepted);
			iArray.Append( id );			
		
            WaitForModuleEventsL();  		
		
		}
#endif 	


// Make network privacy type requestorinfos
  	RPointerArray<CPosSUPLPrivacyRequestInfo>* requestArraySUPL1;
    requestArraySUPL1 = new (ELeave) RPointerArray<CPosSUPLPrivacyRequestInfo>();
	CleanupStack::PushL(requestArraySUPL1);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArraySUPL1));
	
	for(TInt i=0; i<6; ++i)
		{
		requestArraySUPL1->AppendL(CreateSUPLRequestInfoL());
			
		}
    

	// set requestor identities for the first two ones
	
	SetRequestorMINL(*(*requestArraySUPL1)[0], KTxtRequestorMIN);
	SetRequestorMDNL(*(*requestArraySUPL1)[1], KTxtRequestorMDN);
    

	// set LCS clients for the next two
	SetServiceMINL(*(*requestArraySUPL1)[2], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArraySUPL1)[3], KTxtRequestorMDN);
	
	
	// set both for the two last ones
	
	SetRequestorMINL(*(*requestArraySUPL1)[4], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArraySUPL1)[4], KTxtRequestorMDN);
	
	SetRequestorMDNL(*(*requestArraySUPL1)[5], KTxtRequestorMDN);
	SetServiceMINL(*(*requestArraySUPL1)[5], KTxtRequestorMIN);
	
	
		

    INFO_PRINTF1(errorMessage);
	
	for(TInt i=0; i<requestArraySUPL1->Count(); ++i)
		{
			// create some variation to answers

			if(i % 2 == 1)
				{
				iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}

			MakeVerificationL(*netPrivacy, *(*requestArraySUPL1)[i], id, CPosNetworkPrivacy::EDecisionAccepted);
			iArray.Append( id );			
		
            TRAPD(err, WaitForModuleEventsL());
            if (err != KErrNone)
            	{
            	User::Leave(err);
            	}
		}

//GSM  changes ends

	// check that ids are correct
	iPluginManager->GetDiagnosticsL(36, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(37, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(38, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(39, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics


    iPluginManager->GetDiagnosticsL(40, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics	


    iPluginManager->GetDiagnosticsL(41, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMDN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMIN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics	

   
    
	
		
	id = KErrNone;


	for(TInt i=0; i<requestArraySUPL1->Count(); ++i)
		{
			MakeNotificationL(*netPrivacy, *(*requestArraySUPL1)[i], id);
		}
		
	// wait one second longer than what it is required for notifications
	waitTime = (requestArraySUPL1->Count() * notTimingReal * 1000000) + 1000000;
	User::After(waitTime);

	// check that ids are correct
	
	iPluginManager->GetDiagnosticsL(42, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(43, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(44, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(45, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics


    iPluginManager->GetDiagnosticsL(46, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics	


    iPluginManager->GetDiagnosticsL(47, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMDN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMIN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics	

	
	
	
	
	// we are expecting 24 request verifications and 24 notification
	amount = CheckQNAmounts(iPluginManager, 24, 24);
	
   	
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}


	
	RPointerArray<CPosSUPLPrivacyRequestInfo>* requestArraySUPL;
    requestArraySUPL = new (ELeave) RPointerArray<CPosSUPLPrivacyRequestInfo>();
	CleanupStack::PushL(requestArraySUPL);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArraySUPL));
	
	for(TInt i=0; i<24; ++i)
		{
		requestArraySUPL->AppendL(CreateSUPLRequestInfoL());
			
		}

    
	// set requestor identities for the first eight ones
	SetRequestorNameL(*(*requestArraySUPL)[0], KTxtRequestorName);
	SetRequestorPhoneL(*(*requestArraySUPL)[1], KTxtRequestorPhone);
	SetRequestorURLL(*(*requestArraySUPL)[2], KTxtRequestorURL);
	SetRequestorEmailL(*(*requestArraySUPL)[3], KTxtRequestorEmail);
	SetRequestorSIPURLL(*(*requestArraySUPL)[4], KTxtRequestorSIPURL);
	SetRequestorIMSL(*(*requestArraySUPL)[5], KTxtRequestorIMS);
	SetRequestorMINL(*(*requestArraySUPL)[6], KTxtRequestorMIN);
	SetRequestorMDNL(*(*requestArraySUPL)[7], KTxtRequestorMDN);
    

	// set LCS clients for the next eight ones
	
    SetServiceNameL(*(*requestArraySUPL)[8], KTxtRequestorName);
	SetServicePhoneL(*(*requestArraySUPL)[9], KTxtRequestorPhone);
	SetServiceURLL(*(*requestArraySUPL)[10], KTxtRequestorURL);
	SetServiceEmailL(*(*requestArraySUPL)[11], KTxtRequestorEmail);
	SetServiceSIPURLL(*(*requestArraySUPL)[12], KTxtRequestorSIPURL);
	SetServiceIMSL(*(*requestArraySUPL)[13], KTxtRequestorIMS);
	SetServiceMINL(*(*requestArraySUPL)[14], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArraySUPL)[15], KTxtRequestorMDN);
     
	
     // set both for the last eight ones
	SetRequestorNameL(*(*requestArraySUPL)[16], KTxtRequestorName);
	SetServiceEmailL(*(*requestArraySUPL)[16], KTxtRequestorEmail);

	SetRequestorPhoneL(*(*requestArraySUPL)[17], KTxtRequestorPhone);
	SetServiceNameL(*(*requestArraySUPL)[17], KTxtRequestorName);

	SetRequestorURLL(*(*requestArraySUPL)[18], KTxtRequestorSIPURL);
	SetServiceURLL(*(*requestArraySUPL)[18], KTxtRequestorURL);

	SetRequestorEmailL(*(*requestArraySUPL)[19], KTxtRequestorEmail);
	SetServicePhoneL(*(*requestArraySUPL)[19], KTxtRequestorPhone);

    
    SetRequestorSIPURLL(*(*requestArraySUPL)[20], KTxtRequestorSIPURL);
	SetServiceIMSL(*(*requestArraySUPL)[20], KTxtRequestorIMS);
	
	SetRequestorIMSL(*(*requestArraySUPL)[21], KTxtRequestorIMS);
	SetServiceSIPURLL(*(*requestArraySUPL)[21], KTxtRequestorSIPURL);
	
	SetRequestorMINL(*(*requestArraySUPL)[22], KTxtRequestorMIN);
	SetServiceMDNL(*(*requestArraySUPL)[22], KTxtRequestorMDN);
	
	SetRequestorMDNL(*(*requestArraySUPL)[23], KTxtRequestorMDN);
	SetServiceMINL(*(*requestArraySUPL)[23], KTxtRequestorMIN);
	
	
	

    INFO_PRINTF1(errorMessage);
	
	for(TInt i=0; i<requestArraySUPL->Count(); ++i)
		{
			// create some variation to answers

			if(i % 2 == 1)
				{
				iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}

			MakeVerificationL(*netPrivacy, *(*requestArraySUPL)[i], id, CPosNetworkPrivacy::EDecisionAccepted);
			iArray.Append( id );			
		
            WaitForModuleEventsL(); 		
		}
	



	// check that ids are correct
	iPluginManager->GetDiagnosticsL(48, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(49, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(50, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(51, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(52, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
 
     	
    iPluginManager->GetDiagnosticsL(53, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(54, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(55, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   	
	
	
	
	// check that lcs client names are correct

    iPluginManager->GetDiagnosticsL(56, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(57, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(58, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(59, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(60, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
 
     	
    iPluginManager->GetDiagnosticsL(61, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(62, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(63, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   	
	
   
   	// check that both identities are correct
	iPluginManager->GetDiagnosticsL(64, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorName &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorEmail) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(65, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorPhone &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorName) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(66, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(67, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorEmail &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorPhone) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(68, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorIMS) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

    iPluginManager->GetDiagnosticsL(69, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorIMS &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorSIPURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
    iPluginManager->GetDiagnosticsL(70, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
   iPluginManager->GetDiagnosticsL(71, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMDN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMIN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
       
	
		
	id = KErrNone;


	for(TInt i=0; i<requestArraySUPL->Count(); ++i)
		{
			MakeNotificationL(*netPrivacy, *(*requestArraySUPL)[i], id);
		}
		
	// wait one second longer than what it is required for notifications
	waitTime = (requestArraySUPL->Count() * notTimingReal * 1000000) + 1000000;
	User::After(waitTime);

	// check that ids are correct

    iPluginManager->GetDiagnosticsL(72, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(73, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(74, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(75, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(76, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
 
     	
    iPluginManager->GetDiagnosticsL(77, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(78, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(79, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   	
	 
	
	
	// check that lcs client names are correct
	
	iPluginManager->GetDiagnosticsL(80, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)
		{
       	errorMessage.Format( KErrReqIDIncorrectName );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(81,diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorPhone)
		{
       	errorMessage.Format( KErrReqIDIncorrectPhone );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(82, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(83, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)
		{
       	errorMessage.Format( KErrReqIDIncorrectEmail );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(84, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorSIPURL)
		{
       	errorMessage.Format( KErrReqIDIncorrectSIPURL );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
 
     	
    iPluginManager->GetDiagnosticsL(85, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorIMS)
		{
       	errorMessage.Format( KErrReqIDIncorrectIMS );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(86, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMIN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMIN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
	
	iPluginManager->GetDiagnosticsL(87, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorMDN)
		{
       	errorMessage.Format( KErrReqIDIncorrectMDN );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(2); // requestor, diagonostics
   	
	
	// check that both identities are correct
	
	iPluginManager->GetDiagnosticsL(88, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorName &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorEmail) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(89, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorPhone &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorName) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(90, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	iPluginManager->GetDiagnosticsL(91, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorEmail &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorPhone) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

	
	iPluginManager->GetDiagnosticsL(92, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorSIPURL &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorIMS) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics

    iPluginManager->GetDiagnosticsL(93, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorIMS &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorSIPURL) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
    iPluginManager->GetDiagnosticsL(94, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMIN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMDN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
   iPluginManager->GetDiagnosticsL(95, diagnostics);	
	CleanupStack::PushL(diagnostics);
	if( !(diagnostics->RequestorLC(1)->iIdString == KTxtRequestorMDN &&
		diagnostics->RequestorLC(0)->iIdString == KTxtRequestorMIN) )
		{
       	errorMessage.Format( KErrLcsOrReqIdIncorrect );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(3); // requestor, diagonostics
   
       
	
	
	// we are expecting 48 request verifications and 48 notification
	amount = CheckQNAmounts(iPluginManager, 48, 48);
	
   	
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}
	

	 CleanupStack::PopAndDestroy(7);
	 //CleanupStack::PopAndDestroy(3);
	}


void CTdPrivFW::TCPFW011L()
	{
		
     // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfReject, 0.1f);
	iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);
	
    // Make network privacy type requestorinfos
    RPointerArray<CPosNetworkPrivacyRequestInfo>* requestArray;
    requestArray = new (ELeave) RPointerArray<CPosNetworkPrivacyRequestInfo>();
	CleanupStack::PushL(requestArray);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArray));
	
	CPosGSMPrivacyRequestInfo* info = NULL;

    requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorNameL(*info, KTxtRequestorName);

    requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorPhoneL(*info, KTxtRequestorPhone);

    requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorURLL(*info, KTxtRequestorURL);

    requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorEmailL(*info, KTxtRequestorEmail);
	
    //requestArray->AppendL(CreateCDMARequestInfoL());

	TInt id;
	TInt numRequests = 150;
	
	// requets in series
	for(TInt i=0; i< numRequests; ++i)
		{
			if(i % 2 == 1)
				{
					iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}
			MakeVerificationL(*netPrivacy, *(*requestArray)[i % requestArray->Count()], id);
			iArray.Append( id );
			WaitForModuleEventsL();
		}
	// variation for default answer
	iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.1f);

	// allow requests queue up in server
	iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 2.0f);
	
	// parallel requests
	for(TInt i=0; i< numRequests; ++i)
		{
			if(i % 2 == 1)
				{
					iPluginManager->SetNextVerificationAnswerL(ELcfReject, 0.1f);
				}
			MakeVerificationL(*netPrivacy, *(*requestArray)[i % requestArray->Count()], id);
			iArray.Append( id );
		}

	// wait for requests to compete
	WaitForModuleEventsL();
	
	// wait for 150 notifications to complete plus half a second extra
	User::After(numRequests*100000 + 500000);
	
	// requets in series
	for(TInt i=0; i< numRequests; ++i)
		{
			if(i % 2 == 1)
				{
					iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 0.1f);
				}
			MakeNotificationL(*netPrivacy, *(*requestArray)[i % requestArray->Count()], id);

			// wait for notification to complete plus 0.05 seconds extra
			User::After(150000);
		}
	// variation for default answer
	iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.1f);

	// allow requests queue up in server
	iPluginManager->SetNextVerificationAnswerL(ELcfAccept, 2.0f);
	
	// parallel requests
	for(TInt i=0; i< numRequests; ++i)
		{
			if(i % 2 == 1)
				{
					iPluginManager->SetNextVerificationAnswerL(ELcfReject, 0.1f);
				}
			MakeNotificationL(*netPrivacy, *(*requestArray)[i % requestArray->Count()], id);
			
		}

	// wait for 150 notifications plus half a second extra
	User::After(numRequests * 150000 + 500000);

	// we are expecting no request verifications and one notification

	TBool amount = CheckQNAmounts(iPluginManager, (2 * numRequests), (2 * numRequests));
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(3);	
	
	
	} 
	

void CTdPrivFW::TCPFW012L()
	{
		
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfReject, 0.1f);
	iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.1f);

    // Make network privacy type requestorinfo identified by name
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);
	
	// issue a request
    TInt reqId = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo, reqId);
	iArray.Append( reqId );
	WaitForModuleEventsL();

	CLcfPluginDiagnostics* diagnostics = NULL;
	iPluginManager->GetLatestDiagnosticsL(diagnostics);	
	CleanupStack::PushL(diagnostics);
	TPosRequestSource source = diagnostics->RequestorLC(0)->iSource;
	
	if (source != EPosRequestSourceNetwork)
		{
       	errorMessage.Format( _L("Request source differs from network") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
		
	CleanupStack::PopAndDestroy(4); // requestor, diagonostics, reqInfo, netPrivacy

	}

void CTdPrivFW::TCPFW013L()
	{
	//_LIT(KTxtSettingLength, "Setting service name with a legal string, length: %d");
	TBuf<300> buf;
	TInt err = KErrNone;
	TInt id;
	CPosGSMPrivacyRequestInfo* info;
	
	for(TInt i=0; i<300; ++i)
		{
			buf.Append( _L("A") );
		}
	
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfReject, 0.5f);
	iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 0.5f);
	
    // Make network privacy type requestorinfos
  	RPointerArray<CPosGSMPrivacyRequestInfo>* requestArray;
    requestArray = new (ELeave) RPointerArray<CPosGSMPrivacyRequestInfo>();
	CleanupStack::PushL(requestArray);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy, requestArray));

	// legal values
	buf.SetLength(255);
	requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorNameL(*info, buf);

	requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetServiceNameL(*info, buf);

	// illegal values
	buf.SetLength(256);
	requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetRequestorNameL(*info, buf);

	requestArray->AppendL( info = CreateGSMRequestInfoL() );
	SetServiceNameL(*info, buf);

	// these should not leave
	MakeVerificationL(*netPrivacy, *(*requestArray)[0], id);
	iArray.Append (id);
	MakeVerificationL(*netPrivacy, *(*requestArray)[1], id);
	iArray.Append (id);

	WaitForModuleEventsL();
	
	// these should leave
     errorMessage.Format( _L("We are expecting the next one to fail.") );
     INFO_PRINTF1(errorMessage);
	
	TRAP(err, MakeVerificationL(*netPrivacy, *(*requestArray)[2], id));
	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Leave should have occurred.") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	err = KErrNone;

    errorMessage.Format( _L("We are expecting the next one to fail.") );
    INFO_PRINTF1(errorMessage);
	TRAP(err, MakeVerificationL(*netPrivacy, *(*requestArray)[3], id));
	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Leave should have occurred.") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
		
	TBool amount = CheckQNAmounts(iPluginManager, 2, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(3);	
	
	}

#if 0	//We would not test Privacy Server so this test is not applicable
void CTdPrivFW::TCPFW014L()
	{

	//We would not test Privacy Server so this test is not applicable
  	errorMessage.Format( _L("Test Not valid for Symbian - Uses Nokia specif Privacy Server") );
   	INFO_PRINTF1(errorMessage);
	
	// Connect to RPrivacyAPI.
	TPosRequestSource source = EPosRequestSourceNotAvailable;
	RPosRequestorStack stack;
	TRequestStatus status;
	TInt id = KErrNone;	

   	RPrivacyServer server;
    CleanupClosePushL(server);

	// we should be able to connect
	TInt err = server.Connect(source);
	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Connection established through Privacy API") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Could not establish connection through Privacy API, code: %d"), err );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(err);
		}
	User::After(500000);

	CleanupStack::PopAndDestroy(); // server
	}

void CTdPrivFW::TCPFW015L()
	{
	//We would not test Privacy Server so this test is not applicable
  	errorMessage.Format( _L("Test Not valid for Symbian - Uses Nokia specif Privacy Server") );
   	INFO_PRINTF1(errorMessage);

	// Connect to RPrivacyAPI.
	TPosRequestSource source = EPosRequestSourceNotAvailable;
	RPosRequestorStack stack;
	TRequestStatus status;
	TInt id = KErrNone;	

   	RPrivacyServer server;
    CleanupClosePushL(server);

	// we should not be able to connect
	TInt err = server.Connect(source);
	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Connection established through Privacy API") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Could not establish connection through Privacy API, code: %d"), err );
       	INFO_PRINTF1(errorMessage);
		}
	User::After(500000);

	CleanupStack::PopAndDestroy(); // server
	}
#endif	

void CTdPrivFW::TCPFW017L()
	{
	TInt err = KErrNone;
	
    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();

    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 10.5f);

	// Connect to Network Privacy API.
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = NULL;
    netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL(netPrivacy);

    // Make GSM privacy type requestorinfo identified by name
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );

	TInt id = KErrNone;
	
	// we should be able to do verification
	TRAP(err, MakeVerificationL(*netPrivacy, *reqInfo, id));

	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Verification successful through Network Privacy API") );
       	INFO_PRINTF1(errorMessage);
		}
	else
		{
       	errorMessage.Format( _L("Verification unsuccessful through Network Privacy API, code: %d"), err );
       	INFO_PRINTF1(errorMessage);
       	User::Leave( err );
		}
	User::After(500000);

	CleanupStack::PopAndDestroy(2); // reqInfo, netPrivacy
	}

void CTdPrivFW::TCPFW018L()
	{
	TInt err = KErrNone;
	
    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();

    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.5f);

	// Connect to Network Privacy API.
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = NULL;
    netPrivacy = ConnectToNetworkPrivacyAPIL();
	CleanupStack::PushL(netPrivacy);

    // Make CDMA privacy type requestorinfo identified by name
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );

	TInt id = KErrNone;
	
	// this should leave
	TRAP(err, MakeVerificationL(*netPrivacy, *reqInfo, id));
	// we should not be able to connect
	if(err == KErrNone)
		{
       	errorMessage.Format( _L("Verification successful through Network Privacy API") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification unsuccessful through Network Privacy API, code: %d"), err );
       	INFO_PRINTF1(errorMessage);
		}

	TBool amount = CheckQNAmounts(iPluginManager, 0, 0);
	if(!amount)
		{
       	errorMessage.Format( _L("Verification and notification amounts incorrect") );
       	INFO_PRINTF1(errorMessage);
		User::Leave(KErrGeneral);
		}
	else
		{
       	errorMessage.Format( _L("Verification and notification amounts as expected") );
       	INFO_PRINTF1(errorMessage);
		}

	CleanupStack::PopAndDestroy(2); // reqInfo, netPrivacy
	}

void CTdPrivFW::TCPFW020L()
	{
	TInt err = KErrNone;

    // Create managing instance to control notifier
	iPluginManager->ClearAllInputAndOutputL();

    // set the timings for notifiers
	iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 0.5f);
	
	// Connect to Network Privacy API.
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = NULL;
    TRAP(err, netPrivacy = ConnectToNetworkPrivacyAPIL())
	CleanupStack::PushL(netPrivacy);

    // Make GSM privacy type requestorinfo identified by name
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
	SetRequestorNameL(*reqInfo, KTxtRequestorName);

	TInt id = KErrNone;
	MakeVerificationL(*netPrivacy, *reqInfo, id);
	iArray.Append( id );	
	WaitForModuleEventsL();
	
	// should be rejected
	if (iResult != KErrAccessDenied)
		{
       	errorMessage.Format( _L("Request was not rejected") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}
		
    // Make SUPL privacy type requestorinfo identified by name
    CPosSUPLPrivacyRequestInfo* reqInfo2 = CreateSUPLRequestInfoL();
    CleanupStack::PushL( reqInfo2 );

	MakeVerificationL(*netPrivacy, *reqInfo2, id);
	iArray.Append( id );	
	WaitForModuleEventsL();
	
	// should be rejected
	if (iResult != KErrAccessDenied)
		{
       	errorMessage.Format( _L("Request was not rejected") );
       	INFO_PRINTF1(errorMessage);
       	User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(3); // reqInfo2, reqinfo, netPrivacy
	}

#if 0
void CTdPrivFW::TCPFW021L()
	{
	//We would not test Privacy Server so this test is not applicable
  	errorMessage.Format( _L("Test Not valid for Symbian - Uses Nokia specif Privacy Server") );
   	INFO_PRINTF1(errorMessage);

	TInt err = KErrNone;
	RTdPrivacyServer handle;

   	errorMessage.Format( _L("Created a handle to privacy server") );
   	INFO_PRINTF1(errorMessage);

	CleanupClosePushL(handle);
	err = handle.Connect();
	if (err == KErrNone)
		{
   		errorMessage.Format( _L("Connected to privacy server") );
   		INFO_PRINTF1(errorMessage);

	   	errorMessage.Format( _L("Sending crap") );
   		INFO_PRINTF1(errorMessage);
   		
   		errorMessage.Format( _L("Sending out message id %d"), 0 );
   		INFO_PRINTF1(errorMessage);
		TInt err = handle.SendCrap(0);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }

   		errorMessage.Format( _L("Sending out message id %d"), 2 );
   		INFO_PRINTF1(errorMessage);
		err = handle.SendCrap(2);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }

   		errorMessage.Format( _L("Sending out message id %d"), 3 );
   		INFO_PRINTF1(errorMessage);
		err = handle.SendCrap(3);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }

   		errorMessage.Format( _L("Sending out message id %d"), 5 );
   		INFO_PRINTF1(errorMessage);
		err = handle.SendCrap(5);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }

   		errorMessage.Format( _L("Sending out message id %d"), 4 );
   		INFO_PRINTF1(errorMessage);
		err = handle.SendCrap(6);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }

   		errorMessage.Format( _L("Sending out message id %d"), 1 );
   		INFO_PRINTF1(errorMessage);
		err = handle.SendCrap(1);
		if(err == KErrNone)
		    {
   		    errorMessage.Format( _L("Did not cause a leave"));
   		    INFO_PRINTF1(errorMessage);
		    User::Leave(KErrGeneral);
		    }
	    else
	        {
   		    errorMessage.Format( _L("Right on, received error code: %d"), err);
   		    INFO_PRINTF1(errorMessage);
	        }
		}
	else
		{
   		errorMessage.Format( _L("Unable to connect to privacy server") );
   		INFO_PRINTF1(errorMessage);
		}

	
	CleanupStack::PopAndDestroy();
	}
#endif

void CTdPrivFW::TCPFW022L()
	{
	// Stress-test: Keep running TCPFW011L
	// for 2.5 hours, with a delay between each run.
	
	TTime startTime;
	TTime endTime;
	TTimeIntervalMinutes duration;
	const TTimeIntervalMinutes KRequiredDuration(150);
	startTime.UniversalTime();
	
	do 
		{
       	errorMessage.Format( _L("ROUND STARTS") );
       	INFO_PRINTF1(errorMessage);
		TCPFW011L();
       	errorMessage.Format( _L("ROUND ENDS") );
       	INFO_PRINTF1(errorMessage);
       	
       	// Delay of 2s before next test run.
       	WaitForTimeL(2000000);
		
		// Check if 2.5 hours has elapsed.
		endTime.UniversalTime();
		endTime.MinutesFrom(startTime, duration);
		} 
	while (duration < KRequiredDuration);
	}

void CTdPrivFW::TCPFW026L()
    {
    errorMessage.Zero();
    
    // Create network privacy instance
    CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
    CleanupStack::PushL( netPrivacy );

    // initialise notifier plugin manager
    iPluginManager->ClearAllInputAndOutputL();
    
    // set the timings for notifiers
    iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 2.0f);    // accepted by UI after 2s 
    iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 3.0f);    // 3s response time

    // Make network privacy type requestor infos
    CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo );
    SetRequestorNameL(*reqInfo, KTxtRequestorName);

    CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
    CleanupStack::PushL( reqInfo2 );
    SetRequestorEmailL(*reqInfo2, KTxtRequestorEmail);

    // issue notification
    INFO_PRINTF1(_L("Step 1, issue notification"));
    TInt id = KErrNone;
    // issue a notification request
    MakeNotificationL(*netPrivacy, *reqInfo, id);
    
    // issue a verification request
    INFO_PRINTF1(_L("Step 2, issue verification (while notification still outstanding)"));
    TInt id2 = KErrNone;
    MakeVerificationL(*netPrivacy, *reqInfo2, id2, CPosNetworkPrivacy::EDecisionAccepted);
    iArray.Append(id2);
    
    WaitForModuleEventsL();

    // allow UI to dismiss the dialog
    User::After(3000000);

    // we are expecting one verification (accept) requests and 2 notifications ('cancelled by epos', 'accept')
    if(!CheckQNAmounts(iPluginManager, 1, 2))
        {
        errorMessage.Format( _L("Verification + notification total incorrect") );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);
        }
    else
        {
        errorMessage.Format( _L("Verification + notification total as expected") );
        INFO_PRINTF1(errorMessage);
        }

    // check that details are correct - notification (cancel) --> verification (accept) --> notification (accept)
    CLcfPluginDiagnostics* diagnostics = NULL;
    iPluginManager->GetDiagnosticsL(0, diagnostics);    
    CleanupStack::PushL(diagnostics);
    if(diagnostics->VerificationRequest())
        {
        errorMessage.Format( KErrExpectedNotification );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);
        }
    if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
        {
        errorMessage.Format( KErrWrongExitReason );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);         
        }     
    CleanupStack::PopAndDestroy(); // diagnostics
    
    iPluginManager->GetDiagnosticsL(1, diagnostics);    
    CleanupStack::PushL(diagnostics);
    if(!diagnostics->VerificationRequest())
        {
        errorMessage.Format( KErrExpectedVerification );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);
        }
    if(diagnostics->ExitReason() != ELcfExitAccepted)
        {
        errorMessage.Format( KErrWrongExitReason );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);         
        }     
    CleanupStack::PopAndDestroy(); // diagnostics
    
    iPluginManager->GetDiagnosticsL(2, diagnostics);    
    CleanupStack::PushL(diagnostics);
    if(diagnostics->VerificationRequest())
        {
        errorMessage.Format( KErrExpectedNotification );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);
        }
    if(diagnostics->ExitReason() != ELcfExitAccepted)
        {
        errorMessage.Format( KErrWrongExitReason );
        INFO_PRINTF1(errorMessage);
        User::Leave(KErrGeneral);         
        }     
    CleanupStack::PopAndDestroy(); // diagnostics
    
    CleanupStack::PopAndDestroy(3); // 
    }

void CTdPrivFW::TCPFW027L()
    {
    errorMessage.Zero();
     
     // Create network privacy instance
     CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
     CleanupStack::PushL( netPrivacy );

     // initialise notifier plugin manager
     iPluginManager->ClearAllInputAndOutputL();
     
     // set the timings for notifiers
     iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 2.0f);    // rejected by UI after 2s
     iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 3.0f);    // accepted by UI after 3s 
     iPluginManager->SetNotificationAnswerL(1, ELcfAccept, 0.1f);
     iPluginManager->SetNotificationAnswerL(2, ELcfAccept, 0.1f);
     iPluginManager->SetNotificationAnswerL(3, ELcfAccept, 0.1f);
     
     // Make network privacy type requestor infos
     CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo );
     SetRequestorNameL(*reqInfo, KTxtRequestorName);

     CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo2 );
     SetRequestorEmailL(*reqInfo2, KTxtRequestorEmail);

     CPosGSMPrivacyRequestInfo* reqInfo3 = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo3 );
     SetRequestorEmailL(*reqInfo3, KTxtRequestorPhone);
     
     CPosGSMPrivacyRequestInfo* reqInfo4 = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo4 );
     SetRequestorEmailL(*reqInfo4, KTxtRequestorURL);
     
     // issue 3 notifications
     INFO_PRINTF1(_L("Step 1, issue 3 notifications"));
     TInt id = KErrNone;
     // issue a notification request
     MakeNotificationL(*netPrivacy, *reqInfo2, id);
     MakeNotificationL(*netPrivacy, *reqInfo3, id);
     MakeNotificationL(*netPrivacy, *reqInfo4, id);
     
     // issue a verification request
     INFO_PRINTF1(_L("Step 2, issue verification (while notifications still outstanding)"));
     MakeVerificationL(*netPrivacy, *reqInfo, id, CPosNetworkPrivacy::EDecisionAccepted);
     iArray.Append(id);
     
     WaitForModuleEventsL();

     // allow UI to dismiss all the dialogs so we get diagnostic
     User::After(4000000);

     // we are expecting one verification (reject) requests and 4 notifications ('cancelled by epos', 3 * 'accept')
     if(!CheckQNAmounts(iPluginManager, 1, 4))
         {
         errorMessage.Format( _L("Verification + notification total incorrect") );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     else
         {
         errorMessage.Format( _L("Verification + notification total as expected") );
         INFO_PRINTF1(errorMessage);
         }

     // check that details are correct - notification (cancel) --> verification (accept) --> notification (accept)
     CLcfPluginDiagnostics* diagnostics = NULL;
     iPluginManager->GetDiagnosticsL(0, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(1, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(!diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedVerification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitAccepted)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }     
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(2, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitAccepted)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }   
     if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)// make sure it's the first notification that got cancelled
         {
         errorMessage.Format( KErrReqIDIncorrectEmail );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     CleanupStack::PopAndDestroy(2); // diagnostics + requestor
     
     CleanupStack::PopAndDestroy(5); //  network privacy instance + requestors   
    }

void CTdPrivFW::TCPFW028L()
    {
    errorMessage.Zero();
     
     // Create network privacy instance
     CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
     CleanupStack::PushL( netPrivacy );

     // initialise notifier plugin manager
     iPluginManager->ClearAllInputAndOutputL();
     
     // set the timings for notifiers
     iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 20.0f);    // UI won't respond for 20s
     iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 2.0f);    // no response before 2s to allow time for verification to take priority 

     // Make network privacy type requestor infos
     CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo );
     SetRequestorNameL(*reqInfo, KTxtRequestorName);

     CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo2 );
     SetRequestorEmailL(*reqInfo2, KTxtRequestorEmail);
     
     // issue a notification
     INFO_PRINTF1(_L("Step 1, issue a notification"));
     TInt id = KErrNone;
     // issue a notification request
     MakeNotificationL(*netPrivacy, *reqInfo, id);
 
     // wait for the notification dialog to be displayed 
     User::After(500000);
     
     // issue a verification request that will be accepted by network on timeout
     INFO_PRINTF1(_L("Step 2, issue verification that will be accepted by network on timeout"));
     MakeVerificationL(*netPrivacy, *reqInfo2, id, CPosNetworkPrivacy::EDecisionAccepted);
     iArray.Append(id);
     
     // allow time for verification dialog to be displayed
     User::After(2000000);
     
     // cancel (ie 'timeout') the Verification at the network side
      netPrivacy->CancelVerifyLocationRequest(id, CPosNetworkPrivacy::ECancelReasonTimeout);
      errorMessage.Format( _L("Canceled request ID: %d"), id);
      INFO_PRINTF1(errorMessage);

     // allow UI to dismiss both notification dialogs so we get diagnostics
     User::After(5000000);

     // we are expecting a notification (cancelled by epos), then a verification (canceled by epos), 
     // then a notification (accept) for the timed out verification then a notification (accept) for the original notification which was deferred
     if(!CheckQNAmounts(iPluginManager, 1, 3))
         {
         errorMessage.Format( _L("Verification + notification total incorrect") );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     else
         {
         errorMessage.Format( _L("Verification + notification total as expected") );
         INFO_PRINTF1(errorMessage);
         }

     // check that details are correct - notification (cancelled) --> verification (cancelled) --> notification (accept) --> notification (accept)
     CLcfPluginDiagnostics* diagnostics = NULL;
     iPluginManager->GetDiagnosticsL(0, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(1, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(!diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedVerification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }     
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(2, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitAccepted)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }   
     if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)// make sure it's the notification for the cancelled verification
         {
         errorMessage.Format( KErrReqIDIncorrectEmail );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     CleanupStack::PopAndDestroy(2); // diagnostics + requestor
     
     iPluginManager->GetDiagnosticsL(3, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitAccepted)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }   
     if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)// make sure it's the one for the original notification 
         {
         errorMessage.Format( KErrReqIDIncorrectEmail );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     CleanupStack::PopAndDestroy(2); // diagnostics + requestor     
     
     CleanupStack::PopAndDestroy(3); //  network privacy instance + requestors   
    }


void CTdPrivFW::TCPFW029L()
    {
    errorMessage.Zero();
     
     // Create network privacy instance
     CPosNetworkPrivacy* netPrivacy = ConnectToNetworkPrivacyAPIL();
     CleanupStack::PushL( netPrivacy );

     // initialise notifier plugin manager
     iPluginManager->ClearAllInputAndOutputL();
     
     // set the timings for notifiers
     iPluginManager->SetDefaultVerificationAnswerL(ELcfAccept, 20.0f);    // UI won't respond for 20s
     iPluginManager->SetDefaultNotificationAnswerL(ELcfAccept, 2.0f);    // no response before 2s to allow time for verification to take priority 

     // Make network privacy type requestor infos
     CPosGSMPrivacyRequestInfo* reqInfo = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo );
     SetRequestorNameL(*reqInfo, KTxtRequestorName);

     CPosGSMPrivacyRequestInfo* reqInfo2 = CreateGSMRequestInfoL();
     CleanupStack::PushL( reqInfo2 );
     SetRequestorEmailL(*reqInfo2, KTxtRequestorEmail);
     
     // issue a notification
     INFO_PRINTF1(_L("Step 1, issue a notification"));
     TInt id = KErrNone;
     // issue a notification request
     MakeNotificationL(*netPrivacy, *reqInfo, id);
 
     // wait for the notification dialog to be displayed 
     User::After(500000);
     
     // issue a verification request that will be rejected by network on timeout
     INFO_PRINTF1(_L("Step 2, issue verification (while notification outstanding) that will be rejected on timeout"));
     MakeVerificationL(*netPrivacy, *reqInfo2, id, CPosNetworkPrivacy::EDecisionRejected);
     iArray.Append(id);
     
     // allow time for verification dialog to be displayed
     User::After(2000000);
     
     // cancel (ie 'timeout') the Verification at the network side
      netPrivacy->CancelVerifyLocationRequest(id, CPosNetworkPrivacy::ECancelReasonTimeout);
      errorMessage.Format( _L("Canceled request ID: %d"), id);
      INFO_PRINTF1(errorMessage);
      
     // WaitForModuleEventsL(); there aren't any since the only verification is cancelled

     // allow UI to dismiss the dialogs so we get diagnostic
     User::After(6000000);

     // we are expecting a notification (cancelled by epos), then a verification (canceled by epos), 
     // then a notification (rejected) for the verification that was rejected on timeout then a notification (accepted) for the original notification which was deferred
     if(!CheckQNAmounts(iPluginManager, 1, 3))
         {
         errorMessage.Format( _L("Verification + notification total incorrect") );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     else
         {
         errorMessage.Format( _L("Verification + notification total as expected") );
         INFO_PRINTF1(errorMessage);
         }

     // check that details are correct - notification (cancelled) --> verification (cancelled) --> notification (accept) --> notification (accept)
     CLcfPluginDiagnostics* diagnostics = NULL;
     iPluginManager->GetDiagnosticsL(0, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(1, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(!diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedVerification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     if(diagnostics->ExitReason() != ELcfExitCancelledByEPos)
         {
         errorMessage.Format( KErrWrongExitReason );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);         
         }     
     CleanupStack::PopAndDestroy(); // diagnostics
     
     iPluginManager->GetDiagnosticsL(2, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }   
     if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorEmail)// make sure it's the one for the timedout verification 
         {
         errorMessage.Format( KErrReqIDIncorrectEmail );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     CleanupStack::PopAndDestroy(2); // diagnostics + requestor     
     
     iPluginManager->GetDiagnosticsL(3, diagnostics);    
     CleanupStack::PushL(diagnostics);
     if(diagnostics->VerificationRequest())
         {
         errorMessage.Format( KErrExpectedNotification );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }   
     if( diagnostics->RequestorLC(0)->iIdString != KTxtRequestorName)// make sure it's the one for the original notification 
         {
         errorMessage.Format( KErrReqIDIncorrectEmail );
         INFO_PRINTF1(errorMessage);
         User::Leave(KErrGeneral);
         }
     CleanupStack::PopAndDestroy(2); // diagnostics + requestor 
     
     CleanupStack::PopAndDestroy(3); //  network privacy instance + requestors   
    }

