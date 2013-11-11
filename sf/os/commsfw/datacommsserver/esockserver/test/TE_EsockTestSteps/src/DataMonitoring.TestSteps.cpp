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
 @file DataMonitoring.TestSteps.cpp
*/

#include "DataMonitoring.TestSteps.h"
#include "Connections.TestSteps.h"
#include "SubConnections.TestSteps.h"


// Opening data monitor
//---------------------
COpenRCommsDataMonitoringApiExtStep::COpenRCommsDataMonitoringApiExtStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRCommsDataMonitoringApiExtStep);
	}

TInt COpenRCommsDataMonitoringApiExtStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
	// Read in required fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1) &&
			(GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName) != 1))
		{
		INFO_PRINTF1(_L("Connection/subconnection name missing."));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict COpenRCommsDataMonitoringApiExtStep::doSingleTestStep()
	{
	SetTestStepResult(EFail);

   TInt error = iEsockTest->CreateDataMonitoringExtension(iParams);

   if(error != KErrNone)
      {
      INFO_PRINTF2(_L("Could not create data monitoring extension (%d)."), error);
      }
   else
	   {
	   error = iEsockTest->OpenDataMonitoringExtension(iParams);

	   if (error != KErrNone)
	      {
	      INFO_PRINTF3(_L("Could not apply %S to %S."), &iParams.iDataMonitorName, &(iParams.ConnOrSubConnName()));
	      INFO_PRINTF2(_L("Error returned: %d."), error);
	      SetTestStepError(error);
	      }
	   else
	   	{
	   	SetTestStepResult(EPass);
	   	}
	   }

	return TestStepResult();
	}


// Closing data monitor
//---------------------

CCloseRCommsDataMonitoringApiExtStep::CCloseRCommsDataMonitoringApiExtStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRCommsDataMonitoringApiExtStep);
	}

TInt CCloseRCommsDataMonitoringApiExtStep::ConfigureFromIni()
	{
	// Only interested in the connection we should be trying to close
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1
		|| (iParams.iDataMonitorName.Length() == 0)))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRCommsDataMonitoringApiExtStep::doSingleTestStep()
	{
   TInt error = iEsockTest->CloseDataMonitoringExtension(iParams);

   if (error != KErrNone)
      {
      INFO_PRINTF2(_L("Could not close %S."), &iParams.iDataMonitorName);
      INFO_PRINTF2(_L("Error returned: %d."), error);
      return EFail;
      }

	return EPass;
	}


// Request Data Transferred
//-------------------------
CRequestDataTransferredStep::CRequestDataTransferredStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRequestDataTransferredStep);
	}

TInt CRequestDataTransferredStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Extension name missing."));
		return KErrNotFound;
		}

	// Get the threshold values to be tested against
	// They needn't be specified but will default to zero
	GetIntFromConfig(iSection, KReceivedVolumeThreshold, iReceivedVolumeThreshold);
	GetIntFromConfig(iSection, KSentVolumeThreshold, iSentVolumeThreshold);

    // Get the threshold condition types (make them EEqual as a default)
	iParams.iReceivedDataVolumeCondition = EEqual;
	iParams.iSentDataVolumeCondition = EEqual;

    TPtrC condition;
    GetStringFromConfig(iSection, KReceivedVolumeCondition, condition);
    if(condition.Length() != 0)
    	{
		if (condition == KGreaterThanEqual)
			iParams.iReceivedDataVolumeCondition = EGreaterThanEqual;
		else if (condition == KLessThan)
			iParams.iReceivedDataVolumeCondition = ELessThan;
		else if (condition == KEqual)
			iParams.iReceivedDataVolumeCondition = EEqual;
		else
			{
			INFO_PRINTF1(_L("Condition type not recognised."));
			return KErrNotFound;
			}
		}

	GetStringFromConfig(iSection, KSentVolumeCondition, condition);
	if(condition.Length() != 0)
		{
		if (condition == KGreaterThanEqual)
			iParams.iSentDataVolumeCondition = EGreaterThanEqual;
		else if (condition == KLessThan)
			iParams.iSentDataVolumeCondition = ELessThan;
		else if (condition == KEqual)
			iParams.iSentDataVolumeCondition = EEqual;
		else
			{
			INFO_PRINTF1(_L("Condition type not recognised."));
			return KErrNotFound;
			}
		}

    // Are we to compare against a previous request?
    GetStringFromConfig(iSection, KCompareDataRequestResultsName, iParams.iCompareAgainstName);

    // Are we to save the results for comparison in a later request?
    GetStringFromConfig(iSection, KSaveDataRequestResultsName, iParams.iSaveResultsName);

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CRequestDataTransferredStep::doSingleTestStep()
	{
	TVerdict testVerdict = EPass;
	TDataMonitorEventParams params;
	params.iDataMonitorName.Set(iParams.iDataMonitorName);

	TBuf8<KNotificationEventMaxSize> responseBuf;
	iEsockTest->RequestDataTransferred(params, responseBuf);

	ESock::CDataTransferred* dataTransferred = NULL;
	TRAPD(ret, dataTransferred = static_cast<ESock::CDataTransferred*>(CCommsApiExtRespMsg::NewL(responseBuf)));

	if(ret != KErrNone)
		{
		INFO_PRINTF2(_L("Demarshalling data transferred reponse buffer failed with %d."), ret);
		return EFail;
		}

	// Log what we have received
	INFO_PRINTF3(_L("Transferred. received:%d bytes, sent:%d bytes"), dataTransferred->ReceivedBytes(), dataTransferred->SentBytes());

	// Get any saved reference point of received/sent bytes
	TUint receivedBytesReference = 0;
	TUint sentBytesReference = 0;

	// If we are to compare against the results against a previous request then fetch these as reference points
	if (iParams.iCompareAgainstName.Length() != 0)
		{
		// Get the reference point container
		TDataMonitorStats* savedResults = iEsockTest->FetchSavedDataMonitorStats(iParams.iCompareAgainstName);
		if (!savedResults)
			{
			INFO_PRINTF1(_L("Saved data monitor reference point not found."));
			delete dataTransferred;
			testVerdict = EFail;
			}

		// Update reference points for later comparison
		receivedBytesReference = savedResults->iReceivedBytes;
		sentBytesReference = savedResults->iSentBytes;
		}

	// Log the reference we are comparing to
	INFO_PRINTF3(_L("Reference.   received:%d bytes, sent:%d bytes"), receivedBytesReference, sentBytesReference);

	// Log the difference
	INFO_PRINTF3(_L("Difference.  received:%d bytes, sent:%d bytes"),
		(dataTransferred->ReceivedBytes() - receivedBytesReference),
		(dataTransferred->SentBytes() - sentBytesReference));

	// Save the reference point if we are expected to
	if (iParams.iSaveResultsName.Length() != 0)
		{
		TDataMonitorStats* statsToSave = new TDataMonitorStats();
		
		if (statsToSave)
			{
			// Create container with saved results
			statsToSave->iReceivedBytes = dataTransferred->ReceivedBytes();
			statsToSave->iSentBytes = dataTransferred->SentBytes();

			// Save the results away under the specified name
			TInt err = iEsockTest->SaveDataMonitorStats(iParams.iSaveResultsName, *statsToSave);

			if (err != KErrNone)
				{
				INFO_PRINTF1(_L("Could not save data monitor reference point."));
				delete statsToSave;
				testVerdict = EFail;
				}
			}
		}

	// Perform comparisons
	// Received Data
	switch(iParams.iReceivedDataVolumeCondition)
		{
		case EGreaterThanEqual:
			// Received data must be at least (greater than or equal) the threshold specified
			// and any saved reference from previous request
			if(dataTransferred->ReceivedBytes() < (iReceivedVolumeThreshold + receivedBytesReference))
				{
				INFO_PRINTF1(_L("Received less data than expected."));
				testVerdict = EFail;
				}
			break;

		case ELessThan:
			// Received data must be less than the threshold specified and any saved reference from previous request
			if(dataTransferred->ReceivedBytes() >= (iReceivedVolumeThreshold + receivedBytesReference))
				{
				INFO_PRINTF1(_L("Received more data than expected."));
				testVerdict = EFail;
				}
			break;

		case EEqual:
			// Must have received exactly the number of bytes specified as the threshold
			if(dataTransferred->ReceivedBytes() != (iReceivedVolumeThreshold + receivedBytesReference))
				{
				INFO_PRINTF1(_L("Did not receive exactly as much data as expected."));
				testVerdict = EFail;
				}
			break;

		default:
			INFO_PRINTF1(_L("Unrecognised condition type."));
			testVerdict = EFail;
			break;
		}

	// Sent Data
	switch(iParams.iSentDataVolumeCondition)
		{
		case EGreaterThanEqual:
			// Volume of data must be at least the threshold specified
			if(dataTransferred->SentBytes() < (iSentVolumeThreshold + sentBytesReference))
				{
				INFO_PRINTF1(_L("Data volume less than that expected."));
				testVerdict = EFail;
				}
			break;

		case ELessThan:
			// Volume of data must be less than the threshold specified
			if(dataTransferred->SentBytes() >= (iSentVolumeThreshold + sentBytesReference))
				{
				INFO_PRINTF1(_L("Data volume more than that expected."));
				testVerdict = EFail;
				}
			break;

		case EEqual:
			// Data volume must be exactly that specified
			if(dataTransferred->SentBytes() != (iSentVolumeThreshold + sentBytesReference))
				{
				INFO_PRINTF1(_L("Did not receive exactly as much data as expected."));
				testVerdict = EFail;
				}
			break;

		default:
			INFO_PRINTF1(_L("Unrecognised condition type."));
			testVerdict = EFail;
			break;
		}

	delete dataTransferred;
	return testVerdict;
	}


// Request Data Sent Notification
//-------------------------------
CRequestDataSentNotificationStep::CRequestDataSentNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRequestDataSentNotificationStep);
	}


TInt CRequestDataSentNotificationStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Extension name missing."));
		return KErrNotFound;
		}
		
	// Fetch the threshold we are expecting to meet
	if(!GetIntFromConfig(iSection, KSentVolumeThreshold, (TInt&)iParams.iMinExpectedVolume))
		{
		INFO_PRINTF1(_L("Threshold missing."));
		return KErrNotFound;
		}

	// Need to create a receiver for the data monitor notification event
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response =	iEsockTest->FindOrCreateDataMonitoringResponder(iParams);
	if(!response)
		{
		return KErrNoMemory;
		}
	else
		{
		response->iComplete = EFalse;
		}

	// All ok if we got this far
	return KErrNone;
	}

TVerdict CRequestDataSentNotificationStep::doSingleTestStep()
	{
	return iEsockTest->RequestDataSentNotification(iParams) == KErrNone ? EPass : EFail;
	}


// Request Data Received Notification
//-----------------------------------
CRequestDataReceivedNotificationStep::CRequestDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRequestDataReceivedNotificationStep);
	}


TInt CRequestDataReceivedNotificationStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Extension name missing."));
		return KErrNotFound;
		}
		
	// Fetch the threshold we are expecting to meet
	if(!GetIntFromConfig(iSection, KReceivedVolumeThreshold, (TInt&)iParams.iMinExpectedVolume))
		{
		INFO_PRINTF1(_L("Threshold missing."));
		return KErrNotFound;
		}

	// Need to create a receiver for the data monitor notification event
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response =	iEsockTest->FindOrCreateDataMonitoringResponder(iParams);
	if(!response)
		{
		return KErrNoMemory;
		}
	else
		{
		response->iComplete = EFalse;
		}

	// All ok if we got this far
	return KErrNone;
	}

TVerdict CRequestDataReceivedNotificationStep::doSingleTestStep()
	{
	return iEsockTest->RequestDataReceivedNotification(iParams) == KErrNone ? EPass : EFail;
	}

// Receive Data Sent Notification
//-------------------------------
CReceiveDataSentNotificationStep::CReceiveDataSentNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KReceiveDataSentNotificationStep);
	}

TInt CReceiveDataSentNotificationStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Extension name missing."));
		return KErrNotFound;
		}
		
	// Fetch the event responder for the data monitor
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response = iEsockTest->FindDataMonitoringResponder(iParams);
	if (!response)
		{
		INFO_PRINTF1(_L("Data monitor event responder not found."));
		return KErrNotFound;
		}
	
	// The wait can possibly time out. Find out if it should be expected for this test
	TBool expectingTimeout = GetBoolFromConfig(iSection, KExpectTimeout, iParams.iExpectTimeout)
		&& iParams.iExpectTimeout;
	TBool timeoutValuePresent = GetIntFromConfig(iSection, KTimeout, response->iTimeout);
	TBool thresholdPresent = GetIntFromConfig(iSection, KSentVolumeThreshold, (TInt&)iParams.iMinExpectedVolume);

	// Perform some checks of dependency between settings
	if (!expectingTimeout && !thresholdPresent)
	{
		INFO_PRINTF1(_L("Must have a threshold to compare with if no timeout expected."));
		return KErrArgument;
	}
	else if (expectingTimeout && !timeoutValuePresent)
	{
		INFO_PRINTF1(_L("Timeout expected but no timeout specified."));
		return KErrArgument;
	}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CReceiveDataSentNotificationStep::doSingleTestStep()
	{
	// Default to failing
	SetTestStepResult(EFail);
	
	TDesC8* responseBuf;
	TInt ret = iEsockTest->ReceiveDataSentNotification(iParams, responseBuf);

	// De-serialise the notification
	TInt unpackError;
	ESock::CDataSent* dataSent = NULL;
	if(ret == KErrNone)
		{
		TRAP(unpackError, dataSent = static_cast<ESock::CDataSent*>(CCommsApiExtRespMsg::NewL(*responseBuf)));
		if(unpackError != KErrNone)
			{
			INFO_PRINTF2(_L("Demarshalling data sent response buffer failed with %d."), ret);
			}
		else
			{
			// Log the amount of data received in the notification
			INFO_PRINTF2(_L("Data transferred: sent:%d bytes"), dataSent->SentBytes());
			}
		}

	// Maybe we have already completed this notification
	if (ret != KErrNone && ret != KErrTimedOut)
		{
		INFO_PRINTF2(_L("This notification already completed in some way! err:%d"), ret);
		}
	else
		{
		// Did we expect to timeout or were we expecting the notification to be received
		if (iParams.iExpectTimeout)
			{
			if (ret == KErrTimedOut)
				{
				INFO_PRINTF2(_L("Receiving notification passed - received expected timeout %d."), ret);
				SetTestStepResult(EPass);
				}
			else if (ret == KErrNone)
				{
				// Were expecting to timeout but instead received the notification
				INFO_PRINTF1(_L("Receiving notification failed - received notification when expecting timeout."));
				}
			}
		else
			{
			// Received timeout when not expecting it ie. notification failed to arrive
			if (ret == KErrTimedOut)
				{
				INFO_PRINTF2(_L("Receiving notification failed - timed out unexpectedly %d."), ret);
				}
			else if (ret == KErrNone)
				{
				if(unpackError == KErrNone)
					{
					// Received the notification as expected
					INFO_PRINTF1(_L("Receiving notification passed - received notification as expected."));

					// Log the amount of data received in the notification
					INFO_PRINTF2(_L("Data transferred: sent:%d bytes"), dataSent->SentBytes());

					// Check now the expected number of bytes
					if((iParams.iMinExpectedVolume > 0) && (dataSent->SentBytes() < iParams.iMinExpectedVolume))
						{
						INFO_PRINTF1(_L("Sent less than the specified expected minimum volume."));
						}
					else
						{
						// All of we passed
						SetTestStepResult(EPass);
						}
					}
				else
					{
					INFO_PRINTF1(_L("Receiving notification failed - problem unpacking notification %d"));
					}
				}
			}
		}

	// We got this far without failing so therefore we passed
	delete dataSent;
	return TestStepResult();
	}

// Receive Data Received Notification
//-----------------------------------
CReceiveDataReceivedNotificationStep::CReceiveDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KReceiveDataReceivedNotificationStep);
	}

TInt CReceiveDataReceivedNotificationStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Extension name missing."));
		return KErrNotFound;
		}
		
	// Fetch the event responder for the data monitor
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response = iEsockTest->FindDataMonitoringResponder(iParams);
	if (!response)
		{
		INFO_PRINTF1(_L("Data monitor event responder not found."));
		return KErrNotFound;
		}
	
	// The wait can possibly time out. Find out if it should be expected for this test
	TBool expectingTimeout = GetBoolFromConfig(iSection, KExpectTimeout, iParams.iExpectTimeout)
		&& iParams.iExpectTimeout;
	TBool timeoutValuePresent = GetIntFromConfig(iSection, KTimeout, response->iTimeout);
	TBool thresholdPresent = GetIntFromConfig(iSection, KReceivedVolumeThreshold, (TInt&)iParams.iMinExpectedVolume);

	// Perform some checks of dependency between settings
	if (!expectingTimeout && !thresholdPresent)
	{
		INFO_PRINTF1(_L("Must have a threshold to compare with if no timeout expected."));
		return KErrArgument;
	}
	else if (expectingTimeout && !timeoutValuePresent)
	{
		INFO_PRINTF1(_L("Timeout expected but no timeout specified."));
		return KErrArgument;
	}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CReceiveDataReceivedNotificationStep::doSingleTestStep()
	{
	// Default to failing
	SetTestStepResult(EFail);
	
	TDesC8* responseBuf;
	TInt ret = iEsockTest->ReceiveDataReceivedNotification(iParams, responseBuf);

	// De-serialise the notification
	TInt unpackError;
	ESock::CDataReceived* volumeOfData = NULL;
	if(ret == KErrNone)
		{
		TRAP(unpackError, volumeOfData = static_cast<ESock::CDataReceived*>(CCommsApiExtRespMsg::NewL(*responseBuf)));
		if(unpackError != KErrNone)
			{
			INFO_PRINTF2(_L("Demarshalling data sent response buffer failed with %d."), ret);
			}
		else
			{
			// Log the amount of data received in the notification
			INFO_PRINTF2(_L("Data transferred. received:%d bytes"), volumeOfData->ReceivedBytes());
			}
		}

	// Maybe we have already completed this notification
	if (ret != KErrNone && ret != KErrTimedOut)
		{
		INFO_PRINTF2(_L("This notification already completed in some way! err:%d"), ret);
		}
	else
		{
		// Did we expect to timeout or were we expecting the notification to be received
		if (iParams.iExpectTimeout)
			{
			if (ret == KErrTimedOut)
				{
				INFO_PRINTF2(_L("Receiving notification passed - received expected timeout %d."), ret);
				SetTestStepResult(EPass);
				}
			else if (ret == KErrNone)
				{
				// Were expecting to timeout but instead received the notification
				INFO_PRINTF1(_L("Receiving notification failed - received notification when expecting timeout."));
				}
			}
		else
			{
			// Received timeout when not expecting it ie. notification failed to arrive
			if (ret == KErrTimedOut)
				{
				INFO_PRINTF2(_L("Receiving notification failed - timed out unexpectedly %d."), ret);
				}
			else if (ret == KErrNone)
				{
				if(unpackError == KErrNone)
					{
					// Received the notification as expected
					INFO_PRINTF1(_L("Receiving notification passed - received notification as expected."));

					// Log the amount of data received in the notification
					INFO_PRINTF2(_L("Data transferred. received:%d bytes"), volumeOfData->ReceivedBytes());

					// Check now the expected number of bytes
					if((iParams.iMinExpectedVolume > 0) && (volumeOfData->ReceivedBytes() < iParams.iMinExpectedVolume))
						{
						INFO_PRINTF1(_L("Sent less than the specified expected minimum volume."));
						}
					else
						{
						// All of we passed
						SetTestStepResult(EPass);
						}
					}
				else
					{
					INFO_PRINTF1(_L("Receiving notification failed - problem unpacking notification %d"));
					}
				}
			}
		}

	// We got this far without failing so therefore we passed
	delete volumeOfData;
	return TestStepResult();
	}

// Cancel Data Sent Notification
//------------------------------
CCancelDataSentNotificationStep::CCancelDataSentNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCancelDataSentNotificationStep);
	}

TInt CCancelDataSentNotificationStep::ConfigureFromIni()
	{
	// Only interested in the connection we should be trying to close
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // What error are we expecting?
	if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))
		{
		iExpectedError = KExpectedErrorNotUsed;
		}
    
	// Need to find the responder for the data monitor
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response =	iEsockTest->FindDataMonitoringResponder(iParams);
	if(!response)
		{
		INFO_PRINTF1(_L("Data monitoring responder not found."));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCancelDataSentNotificationStep::doSingleTestStep()
	{
	// Call Cancel on the data monitoring provider
	TInt ret = iEsockTest->CancelDataSentNotificationRequest(iParams);
	INFO_PRINTF2(_L("Cancelling notification returned error code: %d."), ret);

	// Determine verdict based upon the return code from the cancellation and that expected
	if(iExpectedError == KExpectedErrorNotUsed)
		{
		return (ret == KErrNone) ? EPass : EFail;
		}
	else
		{
		return (ret == iExpectedError) ? EPass : EFail;
		}
	}

// Cancel Data Received Notification
//----------------------------------
CCancelDataReceivedNotificationStep::CCancelDataReceivedNotificationStep(CCEsockTestBase*& aEsockTest)
   : CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCancelDataReceivedNotificationStep);
	}

TInt CCancelDataReceivedNotificationStep::ConfigureFromIni()
	{
	// Only interested in the connection we should be trying to close
	if((GetStringFromConfig(iSection, KDataMonitorName, iParams.iDataMonitorName) != 1)
		|| (iParams.iDataMonitorName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // What error are we expecting?
	if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))
		{
		iExpectedError = KExpectedErrorNotUsed;
		}
    
	// Need to find the responder for the data monitor
	iParams.iEventName.Copy(iParams.iDataMonitorName);
	iParams.iEventName.Append(KDataMonitorResponderNameSuffix);
	TEsockTestDataMonitoringResponse* response =	iEsockTest->FindDataMonitoringResponder(iParams);
	if(!response)
		{
		INFO_PRINTF1(_L("Data monitoring responder not found."));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCancelDataReceivedNotificationStep::doSingleTestStep()
	{
	// Call Cancel on the data monitoring provider
	TInt ret = iEsockTest->CancelDataReceivedNotificationRequest(iParams);
	INFO_PRINTF2(_L("Cancelling notification returned error code: %d."), ret);

	// Determine verdict based upon the return code from the cancellation and that expected
	if(iExpectedError == KExpectedErrorNotUsed)
		{
		return (ret == KErrNone) ? EPass : EFail;
		}
	else
		{
		return (ret == iExpectedError) ? EPass : EFail;
		}
	}

