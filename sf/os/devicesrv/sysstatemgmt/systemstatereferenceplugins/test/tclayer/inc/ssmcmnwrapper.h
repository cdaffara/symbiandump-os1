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
// Contains wrapper helper functions for test code to control the test wrappers
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef SSMCMNWRAPPER_H_
#define SSMCMNWRAPPER_H_

#include <ssm/ssmstatemanager.h>

class SsmCmnWrapper
{
public:
	static void SetSsmStateManagerNextConnectReturn(TInt aRet);
	static TInt SsmStateManagerNextConnectReturn();
	
	static void SetSsmStateManagerHandle(TInt aHandle);
	static TInt SsmStateManagerHandle();
	
	static TInt GetSsmStateManagerNextStateRequestReturn();
	static void SetSsmStateManagerNextStateRequestReturn(TInt aRet);
	static void SetLastStateTransition(TSsmStateTransition& aTransition);
	
	static TUint16 GetLastMainStateRequest();
	static TUint16 GetLastSubStateRequest();
	static TInt GetLastReasonRequest();
	
	static TInt GetSsmStateManagerNextSwpRequestReturn();
	static void SetSsmStateManagerNextSwpRequestReturn(TInt aRet);
	static void SetLastSwpTransition(TSsmSwp& aSwp);
	static TSsmSwp GetLastSwpTransition();
	
private:
	static TInt iSsmStateManagerNextSwpRequestReturn;
	static TSsmSwp iSsmStateManagerSwpTransition;
	static TInt iSsmStateManagerNextConnectReturn;
	static TInt iSsmStateManagerNextRequestReturn;
	static TInt iSsmStateManagerHandle;
	static TSsmStateTransition iSsmStateManagerStateTransition;
};

#endif /*SSMCMNWRAPPER_H_*/
