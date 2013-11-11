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


#include <centralrepository.h>
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include <lbscommon.h>
#include "LbsInternalCommon.h"
#include "LbsExtendModuleInfo.h"
 
//We only allow TPositionModuleInfo objects

/** Descriptor package of the module info data */
typedef TPckg<TPositionModuleInfoExtended> TPositionModuleInfoExtendedPckg;
/** Descriptor package of the module info data */
typedef TPckgC<TPositionModuleInfoExtended> TPositionModuleInfoExtendedPckgC;

/** Module info count key start offset in RProperty object, stick the count in as the first entry. */
const TUint KLbsModuleInfoCountKeyOffset = KLbsModuleInfoKeyBase;	
/** Module info data key start offset in RProperty object */
const TUint KLbsModuleInfoDataStartKeyOffset = KLbsModuleInfoKeyBase+1;


/** Initialize the module info data. Check the security of the process, set the available keys to 
RProperty object, both the key for info data and count. The secure ID of the property is set to 
the initialization process secure ID.

@param aModuleInfoPtrArray A pointer array of TPositionModuleInfoExtended object
@param aModuleInfoCount The module info count
@see TPositionModuleInfoBase
@released
@internalComponent
 */
 EXPORT_C void LbsModuleInfo::InitializeL(const TPositionModuleInfoExtended* aModuleExtendedInfoPtrArray[], TInt aModuleInfoCount)
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	//First, set the data...
	for(TInt i=aModuleInfoCount-1; i>=0; i--)
		{
		const TPositionModuleInfoExtended* moduleExtendedInfo = aModuleExtendedInfoPtrArray[i];
		__ASSERT_DEBUG(moduleExtendedInfo, User::Invariant());	//To do - get a proper panic code.
		
		const TUint key = KLbsModuleInfoDataStartKeyOffset+i;
		TInt err = RProperty::Define(key, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TPositionModuleInfo));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		
		TPositionModuleInfoExtendedPckgC pckg(*moduleExtendedInfo);
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), key, pckg));
		}
	
	//Next, set the count.
	TInt err = RProperty::Define(KLbsModuleInfoCountKeyOffset, RProperty::EInt, KReadPolicy, KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), KLbsModuleInfoCountKeyOffset, aModuleInfoCount));
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KModuleInfoCategoryKey, RProcess().SecureId());
	}

/** Delete all keys defined in RProperty object.

@released
@internalComponent
 */
EXPORT_C void LbsModuleInfo::ShutDownL()
	{
	TInt err;
	TUint moduleInfoCount;
	err = GetNumModules(moduleInfoCount);
	if(err)
		{
		if(err == KErrNotFound)
			{
			return; // not a problem; it's just not there.
			}
		User::Leave(err);
		}
	 
	for(TInt i=moduleInfoCount-1; i>=0; i--)
		{
		const TUint key = KLbsModuleInfoDataStartKeyOffset+i;
		User::LeaveIfError(RProperty::Delete(key));
		}
		
	User::LeaveIfError(RProperty::Delete(KLbsModuleInfoCountKeyOffset));
	}

/** Get the number of modules by module count key.

@param aNumModules Return the number of modules.
@param aInitializationProcessUid The initialization process UID.
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt LbsModuleInfo::GetNumModules(TUint& aNumModules)
	{
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KModuleInfoCategoryKey, categoryUid);
	if (err == KErrNone)
		{
		err = RProperty::Get(categoryUid, 
							 KLbsModuleInfoCountKeyOffset, 
							 reinterpret_cast<TInt&>(aNumModules));
		}
		
	return err;
	}
	
/** Get module info data by module index.
Only may be used to get the TPositionModuleInfo

@param aModuleIndex Module index.
@param aBlob TAny* pointer to TPositionModuleInfo object.
@param aBlobSize size , in bytes, of blob.
@param aInitializationProcessUid The initialization process UID.
@see TPositionModuleInfoBase
@return Symbian Location service Error code
@released
@internalComponent
 */
EXPORT_C TInt LbsModuleInfo::GetModuleInfoByIndex(TInt aModuleIndex, TAny* aBlob,TInt aBlobSize)
	{
	if (sizeof(TPositionModuleInfo) != aBlobSize)
		{
		return KErrNotSupported;
		}

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KModuleInfoCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		
	TUint numModules;
	err = GetNumModules(numModules);
	if(err!=KErrNone)
		{
		return err;
		}
	
	if( (aModuleIndex < 0) || (aModuleIndex>=numModules) )
		{
		return KErrNotFound ;
		}
	TPositionModuleInfoExtended holding;
	TPositionModuleInfoExtendedPckg pckg(static_cast<TPositionModuleInfoExtended&>(holding));
	
	err = RProperty::Get(categoryUid, KLbsModuleInfoDataStartKeyOffset+aModuleIndex, pckg);
	if(err!=KErrNone)
		{
		return err;
		}

	// copy position module information
	TUint8* endAddr2 = Mem::Copy(aBlob, holding.iPositionModuleInfoBytes, aBlobSize);
	
	return KErrNone;

	}

/** Get module info data by module ID.
Only may be used to get the TPositionModuleInfo

@param aModuleId Position module ID.
@param aBlob TAny* pointer to TPositionModuleInfo object.
@param aBlobSize size , in bytes, of blob.
@param aInitializationProcessUid The initialization process UID.
@see TPositionModuleInfoBase
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt LbsModuleInfo::GetModuleInfoById(TPositionModuleId aModuleId,
											TAny* aBlob, TInt aBlobSize)
	{
	if (sizeof(TPositionModuleInfo) != aBlobSize)
		{
		return KErrNotSupported;
		}

	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KModuleInfoCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		

	TUint numModules;
	err = GetNumModules(numModules);
	if(err!=KErrNone)
		{
		return err;
		}
	
	TPositionModuleInfoExtended holdingInfo;	//Used for getting the data out of P&S.
	TPositionModuleInfoExtendedPckg pckg(holdingInfo);
	for(TInt i=numModules-1; i>=0; i--)
		{
		TInt err = RProperty::Get(categoryUid, KLbsModuleInfoDataStartKeyOffset+i, pckg);
		if(err!=KErrNone)
			{
			return err;		//Something strange going on - should never happen.
			}
			

		if(holdingInfo.iModuleId==aModuleId)
			{
			TUint8* endAddr2 = Mem::Copy(aBlob,holdingInfo.iPositionModuleInfoBytes, aBlobSize);
			//static_cast<TPositionModuleInfo&>(aModuleInfo)=holdingInfo.iPositionModuleInfo;
			return KErrNone;			
			}
		}
	
	return KErrNotFound;
	}

/** Get the Id of the data source plugin.

@param aModuleId Position module ID.
@param aDataSourceId a reference to TUid for the result
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt LbsModuleInfo::GetDataSourceId(TPositionModuleId aModuleId, TUid& aDataSourceId)
	{
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KModuleInfoCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		
	TUint numModules;
	err = GetNumModules(numModules);
	if(err!=KErrNone)
		{
		return err;
		}

	TPositionModuleInfoExtended holdingInfo;	//Used for getting the data out of P&S.
	TPositionModuleInfoExtendedPckg pckg(holdingInfo);

	for(TInt i=numModules-1; i>=0; i--)
		{
		TInt err = RProperty::Get(categoryUid, KLbsModuleInfoDataStartKeyOffset+i, pckg);
		if(err!=KErrNone)
			{
			return err;		//Something strange going on - should never happen.
			}
		if(holdingInfo.iModuleId==aModuleId)
			{
			aDataSourceId = holdingInfo.iDataSourcePluginId;
			return KErrNone;	
			}
		}
	
	return KErrNotFound;
	}


/** Get the Id of the data source plugin.

@param aModuleId Position module ID.
@param aDataSourceId a reference to TUid for the result
@return Symbian Location service Error code
@released
@internalComponent
 */

EXPORT_C TInt LbsModuleInfo::GetDeviceCapabilities(TPositionModuleId aModuleId, TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aDeviceCapabilities)
	{
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt err = LbsInternalCommon::GetCategoryUid(KModuleInfoCategoryKey, categoryUid);
	if (err != KErrNone)
		{
		return err;
		}
		
	TUint numModules;
	err = GetNumModules(numModules);
	if(err!=KErrNone)
		{
		return err;
		}

	TPositionModuleInfoExtended holdingInfo;	//Used for getting the data out of P&S.
	TPositionModuleInfoExtendedPckg pckg(holdingInfo);

	for(TInt i=numModules-1; i>=0; i--)
		{
		TInt err = RProperty::Get(categoryUid, KLbsModuleInfoDataStartKeyOffset+i, pckg);
		if(err!=KErrNone)
			{
			return err;		//Something strange going on - should never happen.
			}
		if(holdingInfo.iModuleId==aModuleId)
			{
			aDeviceCapabilities = holdingInfo.iDeviceGpsModeCapabilities;
			return KErrNone;	
			}
		}
	
	return KErrNotFound;
	}
