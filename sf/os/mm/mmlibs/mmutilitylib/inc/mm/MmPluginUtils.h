
// MmPluginUtils.h

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

/**
 @internalTechnology
 @file
*/

#ifndef MMPLUGINUTILS_H
#define MMPLUGINUTILS_H

#include <ecom/ecom.h>

class MmPluginUtils
	{
public:
	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TInt32 aKeyOffset, 
									    		const TDesC8& aDefaultData,
									   			TUid aResolverUid);
									   			
	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TInt32 aKeyOffset, 
									    		const TDesC8& aDefaultData);
									    		
	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TInt32 aKeyOffset);
									   		    								    		
									   			
	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TUid &aDestructorKey, 
									    		const TDesC8& aDefaultData,
									   			TUid aResolverUid);
	
	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TUid& aDestructorKey, 
									    		const TDesC8& aDefaultData);									   			

	IMPORT_C static TAny* CreateImplementationL(TUid aInterfaceUid, 
									   		    TUid& aDestructorKey);
									   		    
	IMPORT_C static void FindImplementationsL(TUid aInterfaceUid, 
									          RImplInfoPtrArray& aPluginArray);
	IMPORT_C static void FindImplementationsL(TUid aInterfaceUid, 
									          RImplInfoPtrArray& aPluginArray, 
									          const TDesC8& aDefaultData);
	IMPORT_C static void FindImplementationsL(TUid aInterfaceUid, 
									          RImplInfoPtrArray& aPluginArray, 
									          const TDesC8& aDefaultData,
									          TUid aResolverUid);
private:
	static void FindImplementationsL(TUid aInterfaceUid, 
		                             RImplInfoPtrArray& aPluginArray, 
                                     TEComResolverParams& aResolverParams,
		                             TUid aResolverUid);
	static void FindImplementationsL(TUid aInterfaceUid, 
		                             RImplInfoPtrArray& aPluginArray, 
                                     TEComResolverParams& aResolverParams);
	static void CheckAndSortL(RImplInfoPtrArray& aPluginArray);
	
	static TAny* GetHigherVersionImplementationKeyOffsetL(RImplInfoPtrArray& pluginArray, TInt32 aKeyOffset);
	static TAny* GetHigherVersionImplementationaDestructorKeyL(RImplInfoPtrArray& pluginArray, TUid& aDestructorKey);
	
	};

#endif // MMPLUGINUTILS_H
