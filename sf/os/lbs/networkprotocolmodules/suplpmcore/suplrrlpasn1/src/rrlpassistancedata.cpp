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

/**
 @file
 @internalTechnology
 
*/

#include "RRLP-Messages.h"
#include "rrlpassistancedata.h"
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "supldevloggermacros.h" 


/**
Static Factory Constructor
*/
EXPORT_C CRrlpAssistanceData* CRrlpAssistanceData::NewL()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceData::NewL() Begin\n");
	CRrlpAssistanceData* self = new (ELeave) CRrlpAssistanceData();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CRrlpAssistanceData::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}


/**
Private default constructor
*/
CRrlpAssistanceData::CRrlpAssistanceData()
 : CRrlpMessageBase(ERrlpAssistanceData, EFalse)
	{
	}


/**
Second stage constructor
*/
void CRrlpAssistanceData::ConstructL()
	{
	}


/**
Destructor
*/
CRrlpAssistanceData::~CRrlpAssistanceData()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceData::~CRrlpAssistanceData() Begin\n");
	SUPLLOG(ELogP1, "CRrlpAssistanceData::~CRrlpAssistanceData() End\n");
	}
	

/** 
AssistanceDataPresent()

Indicates whether supported assistance data is present in the received message.
This implementation supports only receipt of GPS assistance data, hence this
method returns ETrue if GPS assistance data is present in the received content.

@return ETrue if valid assistance data is present in the received message
*/
EXPORT_C TBool CRrlpAssistanceData::AssistanceDataPresent()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceData::AssistanceDataPresent() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.assistanceData != NULL, User::Invariant());
	
	if (iData->component.u.assistanceData->m.gps_AssistDataPresent == 0)
		{
		SUPLLOG(ELogP1, "CRrlpAssistanceData::AssistanceDataPresent(EFalse) End\n");
		return EFalse;
		}
	SUPLLOG(ELogP1, "CRrlpAssistanceData::AssistanceDataPresent(ETrue) End\n");
	return ETrue;
	}


/** 
BuildAssistanceData()

@param  aDataMask populated to indicate what assistance data exists
@param  aData     reference to assistance data builder object to populate with
                  received assistance data
@return KErrNotFound if gps assistance data is not present in the received message,
        KErrNone otherwise
*/
EXPORT_C TInt CRrlpAssistanceData::BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet)
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceData::BuildAssistanceData() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.assistanceData != NULL, User::Invariant());

	
	// return immediately if no gps assistance data present
	if (iData->component.u.assistanceData->m.gps_AssistDataPresent == 0)
		{
		SUPLLOG(ELogP1, "CRrlpAssistanceData::BuildAssistanceData() End (no assistance data available)\n");
		return KErrNotFound;
		}
		
	// pointer to the received/decoded GPS Assistance Data element
	ASN1T_ControlHeader& gpsData = iData->component.u.assistanceData->gps_AssistData.controlHeader;

	SUPLLOG(ELogP1, "CRrlpAssistanceData::BuildAssistanceData() End\n");
	return CRrlpMessageBase::BuildAssistanceData(aDataMask, aAssistanceDataBuilderSet, gpsData);
	}


/** 
MoreAssDataToBeSent()

@return ETrue if another RRLP Assistance Data Message is pending
*/
EXPORT_C TBool CRrlpAssistanceData::MoreAssDataToBeSent()
	{
	SUPLLOG(ELogP1, "CRrlpAssistanceData::MoreAssDataToBeSent() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.assistanceData != NULL, User::Invariant());
	
	// check the optional field is present
	if (iData->component.u.assistanceData->m.moreAssDataToBeSentPresent != 0)
		{
		// check the content of the field.
		if (iData->component.u.assistanceData->moreAssDataToBeSent == 1)
			{
			SUPLLOG(ELogP1, "CRrlpAssistanceData::MoreAssDataToBeSent(ETrue) End\n");
			return ETrue;
			}
		}
	
	SUPLLOG(ELogP1, "CRrlpAssistanceData::MoreAssDataToBeSent(EFalse) End\n");
	return EFalse;
	}


/**
GetExtendedReference()

Populates aRrlpRef if Rel-5 Extended Reference is present in the received message

@param  aRrlpRef local copy of the session reference details
@return KErrNotFound if the extended reference parameters are not present,
        KErrNone otherwise
*/
TInt CRrlpAssistanceData::GetExtendedReference(TRrlpReference& aRrlpRef)
	{
	__ASSERT_DEBUG(iData->component.u.assistanceData != NULL, User::Invariant());
	
	if (iData->component.u.assistanceData->m.rel5_AssistanceData_ExtensionPresent != 0)
		{
		aRrlpRef.aRel5EntendedRefPresent = ETrue;
		aRrlpRef.aRel5SmlcCode = iData->component.u.assistanceData->rel5_AssistanceData_Extension.extended_reference.smlc_code;
		aRrlpRef.aRel5TransactionId = iData->component.u.assistanceData->rel5_AssistanceData_Extension.extended_reference.transaction_ID;
		}
	else
		{
		return KErrNotFound;
		}
	return KErrNone;
	}

