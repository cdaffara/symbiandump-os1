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
 

#ifndef __TCLE_CLECLIENT_H__
#define __TCLE_CLECLIENT_H__


#include "clecli.h"

_LIT(KCleSrver, "TestCleSrv");

/**
RCleSessionTest is a utility class that allows the test to check for heap balance. 
That's why we need to a derived class to be able to provide a new methods for heap check

*/
class RCleSessionTest : public RCleSession
	{
public:
	TInt HeapMark();
	TInt HeapMarkEnd();
	TInt ReleaseNeverUnloadLibs(); //It will return the number of libraries closed by server which has ENeverUnload as the unloading option.
	TInt SetHeapFailure(TInt aFailureRate);
	TInt HeapReset();
	};


#endif
