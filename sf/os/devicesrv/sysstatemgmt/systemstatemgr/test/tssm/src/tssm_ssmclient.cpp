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

#include "tssm_ssmclient.h"


/**
RSsmStateManagerTestClient implementation
*/

EXPORT_C TInt RSsmStateManagerTestClient::Connect(const TDesC& aServerName, const TVersion& aVersion)
	{
	return DoConnect(aServerName, aVersion, -1);
	}

EXPORT_C TInt RSsmStateManagerTestClient::Connect()
	{
	return DoConnect(KSsmServerNameTest, TVersion(KSsmServMajorVersionNumber, KSsmServMinorVersionNumber, KSsmServBuildVersionNumber), -1);
	}
	
EXPORT_C void RSsmStateManagerTestClient::Close()	
	{
	RSsmStateManager::Close();
	}

EXPORT_C TInt RSsmStateManagerTestClient::HeapMark()
	{
	if(Handle())
		{
		return SendReceive(ERequestMarkHeap);
		}
	return KErrDisconnected;
	}

EXPORT_C TInt RSsmStateManagerTestClient::HeapMarkEnd()
	{
	return HeapMarkEnd(ENoCleanup);
	}

EXPORT_C TInt RSsmStateManagerTestClient::HeapMarkEnd(TInt aDoCleanup)
	{
	if(Handle())
		{
		TIpcArgs args(aDoCleanup);
		return SendReceive(ERequestMarkHeapEnd, args);
		}
	return KErrDisconnected;
	}

EXPORT_C TInt RSsmStateManagerTestClient::RequestDeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename)
	{
	if(Handle())
		{
		TIpcArgs args(aSwpKey, &aFilename);
		return SendReceive(ERequestDeRegisterSwpMapping, args);
		}
	return KErrDisconnected;
	}

