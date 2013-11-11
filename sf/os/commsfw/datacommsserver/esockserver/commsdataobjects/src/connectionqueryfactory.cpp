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


#include "connectionqueryfactory.h"
#include <comms-infras/connectionqueryset.h>


using namespace ESock;

// ---------------- Factory Methods ----------------
/**
Creates an instance of a connection query based on the supplied type 
@param		aConstructionParams A TUint identifying the type of connection query to create
@return		A pointer to the instance of a connection query
@exception	Leaves with KErrNoMemory if the object cannot be created, or KErrUnknown
			if unknown query type id is given
*/
XConnectionQueryBase* CConnectionQueryFactory::NewL(TAny* aConstructionParams)
    {
    TUint typeId = reinterpret_cast<TUint>(aConstructionParams);
    switch (typeId)
    	{
    	case XConnectionQueryBase::ENull:
        	return new (ELeave) XConnectionQuerySet::XNullQuery();
        	//break;

    	case XConnectionQueryBase::EBool:
        	return new (ELeave) XBoolQuery();
        	//break;

    	case XConnectionQueryBase::EInt:
        	return new (ELeave) XIntQuery();
        	//break;

    	case XConnectionQueryBase::EUint:
        	return new (ELeave) XUintQuery();
        	//break;

    	case XConnectionQueryBase::EText8:
        	return new (ELeave) XText8Query();
        	//break;

    	case XConnectionQueryBase::EText16:
        	return new (ELeave) XText16Query();
        	//break;
        }

	User::Leave(KErrUnknown);
    return NULL;
    }

