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
// ECOM Factory for Commdb Query Sets
// 
//

/**
 @file
 @internalComponent
*/


#include "commsdataobjectfactory.h"
#include <comms-infras/connectionqueryset.h>
#include <comms-infras/provinfoqueryset.h>

using namespace ESock;

// ---------------- Factory Methods ----------------


/**
Creates an instance of a comms data object based on the supplied type 
@param aConstructionParams A TUint identifying the type of data object to create
@return A pointer to the instance of a data object
*/
XCommsDataObject* CCommsDataObjectFactory::NewL(TAny* aConstructionParams)
    {
    TUint typeId = reinterpret_cast<TUint>(aConstructionParams);
    switch (typeId)
    	{
    	case XConnectionQuerySet::iUid:
        	return new (ELeave) XConnectionQuerySet();
        	//break;
        	
    	case XProviderInfoQuerySet::iUid:
        	return new (ELeave) XProviderInfoQuerySet();
        	//break;
    	case XTunnelConnQuerySet::iUid:
        	return new (ELeave) XTunnelConnQuerySet();
        	//break;
        }

    return NULL;
    }

