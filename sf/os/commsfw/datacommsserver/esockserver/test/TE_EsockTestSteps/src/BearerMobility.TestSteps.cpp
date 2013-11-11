// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file BearerMobility.TestSteps.cpp
*/

#include "Connections.TestSteps.h"
#include "BearerMobility.TestSteps.h"


using namespace ESock;

// Accept
//-------

Cacceptcarrierrcommsmobilityapiext::Cacceptcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Kacceptcarrierrcommsmobilityapiext);
	}

TInt Cacceptcarrierrcommsmobilityapiext::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }


TVerdict Cacceptcarrierrcommsmobilityapiext::doSingleTestStep()
	{
	RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }
    mob->NewCarrierAccepted();
    
	return EPass;
	}

// Reject
//-------
Crejectcarrierrcommsmobilityapiext::Crejectcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Krejectcarrierrcommsmobilityapiext);
	}

TInt Crejectcarrierrcommsmobilityapiext::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }

TVerdict Crejectcarrierrcommsmobilityapiext::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }   
   	mob->NewCarrierRejected();

	return EPass;
	}


// Cancel notification
//--------------------

CCancelMobilityNotificationStep::CCancelMobilityNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCancelMobilityNotificationStep);
	}

TInt CCancelMobilityNotificationStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }


TVerdict CCancelMobilityNotificationStep::doSingleTestStep()
	{
	SetTestStepResult(EFail);
	RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
	if (mob==NULL)
		{
		INFO_PRINTF1(_L("Could not find mobility extension."));
		}
	else
		{
		mob->CancelMobilityNotification();
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}


// Migrate
//--------

Cmigratercommsmobilityapiext::Cmigratercommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Kmigratercommsmobilityapiext);
	}

TInt Cmigratercommsmobilityapiext::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }


TVerdict Cmigratercommsmobilityapiext::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }

 	mob->MigrateToPreferredCarrier();

	return EPass;
	}


// Ignore
//-------

Cignorercommsmobilityapiext::Cignorercommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Kignorercommsmobilityapiext);
	}

TInt Cignorercommsmobilityapiext::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
    }


TVerdict Cignorercommsmobilityapiext::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }

	mob->IgnorePreferredCarrier();

	return EPass;
	}


// Check
//------
CCheckMobilityNotificationStep::CCheckMobilityNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
	SetTestStepName(KCheckMobilityNotificationStep);
	}

TInt CCheckMobilityNotificationStep::ConfigureFromIni()
	{
	iParams.Reset();
	iOldAP = 0;
	iNewAP = 0;
	iExpectedError = KExpectedErrorNotUsed;
	iExpectedError2 = KExpectedErrorNotUsed;

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_MobilityAPIName, iParams.iMobilityExtName) != 1)
		|| (iParams.iMobilityExtName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

	//calculate name for the event container (it is not settable from the ini)
	iEventName = iParams.iMobilityExtName;
	iEventName.Append(_L("_EvContainer"));
	iParams.iEventName.Set(iEventName);

	// Get any timeout value and if we have one then we are not expecting any notification
	if(!GetIntFromConfig(iSection, KTimeoutInMilliSeconds, iParams.iTimeoutMiliSecs))
		{
		// Default to 5 second timeout
		iParams.iTimeoutMiliSecs = 7000; //Seven for luck :)) and really to be shorter than the idle timer (10)
		}

	//there may be more than one expected event kind, register all of them
	TBool descriptionComplete = EFalse;

	for (TInt idx = 0; !descriptionComplete; idx++)
		{
		TBuf<KMaxTestExecuteNameLength> fieldName(KTe_Expected);
		fieldName.AppendNum(idx);

		TPtrC eventName;
		if (GetStringFromConfig(iSection,fieldName,eventName)==1)
			{
			TBuf<KMaxTestExecuteNameLength> fieldNameAp(fieldName);
			fieldNameAp.Append(KTe_MobilityNotificationPreferredCarrierAvailable_NewAP);
			if (eventName.Compare(KTe_MobilityNotificationPreferredCarrierAvailable)==0)
				{
				iParams.iEventMask |= ESock::EPreferredCarrierAvailable;
				GetIntFromConfig(iSection,fieldNameAp,iNewAP);

				fieldNameAp.Copy(fieldName);
				fieldNameAp.Append(KTe_MobilityNotificationPreferredCarrierAvailable_OldAP);
				GetIntFromConfig(iSection,fieldNameAp,iOldAP);
				}
			else if (eventName.Compare(KTe_MobilityNotificationNewCarrierActive)==0)
				{
				iParams.iEventMask |= ESock::ENewCarrierActive;
				GetIntFromConfig(iSection,fieldNameAp,iNewAP);
				}
			else
			   {
			   INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iMobilityExtName,&eventName);
			   return KErrNotFound;
			   }
			}
		else
			{
			descriptionComplete = ETrue;
			}
		}

	// Log what events we are expecting if any
	if(iParams.iEventMask)
		{
		INFO_PRINTF3(_L("%S: Events to be expected: 0x%x"),&iParams.iMobilityExtName,iParams.iEventMask);
		}
	else
		{
		INFO_PRINTF2(_L("%S: No events specified so expecting no notifications and to timeout instead."), &iParams.iMobilityExtName);
		}

	// What error are we expecting?
	if (!GetIntFromConfig(iSection, KExpectedError,iExpectedError))  
		{
		iExpectedError = KExpectedErrorNotUsed;
		}
	else 
		{
		INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
		}

	// What error are we expecting?
	if (!GetIntFromConfig(iSection, KExpectedError2,iExpectedError2))
		{
		iExpectedError2 = KExpectedErrorNotUsed;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCheckMobilityNotificationStep::doSingleTestStep()
	{
    SetTestStepResult(EFail);
    TDesC8* buffer = NULL;

	// Wait for the outstanding mobility notifications
    TInt error = iEsockTest->ReceiveMobilityNotification(buffer, iParams);

	if ((iExpectedError != KExpectedErrorNotUsed && error == iExpectedError)
		|| (iExpectedError2 != KExpectedErrorNotUsed && error == iExpectedError2))
		{
		INFO_PRINTF2(_L("Error (%d) received as expected"),error);
		SetTestStepResult(EPass);
		return TestStepResult();
		}

	// If we weren't expecting any mobility events and timed out waiting then we passed
	// But if we weren't expecting any and received one then we failed
    if (error == KErrCancel)
    	{
		if(iParams.iEventMask == 0)
			{
			INFO_PRINTF1(_L("Timed out waiting for notifications when expecting to do so."));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Timed out when expecting an event."));
			}
		return TestStepResult();
		}

	if((error == KErrNone) && (buffer != NULL))
		{
		if(iParams.iEventMask != 0)
			{
			CCommsApiExtRespMsg * msg = NULL;
			TRAP(error, msg = CCommsApiExtRespMsg::NewL(*buffer));

			if(error != KErrNone || msg == NULL)
				{
				INFO_PRINTF2(_L("Could not demarshall incoming notification. error:%d."), error);
				}
			else
				{
				//check types
				STypeId preferredCarrierAvailableTID = STypeId::CreateSTypeId(ESock::KMobilityMessagesImplementationUid, ESock::EPreferredCarrierAvailable);
				STypeId newCarrierActiveTID = STypeId::CreateSTypeId(ESock::KMobilityMessagesImplementationUid, ESock::ENewCarrierActive);

				if (msg->IsTypeOf(preferredCarrierAvailableTID))
					{
					CPreferredCarrierAvailable* pca = static_cast<CPreferredCarrierAvailable*>(msg);
					if ((iParams.iEventMask & ESock::EPreferredCarrierAvailable)
						&& (iNewAP==0 || iNewAP==pca->NewAPInfo().AccessPoint())
						&& (iOldAP==0 || iOldAP==pca->OldAPInfo().AccessPoint()))
						{
						INFO_PRINTF4(_L("%S: Received PreferredCarrierAvailable notification from AP%d to AP%d as expected."),&iParams.iMobilityExtName, pca->OldAPInfo().AccessPoint(), pca->NewAPInfo().AccessPoint());
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF4(_L("%S: Received PreferredCarrierAvailable notification from AP%d to AP%d which was unexpected."),&iParams.iMobilityExtName, pca->OldAPInfo().AccessPoint(), pca->NewAPInfo().AccessPoint());
						}
					}

				else if (msg->IsTypeOf(newCarrierActiveTID))
					{
					CNewCarrierActive* nca = static_cast<CNewCarrierActive*>(msg);
					if ((iParams.iEventMask & ESock::ENewCarrierActive)
						&& (iNewAP==0 || iNewAP==nca->NewAPInfo().AccessPoint()))
						{
						INFO_PRINTF3(_L("%S: Received NewCarrierActive notification for AP%d as expected."),&iParams.iMobilityExtName, nca->NewAPInfo().AccessPoint());
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF3(_L("%S: Received NewCarrierActive notification for AP%d which was unexpected."),&iParams.iMobilityExtName, nca->NewAPInfo().AccessPoint());
						}
					}
				}

			delete msg;
			REComSession::FinalClose(); //Different thread to the main one, need to be closed here as well
			}
		else
			{
			// We were expecting a timeout and instead received a notification so we have failed
			INFO_PRINTF1(_L("We were not expecting a notification expecting to timeout."));
			}
		}
    else if (error != KErrNone)
        {
        INFO_PRINTF3(_L("%S: The error code returned was %d."), &iParams.iMobilityExtName, error);
        SetTestStepError(error);
        }
    else if (buffer == NULL)
        {
        INFO_PRINTF2(_L("%S: Did not receive any notification"),&iParams.iMobilityExtName);
        }

	return TestStepResult();
	}


// Close 
//------

CCloseRCommsMobilityAPIExtStep::CCloseRCommsMobilityAPIExtStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRCommsMobilityAPIExtStep);
	}

TInt CCloseRCommsMobilityAPIExtStep::ConfigureFromIni()
	{
	iMobilityExtName.Set(KNullDesC);

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_MobilityAPIName, iMobilityExtName) != 1)
		|| (iMobilityExtName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRCommsMobilityAPIExtStep::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }
    
    mob->Close();
	return EPass;
	}

// Open
//-----

COpenRCommsMobilityAPIExtStep::COpenRCommsMobilityAPIExtStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRCommsMobilityAPIExtStep);
	}

TInt COpenRCommsMobilityAPIExtStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection, KTe_MobilityAPIName, iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iConnectionName)!=1)
        {
        INFO_PRINTF1(_L("Connection name missing."));
        return KErrNotFound;
        }

    return KErrNone;
    }

TVerdict COpenRCommsMobilityAPIExtStep::doSingleTestStep()
	{
	SetTestStepResult(EFail);
	RConnection* c = iEsockTest->FindConnection(iConnectionName);
	if (c==NULL)
		{
		INFO_PRINTF2(_L("Could not find RConnection (%S)."),&iConnectionName);
		return EFail;
		}

	TInt error = iEsockTest->CreateMobilityExtension(iMobilityExtName);
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Could not create mobility extension (%d)."),error);
		return EFail;
		}

	RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
	if (mob==NULL)
		{
		INFO_PRINTF1(_L("Could not find mobility extension."));
		return EFail;
		}

	error = mob->Open(*c);
	if (error!=KErrNone)
		{
		INFO_PRINTF3(_L("Could not apply %S to %S."),&iMobilityExtName,&iConnectionName);
		INFO_PRINTF2(_L("Error returned: %d."),error);
		SetTestStepError(error);
		return EFail;
		}

	return EPass;
	}


// Register for notifications
//---------------------------

CRegisterForMobilityNotificationStep::CRegisterForMobilityNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
	SetTestStepName(KRegisterForMobilityNotificationStep);
	}

TInt CRegisterForMobilityNotificationStep::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the next event container's name
    if (GetStringFromConfig(iSection, KTe_MobilityAPIName, iParams.iMobilityExtName)!=1
        || iParams.iMobilityExtName.Length()==0)
            return KErrNotFound;

	//calculate name for the event container (it is not settable from the ini)
	iEventName = iParams.iMobilityExtName;
	iEventName.Append(_L("_EvContainer"));
	iParams.iEventName.Set(iEventName);

    TPtrC eventName;
    if (GetStringFromConfig(iSection,KTe_Register,eventName)==1)
        {
        //first check, maybe someone wants to register for all events?
        if (eventName.Compare(KTe_All)==0)
            {
            iParams.iAllEvents = ETrue;
            INFO_PRINTF2(_L("%S: Events to be registered: All"),&iParams.iMobilityExtName);
            return KErrNone;
            }
        }

    //secondly..
    //go through the list of particular events to register.
    //there may be more than one expected event kind, register all of them
    TBool desciptionFound = ETrue;
    for (TInt idx = 0; desciptionFound; idx++)
        {
        TBuf<KMaxTestExecuteNameLength> fieldName(KTe_Register);
        fieldName.AppendNum(idx);
        if (GetStringFromConfig(iSection,fieldName,eventName)==1)
            {
            if (eventName.Compare(KTe_MobilityNotificationPreferredCarrierAvailable)==0)
        	    { iParams.iEventMask |= ESock::EPreferredCarrierAvailable; }
            else if (eventName.Compare(KTe_MobilityNotificationNewCarrierActive)==0)
        	    { iParams.iEventMask |= ESock::ENewCarrierActive; }
            else
               {
               INFO_PRINTF3(_L("%S: Mobility notification type (%S) not recognised."),&iParams.iMobilityExtName,&eventName);
               return KErrNotFound;
               }
            }
        else
            {
            //at least one event type must be present
            if (idx==0)
                {
                INFO_PRINTF2(_L("%S: Mobility notification type missing."),&iParams.iMobilityExtName);
                return KErrNotFound;
                }
            else
                {
                desciptionFound = KErrNotFound; //break the loop
                }
            }
        }

    INFO_PRINTF3(_L("%S: Mobility notifications to be registered: 0x%x"),&iParams.iMobilityExtName,iParams.iEventMask);
    return KErrNone;
	}

TVerdict CRegisterForMobilityNotificationStep::doSingleTestStep()
	{
   	TInt error = iEsockTest->RegisterForMobilityNotification(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not register for mobility notifications (%S)."),&iParams.iMobilityExtName);
        INFO_PRINTF2(_L("Error: %d"),error);
        return EFail;
        }

	return EPass;
	}
	
// Mobility notification	
//----------------------
Cmobilitynotifyrcommsmobilityapiext::Cmobilitynotifyrcommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Kmobilitynotifyrcommsmobilityapiext);
	}


TVerdict Cmobilitynotifyrcommsmobilityapiext::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }
    
   	TRequestStatus tStatus;
   	TBuf8<20> tResponseEventBuf;
   	mob->RegisterForMobilityNotification(tResponseEventBuf, tStatus);

	return EPass;
	}
	
TInt Cmobilitynotifyrcommsmobilityapiext::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
	}

// Cancel mobility notification
//-----------------------------
Ccancelnotifyrcommsmobilityapiext::Ccancelnotifyrcommsmobilityapiext(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(Kcancelnotifyrcommsmobilityapiext);
	}

TVerdict Ccancelnotifyrcommsmobilityapiext::doSingleTestStep()
	{
    RCommsMobilityApiExt* mob = iEsockTest->FindMobilityExtension(iMobilityExtName);
    if (mob==NULL)
        {
        INFO_PRINTF1(_L("Could not find mobility extension."));
        return EFail;
        }

	mob->CancelMobilityNotification();
	
	return EPass;
	}
	
TInt Ccancelnotifyrcommsmobilityapiext::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iMobilityExtName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);

    //try reading the name of the subconnection bundle
    if (GetStringFromConfig(iSection,KTe_MobilityAPIName,iMobilityExtName)!=1
        || iMobilityExtName.Length()==0)
            return KErrNotFound;

    return KErrNone;
	}

