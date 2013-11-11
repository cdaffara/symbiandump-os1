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

#include <e32base.h>
#include <lbsadmindatatypes.h>

//******************************************************************************************************************
// TLbsAdminInfoBase
//******************************************************************************************************************
/** Default constructor for TLbsAdminInfoBase */
EXPORT_C TLbsAdminInfoBase::TLbsAdminInfoBase()
:	iClassType(0),
	iClassSize(0)
	{
	}

/**  Returns the type of the TLbsAdminInfoBase-derived class */
EXPORT_C TUint32 TLbsAdminInfoBase::ClassType() const
	{
	return iClassType;
	}
	
/** Returns the size of the TLbsAdminInfoBase-derived class */
EXPORT_C TUint TLbsAdminInfoBase::ClassSize() const
	{
	return iClassSize;
	}
	

//******************************************************************************************************************
// TLbsAdminProtocolModulesInfo
//******************************************************************************************************************
/** Default constructor for TLbsAdminProtocolModulesInfo */
EXPORT_C TLbsAdminProtocolModulesInfo::TLbsAdminProtocolModulesInfo():
	iModuleIdArrayCount(0)
	{
	iClassType = ELbsAdminProtocolModulesInfoClass;
	iClassSize = sizeof(TLbsAdminProtocolModulesInfo);
	}

/** Returns the Id of the default module. 

The default module Id can also be obtained by calling GetModuleIds(). It is the first module in the array.

The default module is used for both, incoming (MTLR, NILR) 
and outgoing (MOLR, MOLR-X3P, MOLR-CellBased) requests.

@return An id of the default module.

@see GetModuleIds
*/
EXPORT_C TLbsProtocolModuleId TLbsAdminProtocolModulesInfo::DefaultModuleId() const
	{
	return iModuleIdArray[0];
	}

/** Gets an array of Protocol Module Ids

The first Protocol Module in the array is used for both
incoming (MTLR, NILR) and outgoing (MOLR, MOLR-X3P, MOLR-CellBased) requests.
The additional protocol modules are used for incoming requests only.

The default module Id can also be obtained by calling DefaultModuleId().

@param aModuleIdArray [Out] An array of Protocol Module Ids.
@param aCount [Out] Number of items in the array.

@see DefaultModuleId
*/
EXPORT_C void TLbsAdminProtocolModulesInfo::GetModuleIds(TLbsProtocolModuleIdArray& aModuleIdArray, TInt& aCount) const
	{
	aCount = iModuleIdArrayCount;
	for (TInt i = 0; i < aCount; i++)
		{
		aModuleIdArray[i] = iModuleIdArray[i];
		}
	}


/** Sets an array of Protocol Module Ids

The first Protocol Module in the array is used for both
incoming (MTLR, NILR) and outgoing (MOLR, MOLR-X3P, MOLR-CellBased) requests.
The additional protocol modules are used for incoming requests only.

@param aModuleIdArray [In] An array of Protocol Module Ids.
@param aCount [In] Number of items in the array.

@return KErrNone if successful; 
        KErrArgument if the number of items in the array exceeds the maximum of KLbsAdminMaxProtocolModuleIds.

@see KLbsAdminMaxProtocolModuleIds
*/
EXPORT_C TInt TLbsAdminProtocolModulesInfo::SetModuleIds(const TLbsProtocolModuleIdArray& aModuleIdArray, TInt aCount)
	{
	if (aCount > KLbsAdminMaxProtocolModuleIds)
		{
		return KErrArgument;
		}
	
	iModuleIdArrayCount = aCount;
	for (TInt i = 0; i < iModuleIdArrayCount; i++)
		{
		iModuleIdArray[i] = aModuleIdArray[i];
		}
	
	return KErrNone;
	}


//******************************************************************************************************************
// TLbsAdminExtLocService
//******************************************************************************************************************
/** Default constructor for TLbsAdminExtLocService */
EXPORT_C TLbsAdminExtLocService::TLbsAdminExtLocService():
	iModuleService(EExternalLocateUnknown),
	iModuleId(KLbsProtocolNullModuleId)
	{
	}

/** Returns the External Locate Service setting of a Protocol Module.

@return External Locate Service setting of the Protocol Module.
*/
EXPORT_C TLbsAdminExtLocService::TExternalLocateService TLbsAdminExtLocService::ModuleService() const
	{
	return iModuleService;
	}

/** Sets the External Locate Service setting of the Protocol Module.

@param aModuleService [In] External Locate Service setting of the Protocol Module.
*/
EXPORT_C void TLbsAdminExtLocService::SetModuleService(const TExternalLocateService aModuleService)
	{
	iModuleService = aModuleService;
	}

/** Returns the Protocol Module unique Id.

@return Protocol Module unique Id.
*/
EXPORT_C TLbsProtocolModuleId TLbsAdminExtLocService::ModuleId() const
	{
	return iModuleId;
	}

/** Sets the Protocol Module unique Id.

@param aModuleId [In] Module unique Id.
*/
EXPORT_C void TLbsAdminExtLocService::SetModuleId(const TLbsProtocolModuleId aModuleId)
	{
	iModuleId = aModuleId;
	}


//******************************************************************************************************************
// TLbsAdminExtLocServicesInfo
//******************************************************************************************************************
/** Default constructor for TLbsAdminExtLocServicesInfo */
EXPORT_C TLbsAdminExtLocServicesInfo::TLbsAdminExtLocServicesInfo():
	iServiceArrayCount(0)
	{
	iClassType = ELbsAdminExtLocServicesInfoClass;
	iClassSize = sizeof(TLbsAdminExtLocServicesInfo);
	}

/** Returns the default value for the External Locate Service setting.

The default setting is used by the LBS sub-system only if 
a Protocol Module does not have a specific setting defined

@return Default External Locate Service setting.

@see GetModuleServices
*/
EXPORT_C TLbsAdminExtLocService::TExternalLocateService TLbsAdminExtLocServicesInfo::DefaultService() const
	{
	return TLbsAdminExtLocService::EExternalLocateUnknown;
	}

/** Sets the default value for the External Locate Service setting.

The default setting is used by the LBS sub-system only if 
a Protocol Module does not have a specific setting defined

@param aModuleService [In] Default External Locate Service setting.

@see SetModuleServices
*/
EXPORT_C void TLbsAdminExtLocServicesInfo::SetDefaultService(
					const TLbsAdminExtLocService::TExternalLocateService& /*aModuleService*/)
	{
	}

/** Gets a Protocol Module specific External Locate Service setting.

The Protocol Module specific settings supersede the default setting.

@param aModuleId [In] An id of the Protocol Module.
@param aModuleService [Out] The value of the Protocol Module specific setting.

@return KErrNone if successful; 
        KErrNotFound if there is no setting defined for a Protocol Module of the specified Id.


@see DefaultService
@see SetDefaultService
*/
EXPORT_C TInt TLbsAdminExtLocServicesInfo::GetModuleService(
					const TLbsProtocolModuleId& aModuleId, 
					TLbsAdminExtLocService::TExternalLocateService& aModuleService) const
	{
	for (TInt i = 0; i < iServiceArrayCount; i++)
		{
		if (iServiceArray[i].ModuleId() == aModuleId)
			{
			aModuleService = iServiceArray[i].ModuleService();
			return KErrNone;
			}
		}
	
	return KErrNotFound;
	}

/** Gets an array of Protocol Module specific External Locate Service settings.

The Protocol Module specific settings supersede the default setting.

@param aModuleServices [Out] An array of the Protocol Module pecific settings.
@param aCount [Out] Number of items in the array
*/
EXPORT_C void TLbsAdminExtLocServicesInfo::GetModuleServices(
					TLbsAdminExtLocServiceArray& aModuleServices, 
					TInt& aCount) const
	{
	aCount = iServiceArrayCount;
	for (TInt i = 0; i < aCount; i++)
		{
		aModuleServices[i] = iServiceArray[i];
		}
	}

/** Sets an array of Protocol Module specific External Locate Service settings.

The Protocol Module specific settings supersede the default setting.

@param aModuleServices [In] An array of the Protocol Module pecific settings.
@param aCount [In] Number of items in the array

@return KErrNone if successful; 
        KErrArgument if the number of items in the array exceeds the maximum of KLbsAdminMaxProtocolModuleIds.
*/
EXPORT_C TInt TLbsAdminExtLocServicesInfo::SetModuleServices(
					const TLbsAdminExtLocServiceArray& aModuleServices, 
					TInt aCount)
	{
	if (aCount > KLbsAdminMaxProtocolModuleIds)
		{
		return KErrArgument;
		}
	
	iServiceArrayCount = aCount;
	for (TInt i = 0; i < iServiceArrayCount; i++)
		{
		iServiceArray[i] = aModuleServices[i];
		}
	
	return KErrNone;
	}


//******************************************************************************************************************
// TLbsAdminProtocolModuleLoadingInfo
//******************************************************************************************************************
/** Default constructor for TLbsAdminProtocolModuleLoadingInfo */
EXPORT_C TLbsAdminProtocolModuleLoadingInfo::TLbsAdminProtocolModuleLoadingInfo():
	iLoadingStrategy(EProtocolModuleLoadingUnknown)
	{
	iClassType = ELbsAdminProtocolModuleLoadingInfoClass;
	iClassSize = sizeof(TLbsAdminProtocolModuleLoadingInfo);
	}

/** Returns the Protocol Module Loading Strategy setting.

@return The the Protocol Module Loading Strategy setting.
*/
EXPORT_C TLbsAdminProtocolModuleLoadingInfo::TProtocolModuleLoading TLbsAdminProtocolModuleLoadingInfo::LoadingStrategy() const
	{
	return iLoadingStrategy;
	}

/** Sets the Protocol Module Loading Strategy setting.

@param aLoadingStrategy [In] The Protocol Module Loading Strategy setting.
*/
EXPORT_C void TLbsAdminProtocolModuleLoadingInfo::SetLoadingStrategy(const TProtocolModuleLoading aLoadingStrategy)
	{
	iLoadingStrategy = aLoadingStrategy;
	}
