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

#if !defined (__APPFWK_SYSSTART_T_APPUTILS_H__)
#define __APPFWK_SYSSTART_T_APPUTILS_H__

#include <e32base.h>

/**
Struct that contains the results obtained when each function
is called.
*/
typedef struct TAppfwkSysstartTestAppResultsTag 
	{
	TInt iStartCount;     // number of times app started
	TInt iRendCount;      // number of times rendezvous called
	TInt iRendFailCount;  // number of times rendezvous failed
	} TAppfwkSysstartTestAppResults;

class CAppfwkSysstartTestAppUtils
	{
	public:
	IMPORT_C static void ReadTestAppResultsFileL(const TDesC&, TAppfwkSysstartTestAppResults&);
	IMPORT_C static void InitTestAppResults(TAppfwkSysstartTestAppResults& aTestAppResults);
	IMPORT_C static void ReplaceTestAppResultsFileL(const TDesC&, const TAppfwkSysstartTestAppResults&);
	
	private:
	CAppfwkSysstartTestAppUtils() {}
	};

#endif
