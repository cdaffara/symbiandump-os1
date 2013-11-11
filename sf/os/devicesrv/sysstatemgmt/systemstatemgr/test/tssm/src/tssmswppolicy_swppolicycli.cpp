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

#include "tssmswppolicy_swppolicycli.h"
#include "ssmswppolicyconst.h"

/**
RSsmSwpPolicySessionTest implementation
*/

TInt RSsmSwpPolicySessionTest::HeapMark()
	{
	if(Handle())
		{
		return SendReceive(EDebugMarkHeap);
		}
	return KErrDisconnected;
	}
	
TInt RSsmSwpPolicySessionTest::DeleteSSwpPolicySessionInfo()
	{
	if(Handle())
		{
		return SendReceive(EDeleteSSwpPolicySessionInfo);
		}
	return KErrDisconnected;
	}

TInt RSsmSwpPolicySessionTest::HeapMarkEnd()
	{
	if(Handle())
		{
		return SendReceive(EDebugMarkHeapEnd);
		}
	return KErrDisconnected;
	}

TInt RSsmSwpPolicySessionTest::SetHeapFailure(TInt aFailureRate)
	{
	if(Handle())
		{
		return SendReceive(EDebugSetHeapFailure, TIpcArgs(aFailureRate));
		}
	return KErrDisconnected;
	}

TInt RSsmSwpPolicySessionTest::HeapReset()
	{
	if(Handle())
		{
		return SendReceive(EDebugHeapReset);
		}
	return KErrDisconnected;
	}

