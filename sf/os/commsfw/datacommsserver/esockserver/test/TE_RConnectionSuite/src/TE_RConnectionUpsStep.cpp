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
// This file contains the implementation of TE_RConnectionUpsStep which is 
// the base class for all the TE_RConnectionUpsTestSteps test step classes
// 
//

/**
 @file 
*/


#include "TE_RConnectionUpsStep.h"
#include <e32property.h>
#include <cinidata.h>
#include <test/upstestnotifierproperties.h>
#include <s32mem.h>
#include <ups/upsclient.h>
#include <comms-infras/esocktestutils.h>


TE_RConnectionUpsStep::~TE_RConnectionUpsStep()
	{
	iNotifierResponse.Close();
	iResult.Close();
	iIncrNotifyCount.Close();
	}

TInt TE_RConnectionUpsStep::GetConfigData()
/**
 * Loads in test configuration information from the .ini
 * file defined by KUpsTestIniFile
 */
 	{
	CIniData* configFile;
	TRAPD(err, configFile = CIniData::NewL(KUpsTestIniFile));
	if(err != KErrNone)
		{
		return err;
		}
	CleanupStack::PushL(configFile);
	
	INFO_PRINTF2(_L("UPS Test Config File: %S"), &KUpsTestIniFile);
	INFO_PRINTF2(_L("UPS Test Config File Section ( %S )"), &ConfigSection());
	
	//Get the StartDelay
	TInt startDelay = 0;
	configFile->FindVar(ConfigSection(), KStartDelayKey, startDelay);
	iStartDelay = startDelay*KOneMilliSecond;
	
	//Get DeleteUpsDb info
	TPtrC deleteUpsDb;
	configFile->FindVar(ConfigSection(), KDeleteUpsDbKey, deleteUpsDb);
	if(deleteUpsDb == _L("Yes"))
		{
		iDeleteUpsDB = ETrue;
		}
	else if(deleteUpsDb == _L("No"))
		{
		iDeleteUpsDB = EFalse;
		}
	else
		{
		CleanupStack::PopAndDestroy(configFile);
		return KErrArgument;
		}
	
	//Get the Notifier Responses
	TInt count = 0;
	TPtrC response; 
	TBuf<20> notifierResponseKey(KNotifierResponseKey);
	notifierResponseKey.AppendNum(count);
	
	while(configFile->FindVar(ConfigSection(), notifierResponseKey, response))
		{
		if(response == _L("Always"))
			{
			iNotifierResponse.Append(CPolicy::EAlways);
			}
		else if(response == _L("Never"))
			{
			iNotifierResponse.Append(CPolicy::ENever);			
			}
		else if(response == _L("SessionYes"))
			{
			iNotifierResponse.Append(CPolicy::ESessionYes);			
			}
		else if(response == _L("SessionNo"))
			{
			iNotifierResponse.Append(CPolicy::ESessionNo);			
			}
		else
			{
			CleanupStack::PopAndDestroy(configFile);
			return KErrArgument;
			}
		
		++count;
		notifierResponseKey.Copy(KNotifierResponseKey);
		notifierResponseKey.AppendNum(count);
		}
	
	if (0 == count)
		{
		CleanupStack::PopAndDestroy(configFile);
		return KErrArgument;
		}
	
	//Get the results
	count = 0;
	TInt result = KErrNotFound;
	TBuf<10> checkResultKey(KCheckResultKey);
	checkResultKey.AppendNum(count);

	while(configFile->FindVar(ConfigSection(), checkResultKey, result))
		{
		iResult.Append(result);
		
		++count;
		checkResultKey.Copy(KCheckResultKey);
		checkResultKey.AppendNum(count);
		}
	
	if (0 == count)
		{
		CleanupStack::PopAndDestroy(configFile);
		return KErrArgument;
		}
	
	//Get the NotifyCount
	count = 0;
	TInt incrNotifyCount = 0;
	TBuf<20> incrNotifyCountKey(KIncrNotifyCountKey);
	incrNotifyCountKey.AppendNum(count);
	
	while(configFile->FindVar(ConfigSection(), incrNotifyCountKey, incrNotifyCount))
		{
		iIncrNotifyCount.Append(incrNotifyCount);
	
		++count;
		incrNotifyCountKey.Copy(KIncrNotifyCountKey);
		incrNotifyCountKey.AppendNum(count);
		}
	
	if (0 == count)
		{
		CleanupStack::PopAndDestroy(configFile);
		return KErrArgument;
		}
	
	CleanupStack::PopAndDestroy(configFile);
	return KErrNone;
	}


enum TVerdict TE_RConnectionUpsStep::doTestStepPreambleL()
/**
 * Preamble loads in any UPS test specific test configuration, then
 * calls the RConnectionStep Preamble
 */
 	{
	TESTL(GetConfigData() == KErrNone);
	return TE_RConnectionStep::doTestStepPreambleL();
	}



void TE_RConnectionUpsStep::DoTestStartDelay()
/**
 * Performs a delay based on the value set in the test config file.
 * Used for delaying the start of a test
 */
 
	{
	if(iStartDelay > 0)
		{
		INFO_PRINTF2(_L("Delaying start of test by ( %d milliseconds )"), iStartDelay/1000);
		User::After(iStartDelay);
		}
	}


TInt TE_RConnectionUpsStep::CheckAndDeleteUpsDB()
/**
 * Deletes the UPS decision database based on whether the test
 * configuration file has specified this
 */
	{
	TInt ret = KErrGeneral;
	if(iDeleteUpsDB)
		{
		ret = DeleteUpsDB();
		}
	else
		{
		ret = KErrNone;
		}
	
	return ret;
	}


TInt TE_RConnectionUpsStep::CheckResult(const TInt aResult)
/**
 * Checks the result of a function call with the expected result
 *
 * @param aResult The result returned by a function call
 * @return KErrGeneral If the result does not match the expected result
 * @return KErrNone If the result matches the expected result
 */
	{
	TInt ret = KErrGeneral;
	
	if(aResult == iResult[iResultCount])
		{
		ret = KErrNone;
		INFO_PRINTF2(_L("RConnection/RSocket/RHostResolver API returned ( %d ) as expected"), aResult);
		}
	else
		{
		ret = KErrGeneral;
		ERR_PRINTF1(_L("RConnection/RSocket/RHostResolver API DID NOT return as expected!"));
		ERR_PRINTF3(_L("API returned ( %d ) when ( %d ) was expected."),aResult ,iResult[iResultCount]);
		}
	
	++iResultCount;
	return ret;
	}


TInt TE_RConnectionUpsStep::SetNotifierResponse()
/**
 * Set the test notifier response based on the next value from the test
 * configuration file. This function needs to be called before the API call
 * which triggers User Prompting
 */
	{
	TInt ret = SetNotifierResponse(iNotifierResponse[iNotifierResponseCount]);
	++iNotifierResponseCount;
	return ret;
	}



TInt TE_RConnectionUpsStep::CheckNotifyCount(const TInt aCountBefore, const TInt aCountAfter)
/**
 * Checks that the NotifyCount has increased by the number specified in 
 * the configuration file
 * 
 * @param aCountBefore The NotifyCount before the API which triggers User Prompting, has been called
 * @param aCountAfter The NotifyCount after the API which triggers User Prompting, has been called
 *
 * @return KErrGeneral If the result does not match the expected result
 * @return KErrNone If the result matches the expected result
 */
	{
	TInt ret = KErrGeneral;
	TBool isNewNotify(EFalse);
	
	TInt expectedCount = aCountBefore + iIncrNotifyCount[iNotifyCheckCount];

	//Check if a new Prompt has occurred
	if(aCountBefore < aCountAfter)
		{
		isNewNotify = ETrue;
		}
	
	//Check that the expected number of prompts have occurred
	if(aCountAfter == expectedCount)
		{
		ret = KErrNone;		
		}
	else if(expectedCount < aCountAfter)
		{
		ERR_PRINTF1(_L("An unexpected prompt ocurred!"));
		ret = KErrOverflow;
		}
	else
		{
		ERR_PRINTF1(_L("Prompt did not occur when one was expected!"));
		ret = KErrUnderflow;
		}
	
	++iNotifyCheckCount;
	
	GetAndPrintNotifyData(aCountAfter, isNewNotify);
	
	return ret;
	}


TInt TE_RConnectionUpsStep::DeleteUpsDB()
/**
 * Does the work of deleting the UPS Decision Database by calling UPS 
 * Management APIs
 */
	{
	UserPromptService::RUpsManagement upsManagement;
	
	//Connect to the UPS Manager
	TInt ret = upsManagement.Connect();
 	if (ret != KErrNone)
 		{
 		ERR_PRINTF2(_L("Failed to Connect to the UPS Manager  ( %d )"), ret);
 		return ret;
 		}
	
	//Delete the contents of the Decision Database
 	TRAPD(err, upsManagement.DeleteDatabaseL());
 	if (err != KErrNone)
 		{
 		ERR_PRINTF2(_L("UPS Database Deletion failed ( %d )"), err);
 		}
 	else
 		{
 		INFO_PRINTF1(_L("UPS Database Deleted"));
 		} 	
	
	return err;
	}


TInt TE_RConnectionUpsStep::SetNotifierDelay(const TInt aDelay)
/**
 * Sets the Notifier response delay using P&S
 *
 * @param aDelay the Notifier response delay in seconds
 */
	{
	TInt ret = RProperty::Set(KUidPSUPSTestNotifCategory, KUtButtonPressDelay, aDelay);
	return ret;
	}
	

TInt TE_RConnectionUpsStep::SetNotifierResponse(const CPolicy::TOptions aButtonPress)
/**
 * Does the work of setting the Notifier Response (Button Press) using P&S
 */
	{
	TPromptResult promptResult;
	promptResult.iSelected = aButtonPress;
	TPckg<TPromptResult> resultPckg(promptResult);
	TInt ret = RProperty::Set(KUidPSUPSTestNotifCategory, KUtButtonPress,resultPckg);

	TBuf<16> buttonPressString;
	switch(aButtonPress)
		{
		case CPolicy::EAlways:
			buttonPressString = _L("Always");
			break;
		case CPolicy::ENever:
			buttonPressString = _L("Never");
			break;
		case CPolicy::ESessionYes:
			buttonPressString = _L("SessionYes");
			break;
		case CPolicy::ESessionNo:
			buttonPressString = _L("SessionNo");
			break;
		default:
			buttonPressString = _L("");
		}
	
	TPtrC buttonPressStringPtr(buttonPressString);
	
	INFO_PRINTF1(_L(" "));
	INFO_PRINTF2(_L("--- NOTIFER BUTTON PRESS  ( %S ) ---"), &buttonPressStringPtr);
	INFO_PRINTF1(_L(" "));
	
	return ret;
	}

TInt TE_RConnectionUpsStep::GetNotifyCount()
/**
 * Does the work of getting the Notifier Count using P&S
 */
	{
	TInt count = 0;
	TInt ret = RProperty::Get(KUidPSUPSTestNotifCategory, KUnNotifyCount, count);
	if(ret!=KErrNone)
		{
		return ret;
		}
	else
		{
		return count;	
		}
	}

TInt TE_RConnectionUpsStep::GetStoredNotifyCount()
/**
 * Does the work of getting the "Stored" Notify Count
 * The "Stored" Notify Count is usually used with MultThreaded and
 * MultiProcess tests. The Get function is called after the test
 * Threads/Process have finished, usually from a separate test step.
 */
	{
	TInt count = 0;
	TInt ret = RProperty::Get(KUidPSUPSTestNotifCategory, KUnStoredNotifyCount, count);
	if(ret!=KErrNone)
		{
		return ret;
		}
	else{
		return count;	
		}
	}

TInt TE_RConnectionUpsStep::SetStoredNotifyCount(const TInt aCount)
/**
 * Does the work of setting the "Stored" Notify Count
 * The "Stored" Notify Count is usually used with MultThreaded and
 * MultiProcess tests. The Set function is called before the test
 * Threads/Process are kicked off, usually from a separate test step.
 */

	{
	TInt ret = RProperty::Set(KUidPSUPSTestNotifCategory, KUnStoredNotifyCount, aCount);
	return ret;
	}
	
CPromptData* TE_RConnectionUpsStep::GetNotifyDataL()
/**
 * Does the work of getting the Notify Data (Prompt Data)
 *
 * @return Returns a pointer to a CPromptData containing Prompt Data, 
 * which the calling function is responsible for deleting
 */
	{
	CPromptData* promptDataPtr = CPromptData::NewL();
	CleanupStack::PushL(promptDataPtr);

	const TInt KMaxPromptDataLength = 512;
    HBufC8* promptBuf=HBufC8::NewLC(KMaxPromptDataLength);
    TPtr8 promptBufPtr(promptBuf->Des());
    
	TInt ret = RProperty::Get(KUidPSUPSTestNotifCategory, KUnNotifyValues, promptBufPtr);
	if (ret != KErrNone)
		{
		CleanupStack::PopAndDestroy(2); //promptBuf, promptDataPtr
		return NULL;
		}
	
	TInt promptBufLen = promptBufPtr.Length();
	if(promptBufLen==0)
		{
		CleanupStack::PopAndDestroy(2); //promptBuf, promptDataPtr
		return NULL;
		}
	
	RDesReadStream rs;
	rs.Open(promptBufPtr);
	CleanupClosePushL(rs);
	rs >> *promptDataPtr;
	
	CleanupStack::PopAndDestroy(2); //rs, promptBuf
	CleanupStack::Pop(promptDataPtr);
	return promptDataPtr;
	}
	
void TE_RConnectionUpsStep::GetAndPrintNotifyData(const TInt aNotifyCount, const TBool aIsNewNotifyData)
/**
 * Checks if a new Notify/Prompt occurred and prints the Notify/Prompt
 * data accordingly
 *
 * @param aNotifyCount The NotifyCount
 * @param aIsNewNotifyData Indicates whether a new prompt occurred or not
 */
	{
	if(EFalse == aIsNewNotifyData)
		{
		INFO_PRINTF1(_L(" "));
		INFO_PRINTF1(_L("--- NO PROMPT ---"));
		INFO_PRINTF1(_L(" "));
		return;
		}
	
	CPromptData* notifyData;
	TRAPD(err, notifyData = GetNotifyDataL());
	if(err != KErrNone)
		{
		ERR_PRINTF1(_L("***Error getting Notify Data***"));
		return;
		}
	else if(notifyData == NULL)
		{
		ERR_PRINTF1(_L("***NULL PromptData Pointer!!***"));
		return;
		}
			
	INFO_PRINTF1(_L(" "));
	INFO_PRINTF1(_L("--- PROMPT DATA ---"));
	INFO_PRINTF2(_L("Prompt Count  ( %d )"), aNotifyCount);
	INFO_PRINTF2(_L("Client Name   ( %S )"), &notifyData->iClientName);
	INFO_PRINTF2(_L("Vendor Name   ( %S )"), &notifyData->iVendorName);
	INFO_PRINTF2(_L("Destination   ( %S )"), &notifyData->iDestination);
	INFO_PRINTF2(_L("ServerSid     ( %08x )"), notifyData->iServerSid.iId);
	INFO_PRINTF2(_L("ServiceId     ( %08x )"), notifyData->iServiceId.iUid);
	//INFO_PRINTF2(_L("Opaque Data  ( %S )"), &notifyData->iDescriptions);
	INFO_PRINTF1(_L(" "));
	
	delete notifyData;
	notifyData = NULL;
	}


