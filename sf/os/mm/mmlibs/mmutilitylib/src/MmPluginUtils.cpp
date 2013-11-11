
// MmPluginUtils.cpp

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

#include <mm/mmpluginutils.h>
#include <mm/mmcleanup.h>

//
// MmPluginUtils
//

/**
Find a plugin with the requested interfaceUid and whose match string matches
the default data field. Where there are more than one plugin, choose that
with highest version no.
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TInt32 aKeyOffset, 
									                const TDesC8& aDefaultData,
									                TUid aResolverUid)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);

	FindImplementationsL(aInterfaceUid, pluginArray, aDefaultData, aResolverUid);
	
	// Get higher version implementation
	TAny* higherversionplugin = GetHigherVersionImplementationKeyOffsetL(pluginArray,aKeyOffset);
	
	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}

/**
Find a plugin with the requested interfaceUid and whose match string matches
the default data field. Where there are more than one plugin, choose that
with highest version no.
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TInt32 aKeyOffset, 
									                const TDesC8& aDefaultData)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);

	FindImplementationsL(aInterfaceUid, pluginArray, aDefaultData);
	
	// Get higher version implementation
	TAny* higherversionplugin = GetHigherVersionImplementationKeyOffsetL(pluginArray,aKeyOffset);

	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}

/**
Find a plugin with the requested interfaceUid and whose match string matches
the default data field. Where there are more than one plugin, choose that
with highest version no.
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TInt32 aKeyOffset)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);

	FindImplementationsL(aInterfaceUid, pluginArray);
	
	// Get higher version implementation
	TAny* higherversionplugin = GetHigherVersionImplementationKeyOffsetL(pluginArray,aKeyOffset);

	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}
	
	
/**
Get the higher version of plugin passing keyoffset
*/
TAny* MmPluginUtils::GetHigherVersionImplementationKeyOffsetL(RImplInfoPtrArray& pluginArray, TInt32 aKeyOffset)
	{
	TAny* self = NULL;
	
	if (pluginArray.Count() == 0)
		{
		User::Leave(KErrNotSupported);
		}
	else if (pluginArray.Count() == 1)
    	{
		self = REComSession::CreateImplementationL(pluginArray[0]->ImplementationUid(), aKeyOffset);
    	}
	else 
		{
		// create each in turn, starting with the highest version, which is the start element
		for (TInt index = 0;  index < pluginArray.Count(); index++)
			{
			TInt err=KErrNone;
			self = NULL;
			TRAP(err, self = REComSession::CreateImplementationL(pluginArray[index]->ImplementationUid(), aKeyOffset))
			if (err == KErrNone)
		    	{
		    	break;
		    	}
			if (err != KErrNotSupported)
				{
				User::Leave(err);
				}
			}
		if (!self)
			{
			User::Leave(KErrNotSupported);
			}
		}
	return self;		
	}

/*
Similar but return destructor key via reference
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TUid& aDestructorKey,
									                const TDesC8& aDefaultData,
									                TUid aResolverUid)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	
	FindImplementationsL(aInterfaceUid, pluginArray, aDefaultData, aResolverUid);
    
    // Get higher version implementation
    TAny* higherversionplugin = GetHigherVersionImplementationaDestructorKeyL(pluginArray, aDestructorKey);
	
	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}

/*
Similar but return destructor key via reference
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TUid& aDestructorKey,
									                const TDesC8& aDefaultData)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	
	FindImplementationsL(aInterfaceUid, pluginArray, aDefaultData);
	
	// Get higher version implementation
    TAny* higherversionplugin = GetHigherVersionImplementationaDestructorKeyL(pluginArray, aDestructorKey);

	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}

/*
Similar but return destructor key via reference
*/
EXPORT_C TAny* MmPluginUtils::CreateImplementationL(TUid aInterfaceUid, 
									                TUid& aDestructorKey)
	{
	RImplInfoPtrArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	
	FindImplementationsL(aInterfaceUid, pluginArray);
	
	// Get higher version implementation
    TAny* higherversionplugin = GetHigherVersionImplementationaDestructorKeyL(pluginArray, aDestructorKey);

	CleanupStack::PopAndDestroy(&pluginArray);
	return higherversionplugin;	
	}
	
/**
Get the higher version of plugin passing destructorkey
*/
TAny* MmPluginUtils::GetHigherVersionImplementationaDestructorKeyL(RImplInfoPtrArray& pluginArray, TUid& aDestructorKey)
	{
	TAny* self = NULL;
	if (pluginArray.Count() == 0)
		{
		User::Leave(KErrNotSupported);
		}
	else if (pluginArray.Count() == 1)
    	{
		self = REComSession::CreateImplementationL(pluginArray[0]->ImplementationUid(), aDestructorKey);
    	}
	else 
		{
		// create each in turn, starting with the highest version, which is the start element
		for (TInt index = 0;  index < pluginArray.Count(); index++)
			{
			TInt err = KErrNone;
			self = NULL;
			TRAP(err, self = REComSession::CreateImplementationL(pluginArray[index]->ImplementationUid(), aDestructorKey))
		    if (err == KErrNone)
		    	{
		    	break;
		    	}
			if (err != KErrNotSupported)
				{
				User::Leave(err);
				}
			}
		if (!self)
			{
			User::Leave(KErrNotSupported);
			}
		}
	return self;		
	}

/*
The function is an implementation of the templated TLinearOrder so it can order implementation information
in an increasing version number.
*/
TInt VersionLinearOrderFunction(const CImplementationInformation& pluginA, 
                                const CImplementationInformation& pluginB)
	{
	if (pluginA.Version() == pluginB.Version())
		{
		return 0;
		}
		
	return (pluginA.Version() < pluginB.Version())? 1: -1;	
	}
	
EXPORT_C void MmPluginUtils::FindImplementationsL(TUid aInterfaceUid, 
						                RImplInfoPtrArray& aPluginArray)
	{
	REComSession::ListImplementationsL(aInterfaceUid, aPluginArray);
	
	CheckAndSortL(aPluginArray);
	}

EXPORT_C void MmPluginUtils::FindImplementationsL(TUid aInterfaceUid, 
						                RImplInfoPtrArray& aPluginArray, 
				                        const TDesC8& aDefaultData,
						                TUid aResolverUid)
	{
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aDefaultData);
	
	FindImplementationsL(aInterfaceUid, aPluginArray, resolverParams, aResolverUid);
	}

EXPORT_C void MmPluginUtils::FindImplementationsL(TUid aInterfaceUid, 
						                RImplInfoPtrArray& aPluginArray, 
				                        const TDesC8& aDefaultData)
	{
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aDefaultData);
	
	FindImplementationsL(aInterfaceUid, aPluginArray, resolverParams);
	}

void MmPluginUtils::FindImplementationsL(TUid aInterfaceUid, 
						                RImplInfoPtrArray& aPluginArray, 
				                        TEComResolverParams& aResolverParams)
	{
	REComSession::ListImplementationsL(aInterfaceUid, aResolverParams, aPluginArray);
	
	CheckAndSortL(aPluginArray);
	}
	
void MmPluginUtils::FindImplementationsL(TUid aInterfaceUid, 
						                RImplInfoPtrArray& aPluginArray, 
				                        TEComResolverParams& aResolverParams,
						                TUid aResolverUid)
	{
	REComSession::ListImplementationsL(aInterfaceUid, aResolverParams, aResolverUid, aPluginArray);
	
	CheckAndSortL(aPluginArray);
	}
	
void MmPluginUtils::CheckAndSortL(RImplInfoPtrArray& aPluginArray)
	{
	// there must be at least one implementation
	if (aPluginArray.Count() > 0)
		{
		if (aPluginArray.Count() > 1)
			{
			aPluginArray.Sort(VersionLinearOrderFunction);
			}
	
		// if the highest version is zero there is no need to linger -- it should not be supported
		// and we will leave
		if (aPluginArray[0]->Version() == 0)
			{
			// plugins were found to be malformed, with 0 version numbers
			User::Leave(KErrNotSupported);
			}
		}
	}
