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
#include "C32BcaFactory.h"

using namespace BasebandChannelAdaptation;
using namespace BasebandChannelAdaptation::C32Bca;

	

extern "C"
    {
    IMPORT_C MBcaFactory* NewBcaFactoryL();	// Force export
    }

EXPORT_C MBcaFactory* NewBcaFactoryL()
/**
 * Creates a new BCA factory. This must be the first exported function
 * in the BCA.DLL.
 *
 * @return a newly created CC32BcaFactory.
 */
	{
	return new (ELeave) CC32BcaFactory;
	}
	
// end of C32BcaDllMain.cpp
