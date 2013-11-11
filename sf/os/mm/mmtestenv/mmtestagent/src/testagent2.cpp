// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// testagent.cpp
// 
//

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "testagentfactory2.h"

/* Implementation table is required by ECom. Allows alternative
	New methods to be specified.
*/
const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101F7C25, CTestAgentFactory::NewL)
	}; 

/* Lookup method required by ECom. Returns the above table to the 
	ECom framework 
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


