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


#include "tsus_ssmsuscli.h"
#include "susutilclisrv.h"

TInt RSsmSusCliTest::Connect(const TDesC& aServerName, TBool aHeapCheck)
	{
	return Connect(aServerName, Version(), -1, aHeapCheck);
	}

TInt RSsmSusCliTest::Connect(const TDesC& aServerName, TVersion aVersion, TInt aAsyncMessageSlots, TBool aHeapCheck)
	{
	TInt ret = DoConnect(aServerName, aVersion, aAsyncMessageSlots);
	if(aHeapCheck && (ret == KErrNone))
	{
		ret = HeapMark();
	}
	return ret;
	} //lint !e1746 Suppress parameter 'aVersion' could be made const reference

TInt RSsmSusCliTest::SendAnyrequest()
	{
	return SendReceive(KSusUtilServFirstUsedOpCode);
	}

TInt RSsmSusCliTest::Shutdown()
	{
	return SendReceive(EEndOfSusUtilOpCodes);
	}

void RSsmSusCliTest::Close(TBool aCheckHeap)
	{
	if(aCheckHeap)
		{
		TInt ret = CompressPluginArray();
		__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("RSsmSusCliTest"), ret));
		ret = HeapMarkEnd();
		__ASSERT_DEBUG(ret == KErrNone, User::Panic(_L("RSsmSusCliTest"), ret));
		}
	RSsmSusCli::Close();
	}

TInt RSsmSusCliTest::HeapMark()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeap);
		}
	return ret;
	} 

TInt RSsmSusCliTest::HeapMarkEnd()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugMarkHeapEnd);
		}
	return ret;
	} 

TInt RSsmSusCliTest::CompressPluginArray()
	{
	TInt ret = KErrNone;
	if(Handle())
		{
		ret = SendReceive(EDebugCompressPluginArray);
		}
	return ret;
	} 
