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

#ifndef __TSUS_SSMSUSCLI_H__
#define __TSUS_SSMSUSCLI_H__

#include <ssm/ssmsuscli.h>

/**
 Extends the production RSsmSusCli with another version of Connect which 
 connects to our test-server rather than the production one started during
 device boot.
 */
class RSsmSusCliTest: public RSsmSusCli
	{
public:
	TInt Connect(const TDesC& aServerName, TBool aHeapCheck = ETrue);
	TInt Connect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots, TBool aHeapCheck = ETrue);
	TInt SendAnyrequest();
	TInt Shutdown();
	void Close(TBool aCheckHeap = ETrue);
	TInt HeapMark();
	TInt HeapMarkEnd();
	TInt CompressPluginArray();
	};

#endif	// __TSUS_SSMSUSCLI_H__
