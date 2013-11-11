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
// A dummy test to workaround DEF059510 for the first test case failure.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32debug.h>

GLDEF_C TInt E32Main()
	{	
	// debug statement for DABS script to recognise that the test has completed successfully
	RDebug::Print(_L("RTEST: SUCCESS : Dummy Test completed"));
	return KErrNone;
    }



