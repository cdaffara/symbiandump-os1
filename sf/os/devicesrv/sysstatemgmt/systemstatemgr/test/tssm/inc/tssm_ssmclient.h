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
 

#ifndef __TSSM_SSMCLIENT_H__
#define __TSSM_SSMCLIENT_H__

#include <ssm/ssmstatemanager.h>
#include "ssmclisrv.h"


_LIT(KSsmServerNameTest, "SysStateMgrTest");


/**
RSsmStateManagerTestClient is a utility class that allows the test to connect to a clone of the SsmServer, 
but with a different name. That's why we need to a derived class to be able to provide a different server
name

NOTES:
 	1.) To inherit from RSsmStateManager, we need to include the RSsmStateManager production code because that class 
		is declared with NONSHARABLE_CLASS which means its size is not exported outside the DLL. 
		(and neither is DoConnect).
	2.) We also need to make sure ssmclisrv.h is available to the policy mmp
	3.) Connect() is declared 'protected' in RSsmStateManager for this purpose.
*/
class RSsmStateManagerTestClient : public RSsmStateManager
	{
public:
	IMPORT_C TInt Connect(const TDesC& aServerName, const TVersion& aVersion);
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
public:
	IMPORT_C TInt HeapMark();
	IMPORT_C TInt HeapMarkEnd();
	IMPORT_C TInt HeapMarkEnd(TInt aDoCleanup);
	IMPORT_C TInt RequestDeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename);
	};


#endif
