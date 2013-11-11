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
 @file ConnectionServer.TestSteps.cpp
*/

#include "Commsdat.TestSteps.h"


using namespace CommsDat;
using namespace Meta;


_LIT(KRecordType, "RecordType");
_LIT(KRecordId, "RecordId");
_LIT(KAccessPointRecord, "AccessPointRecord");

// Update record
//--------------
CUpdateCommsdatRecordStep::CUpdateCommsdatRecordStep(CCEsockTestBase*& aEsockTest)
:   CCommsdatTestStep(aEsockTest)
	{
	SetTestStepName(KUpdateCommsdatRecordStep);
	}

TVerdict CUpdateCommsdatRecordStep::doTestStepPreambleL()
	{
	// TODO_CDG do we need this preamble here at all?
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CUpdateCommsdatRecordStep::ConfigureFromIni()
	{
	// Read in record type name
	if((GetStringFromConfig(iSection, KRecordType, iRecordTypeName) != 1)
		|| (iRecordTypeName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordType);
		return KErrNotFound;
		}

	// Read in the record id
	if(GetIntFromConfig(iSection, KRecordId, iRecordId) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordId);
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

_LIT(KAppSID, "AppSID");
TVerdict CUpdateCommsdatRecordStep::doSingleTestStep()
	{
    // Default to fail
    SetTestStepResult(EFail);
    
    // Open a session with the comms database
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
	CleanupStack::PushL(cmdbSession);

	// Which type of record are we updating
	if(iRecordTypeName == KAccessPointRecord)
		{
		CCDAccessPointRecord *accessPointRecord = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
		CleanupStack::PushL(accessPointRecord);

		// Set the record id first
		accessPointRecord->SetRecordId(iRecordId);
		
		// Set each of the record specific fields
		TInt tempInt;
		if(GetIntFromConfig(iSection, KAppSID, tempInt))
			{
			// Set the field within the record
			accessPointRecord->iAppSID = static_cast<TUint>(tempInt);
			}
		
		// Apply the record changes to the database
		TRAPD(error, accessPointRecord->ModifyL(*cmdbSession);)

		// Don't need the record any more
		CleanupStack::PopAndDestroy(accessPointRecord);

		if(error != KErrNone)
			{
			SetTestStepError(error);
			INFO_PRINTF2(_L("Deletion of record failed. error:%d"), error);
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF2(_L("Invalid or unsupported record type recordType:%S"), &iRecordTypeName);
		SetTestStepError(KErrNotSupported);
		}

	// Cleanup and out of here
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}


// Delete record
//--------------
CDeleteCommsdatRecordStep::CDeleteCommsdatRecordStep(CCEsockTestBase*& aEsockTest)
:   CCommsdatTestStep(aEsockTest)
	{
	SetTestStepName(KDeleteCommsdatRecordStep);
	}

TVerdict CDeleteCommsdatRecordStep::doTestStepPreambleL()
	{
	// TODO_CDG do we need this preamble here at all?
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CDeleteCommsdatRecordStep::ConfigureFromIni()
	{
	// Read in record type name
	if((GetStringFromConfig(iSection, KRecordType, iRecordTypeName) != 1)
		|| (iRecordTypeName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordType);
		return KErrNotFound;
		}

	// Read in the record id
	if(GetIntFromConfig(iSection, KRecordId, iRecordId) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordId);
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CDeleteCommsdatRecordStep::doSingleTestStep()
	{
    // Default to fail
    SetTestStepResult(EFail);
    
    // Open a session with the comms database
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
	CleanupStack::PushL(cmdbSession);

	if(iRecordTypeName == KAccessPointRecord)
		{
		CCDAccessPointRecord *accessPointRecord = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
		CleanupStack::PushL(accessPointRecord);

		// Set the record id and perform the deletion
		accessPointRecord->SetRecordId(iRecordId);
		TRAPD(error, accessPointRecord->DeleteL(*cmdbSession);)

		CleanupStack::PopAndDestroy(accessPointRecord);

		if(error != KErrNone)
			{
			SetTestStepError(error);
			INFO_PRINTF2(_L("Deletion of record failed. error:%d"), error);
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF2(_L("Invalid or unsupported record type recordType:%S"), &iRecordTypeName);
		SetTestStepError(KErrNotSupported);
		}

	// Cleanup and out of here
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}



// Create record
//--------------
CCreateCommsdatRecordStep::CCreateCommsdatRecordStep(CCEsockTestBase*& aEsockTest)
:   CCommsdatTestStep(aEsockTest)
	{
	SetTestStepName(KCreateCommsdatRecordStep);
	}

TVerdict CCreateCommsdatRecordStep::doTestStepPreambleL()
	{
	// TODO_CDG do we need this preamble here at all?
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateCommsdatRecordStep::ConfigureFromIni()
	{
	// Read in record type name
	if((GetStringFromConfig(iSection, KRecordType, iRecordTypeName) != 1)
		|| (iRecordTypeName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordType);
		return KErrNotFound;
		}

	// Read in the record id
	if(GetIntFromConfig(iSection, KRecordId, iRecordId) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KRecordId);
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateCommsdatRecordStep::doSingleTestStep()
	{
    // Default to fail
    SetTestStepResult(EFail);
    
    // Open a session with the comms database
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
	CleanupStack::PushL(cmdbSession);

	// Which type of record are we creating
	if(iRecordTypeName == KAccessPointRecord)
		{
		TInt error;
		CMDBSession *cmdbSession = NULL;
		cmdbSession = CMDBSession::NewL(KCDVersion1_2);
		CleanupStack::PushL(cmdbSession);

		cmdbSession->OpenTransactionL();

		// Create a new record object locally which we will populate provided it doesn't exist already
		CCDAccessPointRecord *accessPointRecord = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
		CleanupStack::PushL(accessPointRecord);

		// Set the record id first
		accessPointRecord->SetRecordId(iRecordId);

		// See if there is already a record in the database
		TBool found(0);
		TRAP(error, found = accessPointRecord->FindL(*cmdbSession););

		if(!found)
			{
			TInt tempInt;
			
			// iRecordTag
			if(GetIntFromConfig(iSection, _L("RecordTag"), tempInt))
				{
				accessPointRecord->iRecordTag = static_cast<TUint>(tempInt);
				}
			
			// iTier
			if(GetIntFromConfig(iSection, _L("Tier"), tempInt))
				{
				accessPointRecord->iTier = static_cast<TUint>(tempInt);
				}
			
			// iMCpr
			if(GetIntFromConfig(iSection, _L("MCpr"), tempInt))
				{
				accessPointRecord->iMCpr = static_cast<TUint>(tempInt);
				}
			
			// iCpr
			if(GetIntFromConfig(iSection, _L("Cpr"), tempInt))
				{
				accessPointRecord->iCpr = static_cast<TUint>(tempInt);
				}
			
			// iSCpr
			if(GetIntFromConfig(iSection, _L("SCpr"), tempInt))
				{
				accessPointRecord->iSCpr = static_cast<TUint>(tempInt);
				}
			
			// iProtocol
			if(GetIntFromConfig(iSection, _L("Protocol"), tempInt))
				{
				accessPointRecord->iProtocol = static_cast<TUint>(tempInt);
				}
			
			// iCprConfig
			if(GetIntFromConfig(iSection, _L("CprConfig"), tempInt))
				{
				accessPointRecord->iCprConfig = static_cast<TUint>(tempInt);
				}
			
			// iSelectionPolicy
			if(GetIntFromConfig(iSection, _L("AccessPointSelectionPolicy"), tempInt))
				{
				accessPointRecord->iSelectionPolicy = static_cast<TUint>(tempInt);
				}

			// iAppSID
			if(GetIntFromConfig(iSection, _L("AppSID"), tempInt))
				{
				accessPointRecord->iSelectionPolicy = static_cast<TUint>(tempInt);
				}
			
			// iConfigAPIdList
			TPtrC tempString;
			if(GetStringFromConfig(iSection, _L("ConfigAPIdList"), tempString))
				{
				accessPointRecord->iConfigAPIdList = tempString;
				}
			
			// Store the new record and commit the transaction
			// TODO_CDG need to report on failed trapped routines here
			TRAP(error, accessPointRecord->StoreL(*cmdbSession););
			TRAP(error, cmdbSession->CommitTransactionL(););
			}

		cmdbSession->Close();
		CleanupStack::PopAndDestroy(accessPointRecord);
		CleanupStack::PopAndDestroy(cmdbSession);

		if(error != KErrNone)
			{
			SetTestStepError(error);
			INFO_PRINTF2(_L("Insertion of new record failed. error:%d"), error);
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF2(_L("Invalid or unsupported record type recordType:%S"), &iRecordTypeName);
		SetTestStepError(KErrNotSupported);
		}

	// Cleanup and out of here
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}



