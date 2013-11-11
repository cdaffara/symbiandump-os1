/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32property.h>
#include <centralrepository.h>

// LBS-specific
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancealmanac.h> 
#include <lbs/lbsassistanceaquisitionassistance.h> 
#include <lbs/lbsassistancebadsatlist.h> 
#include <lbs/lbsassistanceionosphericmodel.h> 
#include <lbs/lbsassistancenavigationmodel.h> 
#include <lbs/lbsassistancereferencelocation.h> 
#include <lbs/lbsassistancereferencetime.h> 
#include <lbs/lbsassistanceutcmodel.h> 

#include "lbsassistancedatacacheapi.h"
#include "lbsprocessuiddefs.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"

// List of all the assistance data types
const TLbsAssistanceDataItem KAssistanceDataList[KAssistanceDataListCount]=
	{
	EAssistanceDataAquisitionAssistance,
	EAssistanceDataBadSatList,
	EAssistanceDataNavigationModel,
	EAssistanceDataReferenceTime,
	EAssistanceDataIonosphericModel,
	EAssistanceDataDgpsCorrections,
	EAssistanceDataReferenceLocation,
	EAssistanceDataAlmanac,
	EAssistanceDataPositioningGpsUtcModel
	};

const TLbsAsistanceDataGroup KAllAssistanceData = 
				EAssistanceDataAquisitionAssistance | 
				EAssistanceDataBadSatList | 
				EAssistanceDataNavigationModel | 
				EAssistanceDataReferenceTime | 
				EAssistanceDataIonosphericModel | 
				EAssistanceDataDgpsCorrections | 
				EAssistanceDataReferenceLocation | 
				EAssistanceDataAlmanac | 
				EAssistanceDataPositioningGpsUtcModel;

/**
Static initializer. 
Defines the properties for each of the types of assistance data
Leaves if any error (except KErrAlreadyExists)
*/
EXPORT_C void RAssistanceDataCache::InitializeL()
    {
	_LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
    TInt err;
    
    for (TInt i = 0; i < KAssistanceDataListCount; i++)
    	{
	    err = RProperty::Define(KLbsAssistDataApiBase + i,
								RProperty::ELargeByteArray,
								KAllowAllPolicy,
								KAllowAllPolicy, 
								GetDataSize(KAssistanceDataList[i]));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		}
				
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt categoryUid(RProcess().SecureId());
	User::LeaveIfError(rep->Set(KNgAssistanceDataUpdatesCategoryKey, categoryUid));
	CleanupStack::PopAndDestroy(rep);
    }

/**
Utility function for deleting a property while checking any error.
KerrNone andKErrNotFound are acceptable, any other error causes a Leave.

@param  aPropertyKey A key of the property to be deleted
@return An error code returned by RProperty::Delete
*/
TInt RAssistanceDataCache::DeletePropertyL(TInt aPropertyKey)
    {
	TInt err;
	TRAP(err,RProperty::Delete(aPropertyKey));
	if(err == KErrNotFound)
	    err = KErrNone;
	return(err);
    }

/**
Static shutdown function. 
Deletes the properties defined in InitializeL().
*/

EXPORT_C void RAssistanceDataCache::ShutDownL()
    {
    for (TInt i = 0; i < KAssistanceDataListCount; i++)
    	{
		User::LeaveIfError(DeletePropertyL(KLbsAssistDataApiBase + i));
		}
    }
    
/**
Opens an access to the assistance data cache.
*/
EXPORT_C void RAssistanceDataCache::OpenL()
    {
	AttachToPropertiesL();   
    }
    

/**
Called to close the interface and its properties.
*/
EXPORT_C void RAssistanceDataCache::Close()
    {
    for (TInt i = 0; i < KAssistanceDataListCount; i++)
    	{
	    iPropertyList[i].Close();
		}
    }
    
/**
Called to attach to the properties defined by this object
*/
void RAssistanceDataCache::AttachToPropertiesL()
    {
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(KNgAssistanceDataUpdatesCategoryKey, category));
	CleanupStack::PopAndDestroy(rep);
	TUid categoryUid(TUid::Uid(category));
	
    for (TInt i = 0; i < KAssistanceDataListCount; i++)
    	{
	    User::LeaveIfError(iPropertyList[i].Attach(categoryUid, 
                           						   KLbsAssistDataApiBase + i,
                           						   EOwnerThread));
		}
    }

/**
Called by the AGPS module to retrieve a specified type of assistance data. 
@return KErrNone, or an error code returned by the attempt to read from the 
        property.
@param  TLbsAssistanceDataItem the type of assistance data to retrieve
@param  RDataReaderRootBase reference to object which holds the assistance data.
*/
EXPORT_C TInt RAssistanceDataCache::GetAssistDataItem(TLbsAssistanceDataItem aItem,
			                                          RDataReaderRootBase& aDataRoot)
    {
	__ASSERT_DEBUG(aItem != 0, User::Invariant());
	
	TInt err;
	if (aItem  > EAssistanceDataPositioningGpsUtcModel)
		{
		err = KErrArgument;
		}
	else
		{
		TInt adNumber = BitPosToNumber(aItem);
		__ASSERT_DEBUG((adNumber > 0) && (adNumber <= KAssistanceDataListCount), User::Invariant());
	
		TPtr8 ptr(aDataRoot.DataBuffer());
    	err = iPropertyList[adNumber-1].Get(ptr);;
    	}
    	
    return(err);
    }

/**
Called by the Network Gateway to set a specified type of assistance data. 
@return KErrNone, or an error code returned by the attempt to set a 
        property.
@param  TLbsAssistanceDataItem the type of assistance data to retrieve
@param  RDataReaderRootBase reference to object which holds the assistance data.
*/
EXPORT_C TInt RAssistanceDataCache::SetAssistDataItem(TLbsAssistanceDataItem aItem,
			                                          const RDataBuilderRootBase& aDataRoot)
    {
	__ASSERT_DEBUG(aItem != 0, User::Invariant());
	__ASSERT_DEBUG(aItem <= EAssistanceDataPositioningGpsUtcModel, User::Invariant());
	
	TInt adNumber = BitPosToNumber(aItem);
	__ASSERT_DEBUG((adNumber > 0) && (adNumber <= KAssistanceDataListCount), User::Invariant());
	
	TInt err = iPropertyList[adNumber-1].Set(aDataRoot.DataBuffer());
	return(err);
    }

/**
 This method resets the assistance data according to the given mask
 
 @param aDataMask The mask indicating which assistance data to reset
 
 @return KErrNone, or an error code returned when attempting to reset any of the assistance data
 */
EXPORT_C TInt RAssistanceDataCache::ResetAssistanceData(TLbsAsistanceDataGroup aDataMask)
	{
	TLbsAssistanceDataGroup data(aDataMask);
	data &= KAllAssistanceData; // make the mask to cover only the items defined for assistance data
	// using this item to go through all the assistance data items
	TLbsAssistanceDataItem item = EAssistanceDataAquisitionAssistance; // 0x00000001 
	TInt error = KErrNone;
	do
		{ // looping through all the assistance data items, marking the items that have been
		  // reset, until the data doesn't contain any more items not reset
		if((data & item) != 0)
			{ // if the mask contains the current item then set the property to an empty descriptor
			__ASSERT_DEBUG(item != 0, User::Invariant());
			__ASSERT_DEBUG(item <= EAssistanceDataPositioningGpsUtcModel, User::Invariant());
			TInt adNumber = BitPosToNumber(item);
			__ASSERT_DEBUG((adNumber > 0) && (adNumber <= KAssistanceDataListCount), User::Invariant());
			error = iPropertyList[adNumber-1].Set(KNullDesC8);
			
			// mark the item has been reset by making that bit 0 (this is done by anding with the 
			// bitwise negation of the item that has just been reset)
			data &= (~item);
			}
		item = static_cast<TLbsAssistanceDataItem>(KAllAssistanceData & (item << 1)); // go to the next item by shifting left by one
		}
	while(!error && data != 0);
	return error;
	}


/** Utility function.
	Bit Position to Number 1...64
    0 if aItem is 0 */
TUint32 RAssistanceDataCache::BitPosToNumber(TLbsAssistanceDataItem aItem)
	{
	TUint32 item = aItem;
	TInt number = 0;
	
	while(item != 0)
		{
		number++;
		item = item >> 1;
		}

	return number;
	}


TInt RAssistanceDataCache::GetDataSize(TLbsAssistanceDataItem aItem)
	{
	TInt size = 0;
	switch(aItem)
		{
		case EAssistanceDataAquisitionAssistance:
			size = RUEPositioningGpsAcquisitionAssistanceReader::DataSize();
			break;
		case EAssistanceDataBadSatList:
			size = RBadSatListReader::DataSize();
			break;
		case EAssistanceDataNavigationModel:
			size = RUEPositioningGpsNavigationModelReader::DataSize();
			break;
		case EAssistanceDataReferenceTime:
			size = RUEPositioningGpsReferenceTimeReader::DataSize();
			break;
		case EAssistanceDataIonosphericModel:
			size = RUEPositioningGpsIonosphericModelReader::DataSize();
			break;
		case EAssistanceDataDgpsCorrections:
			// Data type not supported yet
			size = 0;
			break;
		case EAssistanceDataReferenceLocation:
			size = RReferenceLocationReader::DataSize();
			break;
		case EAssistanceDataAlmanac:
			size = RUEPositioningGpsAlmanacReader::DataSize();
			break;
		case EAssistanceDataPositioningGpsUtcModel:
			size = RUEPositioningGpsUtcModelReader::DataSize();
			break;
		default:
			//PANIC TBD
			break;
		}
	
	return size;
	}
