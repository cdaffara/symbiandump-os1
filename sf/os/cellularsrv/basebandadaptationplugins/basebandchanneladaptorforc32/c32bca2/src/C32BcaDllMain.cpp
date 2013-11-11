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
//

/**
 @file 
 @internalComponent
*/
#include "MBca2Impl.h"

using namespace BasebandChannelAdaptation2;
using namespace BasebandChannelAdaptation2::C32BcaShim;

	

extern "C"
    {
    IMPORT_C MBca2Factory* NewBcaFactoryL();	// Force export
    }

EXPORT_C MBca2Factory* NewBcaFactoryL()
/**
 * Creates a new BCA factory. This must be the first exported function
 * in the BCA.DLL.
 *
 * @return a newly created CC32BcaFactory.
 */
	{
	return new (ELeave) MBca2FactoryImpl;
	}
	
// end of C32BcaDllMain.cpp
