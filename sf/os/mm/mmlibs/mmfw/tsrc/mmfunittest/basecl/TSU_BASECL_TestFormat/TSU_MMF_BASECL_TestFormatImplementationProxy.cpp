// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Entry point for the ECom dll only
// 
//

/**
 @file TSU_MMF_BASECL_TestFormatImplementationProxy.cpp
*/

#include "TSU_MMF_BASECL_TestFormat.h"
#include <ecom/implementationproxy.h>
#include "TSU_MMF_BASECL_TestFormatImplementationUID.hrh"



// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs



const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(  KMmfUidTSUBASECLTestFormatDecode,	CMMF_TSU_BASECL_TestFormatDecode::NewL),
	IMPLEMENTATION_PROXY_ENTRY(  KMmfUidTSUBASECLTestFormatEncode,	CMMF_TSU_BASECL_TestFormatEncode::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
