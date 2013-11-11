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
 

#ifndef __TIPCFUZZER_H__
#define __TIPCFUZZER_H__

#include <e32std.h>
#include <e32base.h>
#include <test/testexecutestepbase.h>

// Test resource class
class CIpcFuzzer : public CBase
	{
public:
	//Two-phase constructor.
	static CIpcFuzzer* NewL();
	static CIpcFuzzer* NewLC();
	~CIpcFuzzer(); // Destructor

public:
	//IPC Fuzz Logic
	HBufC* GetDescData16L(TUint aIdx);
    HBufC8* GetDescData8L(TUint aIdx);
    TInt GetIntData(TUint aIdx);
	
private:
	//Constructor
	CIpcFuzzer();
	void ConstructL();
	};

#endif //__TIPCFUZZER_H__
