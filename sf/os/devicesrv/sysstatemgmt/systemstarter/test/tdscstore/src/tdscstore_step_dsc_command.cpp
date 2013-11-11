// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tdscstore_step_dsc_command.h"
#include "tdscstore_constants.h"

#include <e32cmn.h>
#include <bautils.h>

/**
   Constructor
 */
CAppfwkDscStoreTestStepDscCommand::CAppfwkDscStoreTestStepDscCommand(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);		
	}

/**
   Destructor
 */
CAppfwkDscStoreTestStepDscCommand::~CAppfwkDscStoreTestStepDscCommand()
	{
	}

void CAppfwkDscStoreTestStepDscCommand::SetHeapFailNext(TInt aFailureIndex)
	{
	_LIT(KFailureIndex, "Failure Index : %d.");
	INFO_PRINTF2(KFailureIndex, aFailureIndex);
	
	iOOMSwitch = ETrue;
	CTestStep::SetHeapFailNext(aFailureIndex);
	}

TBool CAppfwkDscStoreTestStepDscCommand::CheckForHeapFailNext()
	{
	iOOMLoopDone = CTestStep::CheckForHeapFailNext();
	
	return iOOMLoopDone;
	}
	
/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkDscStoreTestStepDscCommand::doTestStepPreambleL()
	{
	// Check if we need to avoid the preamble connection
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	if (!BaflUtils::FolderExists(fs, KTestAvoidConnectionInPreamble))
		{
		// If the token folder does not exist ==> we need to connect
		iDscClient.OpenL();
		if (!iDscClient.IsOpened())
			{
			_LIT(KPrintStats,"Unable to connect to Dsc database.");
			ERR_PRINTF1(KPrintStats);
			User::Leave(KErrGeneral);
			}		
		}
 	CleanupStack::PopAndDestroy(&fs);
	
	//Set the enum file name
	//NOTE : This method is called here because when base test step constructor is called,
	//it does not know about which data (ini) file and which section user has mentioned
	//from script.So enum file name cannot be known at that point, so extra method is 
	//introduced to set the enum file.
	TPtrC enumFile;
	TBool readResult = GetStringFromConfig(KEnumsFile, KEnumsFile, enumFile);
	if (readResult)
		{
		SetEnumFileL(enumFile);
		}	

	//Disbale the INI Access log under OOM conditions to avoid large logging
	//for many iterations for OOM looping. We will restore the value when 
	//we finish the OOM looping in doTestStepPostambleL.
	if (ShouldRunOOMTest() && iOOMSwitch == EFalse)
		{	
		TPtrC stringValue;
		TBool readResult = GetTEFParameterL(KTEFEnableIniAccessLog, stringValue);
		
		if (readResult)
			{
			iIniAccessLogValue = stringValue;
			_LIT(KIniAccessLogOff, "OFF");
			TRAPD (err, SetTEFParameterL(KTEFEnableIniAccessLog, KIniAccessLogOff));
			User::LeaveIfError(err);
			}
		}
	
	return TestStepResult();
	}
	
/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkDscStoreTestStepDscCommand::doTestStepPostambleL()
	{
	iDscClient.Close();
	if (iDscClient.IsOpened())
		{
		_LIT(KPrintStats,"Unable to disconnect from Dsc database.");
		ERR_PRINTF1(KPrintStats);
		User::Leave(KErrGeneral);
		}
			
	//Set the return value (iErrVal) of the test step.
	SetTestStepError(iErrVal);
	SetTestStepResult(TVerdict(iErrVal));
		
	_LIT(KTestStepReturnValue, "TestStep Return Value : %d.");
	INFO_PRINTF2(KTestStepReturnValue, iErrVal);
	
	//Restore the INI Access log value(s) we set in doTestStepPreambleL during start of OOM loop
	if (ShouldRunOOMTest() && iOOMLoopDone)
		{		
		TRAPD (err, SetTEFParameterL(KTEFEnableIniAccessLog, iIniAccessLogValue));
		User::LeaveIfError(err);
		}
		
	return TestStepResult();
	}

/**
  Override of base class virtual.
  @return - TVerdict code
 */
TVerdict CAppfwkDscStoreTestStepDscCommand::doTestStepL()
	{	
	
	//This condition check is required because when TEF execute the test step 
	//under !OOM, it executes in the loop till KErrNoMemory is returned, but for 
	//the first execution its does not call method SetHeapFailNext(), i.e. when
	//failureIndex is '0'.
	
	//TEF expects the first call to be successful and then it iterates in a OOM loop.
	//In this component we are testing the database records, which once successful in
	//creation/deletion, would leave in next iteration with KErrAlreadyExist or KErrNotFound.
	//In which case it would not execute code further than that where it leaves.	
	//To avoid this we want to execute a successful API call only once when all the OOM 
	//simulated conditions are consumed by API under test. Thus we skip the '0'th iteration
	//execution which TEF expects to be successful.
	
	if (ShouldRunOOMTest() && iOOMSwitch == EFalse)
		{		
		return TestStepResult();
		}
	
	__UHEAP_MARK;
		
	TInt	dscId = KDefaultSymbianDsc;	
	GetHexFromConfig(ConfigSection(), KDscUid, dscId);
	
	//Set the DscUID
	iDscUid = TUid::Uid(dscId);
	
	TPtrC cmd = TestStepName();

	if (cmd == KCmdOpenDsc)
		{
		OpenDsc();
		}  
	else if (cmd == KCmdOpenCloseDsc)
		{
		CloseDsc();
		}
	else if (cmd == KCmdConnectedDsc)
		{
		ConnectedDsc();
		}
	else if (cmd == KCmdCreateDsc)
		{
		CreateDsc();
		}
	else if (cmd == KCmdDeleteDsc)
		{
		DeleteDsc();
		}
	else if (cmd == KCmdDscExist)
		{
		DscExist();
		}
	else if (cmd == KCmdEnumDscItems)
		{
		TRAP (iErrVal, EnumerateItemsL());
		}
	else if (cmd == KCmdEnumReadNextDscItem)
		{
		TRAP (iErrVal, EnumerateReadNextL());
		}
	else if (cmd == KCmdEnumOpenDsc)
		{
		TRAP (iErrVal, EnumOpenDscL());
		}
	else if (cmd == KCmdDscItemCount)
		{
		DscItemCount();
		}
	else if (cmd == KCmdCreateDscItem)
		{				
		CreateDscItem();
		}
	else if (cmd == KCmdUpdateDscItem)
		{		
		UpdateDscItem();		
		}
	else if (cmd == KCmdDeleteDscItem)
		{					
		DeleteDscItem();
		}
	else if (cmd == KCmdCompareDscItems)
		{		
		TRAP (iErrVal, CompareDscItemsL());
		}
	else if (cmd == KCmdDscItemExist)
		{		
		DscItemExists();
		}
	else if (cmd == KCmdReadDscItem)
		{		
		CDscItem *item = ReadDscItem();
		delete item;		
		}	
	else if (cmd ==	KCmdGetDscDescriptions)
		{
		TRAP (iErrVal, GetDscDescriptionsL());
		}	
	else if( cmd == KCmdCheckMaxStringLength )
		{
		CheckMaxStringLength();	
		}
	else
		{
		_LIT(KInvalidCommand,"Invalid Commmand : %S.");
		WARN_PRINTF2(KInvalidCommand,&cmd);
		}
		
	__UHEAP_MARKEND;
	
	//We have trapped all the leaves for DSC API calls
	//Make TEF aware of KErrNoMemory if its a OOM test
	if (ShouldRunOOMTest() /*&& iErrVal == KErrNoMemory*/)
		{
		User::LeaveIfError(iErrVal);	
		}
	
	return TestStepResult();
	}

/**
  Create the DscItem reading values from the ini file.
  @return - CDscItem
  @param - aUid 			DSCUid of the DSC
  @param - aParamKeyName	Keyname in the data file which has AMA parameters 
 */
CDscItem* CAppfwkDscStoreTestStepDscCommand::ConstructDscItemL(const TDesC& aParamKeyName)
	{
	TBool readResult = EFalse;
	
	CDscItem* dscItem = CDscItem::NewL();
	
	CleanupStack::PushL(dscItem);

	dscItem->SetDscId(iDscUid);
	
	TPtrC	amaFilename;
	
	readResult = GetStringFromConfig(ConfigSection(), KAMCFileName, amaFilename);
	if (readResult)
		{
		TPtrC	amaArguments;
		readResult = GetStringFromConfig(ConfigSection(), KAMCArguments, amaArguments);
		dscItem->SetFileParamsL(amaFilename, amaArguments);
		}
	else
		{
		_LIT(KAmaFileErrStatement, "AMC filename could not be read from file, try ItemId...");
		INFO_PRINTF1(KAmaFileErrStatement);
		
		TPtrC id;
		readResult = GetStringFromConfig(ConfigSection(), KItemId, id);
		if (readResult)
			{
			TLex lex(id);
			TUint itemId=0;
			User::LeaveIfError(lex.Val(itemId, EDecimal));
			dscItem->SetItemId(itemId);
			INFO_PRINTF1(_L("ItemId was setup according to Ini file"));
			}
		else
			{
			_LIT(KErrStatement, "Neither AMC filename nor ItemId could be read from file!!");
			INFO_PRINTF1(KErrStatement);
			}
		}
	
	if (aParamKeyName.Length() > 0)
		{
		TPtrC amaParamSection;
		readResult = GetStringFromConfig(ConfigSection(), aParamKeyName, amaParamSection);
		
		if (readResult)
			{	
			TInt	startupType = 0;
			GetEnumFromConfig(amaParamSection, KStartuptype, startupType);
			dscItem->SetStartupType(TStartupType(startupType));
			
			TInt	startMethod = 0;
			GetEnumFromConfig(amaParamSection, KStartMethod, startMethod);
			dscItem->SetStartMethodL(TStartMethod(startMethod));
			
			TInt	noOfRetries = 0;
			GetEnumFromConfig(amaParamSection, KNoOfReTries, noOfRetries);						
			dscItem->SetNoOfRetriesL(noOfRetries);
			
			TInt	timeout = 0;
			GetIntFromConfig(amaParamSection, KTimeout, timeout);						
			dscItem->SetTimeoutL(timeout);			
			
			TBool	isMonitored = EFalse;
			GetBoolFromConfig(amaParamSection, KMonitored, isMonitored);
			dscItem->SetMonitored(isMonitored);
			
			TBool	isStartInBackground = EFalse;			
			GetBoolFromConfig(amaParamSection, KStartInBackground, isStartInBackground);
			dscItem->SetStartInBackground(isStartInBackground);
						
			TBool	isViewless = EFalse;			
			GetBoolFromConfig(amaParamSection, KViewless, isViewless);
			dscItem->SetViewless(isViewless);
			}
		else
			{
			_LIT(KValueReadErr, "Value for %s could not be read.");
			INFO_PRINTF2(KValueReadErr, &aParamKeyName);
			}
		}
	
	CleanupStack::Pop(dscItem);
	
	return dscItem;
	}

void CAppfwkDscStoreTestStepDscCommand::PrintDscItemValues(const CDscItem& aDscItem)
	{
	if (!iOOMSwitch)
		{
		_LIT(KStartProcess, "EStartProcess");
		_LIT(KStartApp, "EStartApp");
		_LIT(KFireAndForget, "EFireAndForget");
		_LIT(KWaitforStart, "EWaitForStart");
		_LIT(KDeferredWaitForStart, "EDeferredWaitForStart");	
		
		const TPtrC startupType[3] = {KNullDesC(), KStartProcess(), KStartApp()};
		const TPtrC startMethod[4] = {KNullDesC(), KFireAndForget(), KWaitforStart(), KDeferredWaitForStart()};	
		
		_LIT(KDscInfoStart, "========= DSC ITEM INFO START =========");	
		_LIT(KDscId, "DscId			: 0x%x");
		_LIT(KItemId, "ItemId			: %d");
		_LIT(KFileName, "FileName			: \"%S\"");
		_LIT(KArgs, "Args			: \"%S\"");
		_LIT(KStartupTypeVal, "StartupType		: \"%S\"");
		_LIT(KStartMethodVal, "StartMethod		: \"%S\"");
		_LIT(KNoOfRetriesVal, "NoOfRetries		: %d");
		_LIT(KTimeoutVal, "Timeout			: %d");	
		_LIT(KMonitoredVal, "Monitored		: %d");
		_LIT(KViewlessVal, "Viewless			: %d");
		_LIT(KStartInBackGroundVal, "StartInBackground	: %d");
		_LIT(KDscInfoEnd, "========= DSC ITEM INFO END ==========");
		
		INFO_PRINTF1(KDscInfoStart);	
		INFO_PRINTF2(KDscId, aDscItem.DscId());
		INFO_PRINTF2(KItemId, aDscItem.ItemId());
		const TPtrC amaFile = aDscItem.FileName();
		const TPtrC amaArgs = aDscItem.Args();
		INFO_PRINTF2(KFileName, &amaFile);	
		INFO_PRINTF2(KArgs, &amaArgs);
		INFO_PRINTF2(KStartupTypeVal, &startupType[aDscItem.StartupType()]);
		INFO_PRINTF2(KStartMethodVal, &startMethod[aDscItem.StartMethod()]);
		INFO_PRINTF2(KNoOfRetriesVal, aDscItem.NoOfRetries());
		INFO_PRINTF2(KTimeoutVal, aDscItem.Timeout());	
		INFO_PRINTF2(KMonitoredVal, aDscItem.Monitored());
		INFO_PRINTF2(KViewlessVal, aDscItem.Viewless());
		INFO_PRINTF2(KStartInBackGroundVal, aDscItem.StartInBackground());
		INFO_PRINTF1(KDscInfoEnd);
		}
	}


void CAppfwkDscStoreTestStepDscCommand::OpenDsc()
	{
	TRAP (iErrVal, iDscClient.OpenL());

	if (iErrVal == KErrNone && !iDscClient.IsOpened())
		{
		_LIT(KPrintStats,"Unable to connect to Dsc database.");
		ERR_PRINTF1(KPrintStats);
		iErrVal = KErrGeneral;
		}
		
	if (iErrVal == KErrNone)	
		{
		// We better close it unless we want a memory leak...(not a problem, it is only used as part of UT)
		CloseDsc();
		}
	}

void CAppfwkDscStoreTestStepDscCommand::CloseDsc()
	{
	iDscClient.Close();
	if (iDscClient.IsOpened())
		{
		_LIT(KPrintStats,"Unable to close Dsc database.");
		ERR_PRINTF1(KPrintStats);
		iErrVal = KErrGeneral;
		}
	}

void CAppfwkDscStoreTestStepDscCommand::ConnectedDsc()
	{
	if (!iDscClient.IsOpened())
		{
		INFO_PRINTF1(_L("Dsc is not connected."));
		iErrVal = KErrNotReady;
		}
	}

void CAppfwkDscStoreTestStepDscCommand::CreateDsc()
	{
	TPtrC desc;
	GetStringFromConfig(ConfigSection(), KDescription, desc);

	if (iDscUid == TUid::Uid(KDefaultSymbianDsc) && desc == KNullDesC)
		{
		INFO_PRINTF1(_L("Create a DSC with value KDefaultSymbianDsc"));
		TRAP (iErrVal, iDscClient.CreateDscL());
		}
	else
		{
		INFO_PRINTF2(_L("Create a DSC with value of Uid %u"), iDscUid);
		TRAP (iErrVal, iDscClient.CreateDscL(iDscUid, desc));
		}
	}
	
void CAppfwkDscStoreTestStepDscCommand::DeleteDsc()
	{
	TRAP (iErrVal, iDscClient.DeleteDscL(iDscUid));
	}

TBool CAppfwkDscStoreTestStepDscCommand::DscExist()
	{
	TBool dscExists = ETrue;

	if (iDscUid == TUid::Uid(KDefaultSymbianDsc))
		{
		TRAP (iErrVal, dscExists = iDscClient.DscExistsL());
		}
	else
		{
		TRAP (iErrVal, dscExists = iDscClient.DscExistsL(iDscUid));
		}	
	
	if (iErrVal == KErrNone && !dscExists)
		{
		_LIT(KDscNotFound, "Dsc (%d) not found.");
		INFO_PRINTF2(KDscNotFound, iDscUid);
		iErrVal = KErrNotFound;	
		}
		
	return dscExists;
	}
	
void CAppfwkDscStoreTestStepDscCommand::CreateDscItem()
	{
	CDscItem* item = NULL;
	
	TRAP (iErrVal, item = ConstructDscItemL(KCreateDscItemSection));
	
	if (iErrVal == KErrNone)
		{
		TInt dscPos = 0;
		GetEnumFromConfig(ConfigSection(), KDscPosition, dscPos);
		
		if (dscPos == ELast)
			{
			TRAP (iErrVal, iDscClient.AddItemL(*item));
			}
		else
			{
			TRAP (iErrVal, iDscClient.AddItemL(*item, TDscPosition(dscPos)));
			}
		}
	
	delete item;
	}

void CAppfwkDscStoreTestStepDscCommand::DeleteDscItem()
	{
	CDscItem* item = NULL;
	
	TRAP (iErrVal, item = ConstructDscItemL(KCreateDscItemSection));
	
	if (iErrVal == KErrNone)
		{
		TRAP (iErrVal, iDscClient.DeleteItemL(*item));
		}
	
	delete item;
	}

void CAppfwkDscStoreTestStepDscCommand::UpdateDscItem()
	{
	CDscItem* item = NULL;
	
	TRAP (iErrVal, item = ConstructDscItemL(KModifyDscItemSection));
	
	if (iErrVal == KErrNone)
		{					
		TRAP (iErrVal, iDscClient.UpdateItemL(*item));
		}
		
	delete item;
	}
	
TBool CAppfwkDscStoreTestStepDscCommand::DscItemExists()
	{
	CDscItem* item = NULL;
		
	TRAP (iErrVal, item = ConstructDscItemL(KCreateDscItemSection));
	
	TBool itemExists = EFalse;
	
	if (iErrVal == KErrNone)
		{		
		TRAP (iErrVal, itemExists = iDscClient.ItemExistsL(*item));
		
		if (iErrVal == KErrNone && !itemExists)
			{
			_LIT(KDscItemNotFound, "DscItem not found");
			ERR_PRINTF1(KDscItemNotFound);
			iErrVal = KErrNotFound;
			}				
		}
	
	delete item;
	return itemExists;
	}
		
CDscItem* CAppfwkDscStoreTestStepDscCommand::ReadDscItem()
	{
	CDscItem* item = NULL;
	
	TRAP (iErrVal, item = ConstructDscItemL(KCreateDscItemSection));

	if (iErrVal == KErrNone)
		{
		TRAP (iErrVal, iDscClient.ReadItemL(*item));
		}
		
	if (item)
		{
		PrintDscItemValues(*item);
		}
	
	return item;
	}

/**
  Compares the DSC item parameters.First DSC item is constructed from data file which is compared agianst the
  DSC item parameters from the actual database.
  @return - If the both the DSC Item are identical or not.
  @param - aDscItem		DSC Item constructed from data file.
 */
TBool CAppfwkDscStoreTestStepDscCommand::CompareDscItemsL()
	{
	CDscItem* item = NULL;
	
	TRAP (iErrVal, item = ConstructDscItemL(KCompareDscItemSection));
	
	CleanupStack::PushL(item);
	
	TBool sameItems = EFalse;
	
	if (iErrVal == KErrNone)
		{	
		//Read the item from the database to be compared (based on FileName, Arguments and DscId provided)
		CDscItem* dscItemFromDb = ReadDscItem();
		CleanupStack::PushL(dscItemFromDb);
		if(dscItemFromDb)
			{
			sameItems = ( (item->FileName().Compare(dscItemFromDb->FileName()) == 0) &&
							(item->Args().Compare(dscItemFromDb->Args()) == 0) &&
							item->StartupType() == dscItemFromDb->StartupType() &&
							item->StartMethod() == dscItemFromDb->StartMethod() &&
							item->NoOfRetries() == dscItemFromDb->NoOfRetries() &&
							item->Timeout() == dscItemFromDb->Timeout() &&
							item->Monitored() == dscItemFromDb->Monitored() &&
							item->Viewless() == dscItemFromDb->Viewless() &&
							item->StartInBackground() == dscItemFromDb->StartInBackground()
							);
			}
		CleanupStack::PopAndDestroy(dscItemFromDb);
		
		if (!sameItems)
			{
			_LIT(KDscComparisonFailed, "DscItems comparison failed. Items are not same.");
			ERR_PRINTF1(KDscComparisonFailed);
			iErrVal = KErrGeneral;
			}
		}
	
	CleanupStack::PopAndDestroy(item);
	
	return sameItems;
	}


void CAppfwkDscStoreTestStepDscCommand::EnumOpenDscL()
	{
	if (iDscUid==TUid::Uid(KDefaultSymbianDsc))
		{
		INFO_PRINTF1(_L("Open KDefaultSymbianDsc for enumeration"));
		iDscClient.EnumOpenLC();
		}
	else
		{
		INFO_PRINTF2(_L("Open dscUid: %u for enumeration"), iDscUid);	
		iDscClient.EnumOpenLC(iDscUid);
		}

	//Read the wait time (if specified in the data file), to create database 
	//lock condition after we called EnumOpenLC. Other test steps can execute
	//concurrently while we put this command/test step on sleep.
	TInt waitTime = 0;
	TInt readResult = GetIntFromConfig(ConfigSection(), KWaitInSecs, waitTime);
	
	if (readResult)
		{
		_LIT(KWait, "Waiting for %d seconds...");
		INFO_PRINTF2(KWait, waitTime);
		User::After(waitTime * 1000000);
		}

	// no functional difference between the two ways (jut to cover EnumClose() for the sake of coverage)
	if (iDscUid==TUid::Uid(KDefaultSymbianDsc))
		{
		CleanupStack::PopAndDestroy();
		}
	else
		{
		iDscClient.EnumClose();
		CleanupStack::Pop();
		}
	}

/**
  Enumerates through all the DSC items for a particular DSC and print the DSCItem info,if asked to.
  @return - Number of DSC Items present in the DSC
*/
TInt CAppfwkDscStoreTestStepDscCommand::EnumerateReadNextL()
 	{
	TInt count = 0;	
	while(CDscItem* dscItem = iDscClient.EnumReadNextL())
		{
		++count;
 		PrintDscItemValues(*dscItem);
	 	delete dscItem;
	 	dscItem = NULL;
	 	}
	return count;
 	}


/**
  Enumerates through all the DSC items for a particular DSC and print the DSCItem info,if asked to.
  @return - Number of DSC Items present in the DSC
  @param - aDscUid 			DSCUid of the DSC
 */
 TInt CAppfwkDscStoreTestStepDscCommand::EnumerateItemsL()
	{
	iDscClient.EnumOpenLC(iDscUid);

	//Read the wait time (if specified in the data file), to create database 
	//lock condition after we called EnumOpenLC. Other test steps can execute
	//concurrently while we put this command/test step on sleep.
	TInt waitTime = 0;
	TInt readResult = GetIntFromConfig(ConfigSection(), KWaitInSecs, waitTime);
	
	if (readResult)
		{
		_LIT(KWait, "Waiting for %d seconds...");
		INFO_PRINTF2(KWait, waitTime);
		User::After(waitTime * 1000000);
		}
	
	TInt count = EnumerateReadNextL();
	
	CleanupStack::PopAndDestroy(); //iDscClient.EnumClose();
	
	return count;
	}

/**
  Enumerates through all the DSC Items present in the DSC and calculate the count.
  Data file specifies the count, against which check will be made on calculated count,
  if expected and actual counts are same or not.  
  @param - aDscUid 		DSCUid of the DSC  
 */
void CAppfwkDscStoreTestStepDscCommand::DscItemCount()
	{
	TInt dscItemCount = 0;
	
	TRAP (iErrVal, dscItemCount = EnumerateItemsL());	
	
	TInt checkValue = 0;
	GetIntFromConfig(ConfigSection(), KDscItemCount, checkValue);
	
	_LIT(KCountResult, "Expected count : %d Actual count : %d");
	
	if (checkValue != dscItemCount)
		{
		ERR_PRINTF3(KCountResult, checkValue, dscItemCount);
		iErrVal = KErrGeneral;
		}
	else
		{
		INFO_PRINTF3(KCountResult, checkValue, dscItemCount);
		}
	}


/**
  Test GetDscDescriptionL(), compare the result with the expect value in Ini
  If expected value doesn't exist in Ini, KNullDesC should be returned from the API 
*/	
void CAppfwkDscStoreTestStepDscCommand::GetDscDescriptionsL()
	{
	TPtrC expected;
	GetStringFromConfig(ConfigSection(), KDescription, expected);

	TInt length = 0;
	iDscClient.GetMaxStringLength( length );
	HBufC* buf = HBufC::NewLC( length );
	TPtr description = buf->Des();
  	iDscClient.GetDscDescriptionL(iDscUid, description);

	_LIT(KCompareResult, "Expected description : '%S' Actual description : '%S'");
	if (description != expected)
		{
		ERR_PRINTF3(KCompareResult, &expected, &description);
		iErrVal = KErrGeneral;
		}
	else
		{
		INFO_PRINTF3(KCompareResult, &expected, &description);
		}
		
	CleanupStack::PopAndDestroy(buf);
	}


/**
  Ensure the DscDatabase can be interrogated to find the maximum query-string length.
*/
void CAppfwkDscStoreTestStepDscCommand::CheckMaxStringLength()
	{
	
	TInt stringLength = 0;
	
	iDscClient.GetMaxStringLength( stringLength );

#ifndef DSCSTORE_SQL
	if( KDbMaxStrLen != stringLength )
		{
		iErrVal = KErrGeneral;	
		ERR_PRINTF3( _L("KDbMaxStrLen is %d but DB returned %d"), KDbMaxStrLen, stringLength );
		}
#else
	ASSERT(0);	//TODO: ASSERT the max supported length of VARCHAR in SQL Lite
#endif //DSCSTORE_SQL				
	}

