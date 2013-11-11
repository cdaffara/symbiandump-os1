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
// C32 Baseband Channel Adaptor(BCA) Factory APIs implementation.
// This file contains all the API implementations of C32BcaFactory for Symbian OS.
// 
//

/**
 @file 
 @internalComponent
*/
 #include "C32BcaFactory.h"
 #include "C32Bca.h"
 
 using namespace BasebandChannelAdaptation;
 using namespace BasebandChannelAdaptation::C32Bca;
 
/** Deletes the factory */
void CC32BcaFactory::Release()
	{
	delete this;
	}
 
/**
Factory method: creates a C32BCA instance and returns a pointer to it.

@return a pointer to BCA instance.
@leave if C32 BCA instance could not be constructed.
*/
MBca* CC32BcaFactory:: NewBcaL()
	{
	CC32Bca* instance = new(ELeave) CC32Bca;
	CleanupStack::PushL(instance);
	instance->ConstructL();
	CleanupStack::Pop(instance);
	return instance;
	}



