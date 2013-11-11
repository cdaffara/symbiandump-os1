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
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#if !defined (__T_DLL_H__)
#define __T_DLL_H__

#include <e32base.h>
#include "appfwk_sysstart_test_consts.h"

/**
Struct that contains the results obtained when each function
is called.
*/
typedef struct TTestDllResultsTag 
	{
	TInt16 count; // number of times function is called
	TUint arg1;   // value of arg1 passed in
	TUint arg2;   // value of arg2 passed in
	TUint arg3;   // value of arg3 passed in
	} TTestDllResults;

/**
Exported test functions.
*/
IMPORT_C TInt TDllFn1(const TDesC8&);
IMPORT_C TInt TDllFn2(const TDesC8&);
IMPORT_C TInt TDllFn3(const TDesC8&);
IMPORT_C TInt TDllFn4(const TDesC8&);
IMPORT_C TInt TDllFn5(const TDesC8&);
IMPORT_C TInt TDllFn6(const TDesC8&);
IMPORT_C TInt TDllFn7(const TDesC8&);
IMPORT_C TInt TDllFn8(const TDesC8&);
IMPORT_C TInt TDllFn9(const TDesC8&);
IMPORT_C TInt TDllFn10(const TDesC8&);
IMPORT_C TInt TDllFn11(const TDesC8&);
IMPORT_C TInt TDllFn12(const TDesC8&);
IMPORT_C TInt TDllFn13(const TDesC8&);
IMPORT_C TInt TDllFn14(const TDesC8&);
IMPORT_C TInt TDllFn15(const TDesC8&);
IMPORT_C TInt TDllFn16(const TDesC8&);
IMPORT_C TInt TDllFn17(const TDesC8&);
IMPORT_C TInt TDllFn18(const TDesC8&);

#endif
