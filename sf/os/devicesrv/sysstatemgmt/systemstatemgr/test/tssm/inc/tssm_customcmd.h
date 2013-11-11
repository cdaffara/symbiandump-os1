// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#if !defined (__TSSM_CUSTOMCMD_H__)
#define __TSSM_CUSTOMCMD_H__

#include <e32base.h>


_LIT(KSsmCustomCmdTest, "SsmCustomCmdTest");

enum TSsmCustomCmdTestPanic
	{
	ESsmCustomCmdTestPanic1 = 2042,
	ESsmCustomCmdTestPanic2,
	ESsmCustomCmdTestPanic3,
	ESsmCustomCmdTestPanic4
	};

// Forward declare
class MSsmCustomCommand;

/**
Exported test functions.
*/
IMPORT_C TInt TCustomCmdDllFn1(const TDesC8&);
IMPORT_C void TCustomCmdDllFn2L(const TDesC8&);
IMPORT_C TInt TCustomCmdDllFn3(const TDesC8&);
IMPORT_C TInt TCustomCmdDllFn4(const TDesC8&);
IMPORT_C MSsmCustomCommand* TCustomCmdDllFn5L();
IMPORT_C MSsmCustomCommand* TCustomCmdDllFn6L();


#endif
