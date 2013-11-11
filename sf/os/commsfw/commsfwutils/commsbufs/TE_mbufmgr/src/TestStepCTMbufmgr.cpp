// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definition of CTestStepCTMbufmgr which is the base class
// for all the Mbufmgr Test Step classes
// 
//

// EPOC includes
//#include <e32base.h>
//#include <es_sock.h>

// Test system includes
#include <es_mbuf.h>

//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#include "networking/testsuite.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#include <networking/testsuite.h>
//#endif

#include "TestStepCTMbufmgr.h"
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufpondop.h>

// constructor
CTestStepCTMbufmgr::CTestStepCTMbufmgr() 
	{
	}

// destructor
CTestStepCTMbufmgr::~CTestStepCTMbufmgr()
	{
	}

void CTestStepCTMbufmgr::StripeMem(TUint8 *aBuf, 
								   TInt aStartPos, 
								   TInt anEndPos, 
								   TUint aStartChar, 
								   TUint anEndChar)
	{
	TUint character=aStartChar;
	for (TInt i=aStartPos;i<anEndPos;i++)
		{
		aBuf[i]=(TText8)character;
		if(++character>anEndChar) character=aStartChar;
		}
	}

void CTestStepCTMbufmgr::StripeDes(TDes8 &aBuf, 
								   TInt aStartPos, 
								   TInt anEndPos, 
								   TUint aStartChar, 
								   TUint anEndChar)
// Mark a buffer with repeating byte pattern
	{
	__ASSERT_ALWAYS(aStartChar<=anEndChar, User::Panic(_L("MBufTest"),0));
	__ASSERT_ALWAYS(aStartPos<=anEndPos, User::Panic(_L("MBufTest"),0));
	__ASSERT_ALWAYS(anEndPos<=aBuf.Length(), User::Panic(_L("MBufTest"),0));

	StripeMem((TUint8 *)aBuf.Ptr(), aStartPos, anEndPos, aStartChar, anEndChar);
	}

// create an mbuf manager instance with some arbitrary default mbuf size alloc info
// - refer CMBufManager::NewL notes regarding explaination of why this is deliberately not done within the mbuf manager
static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;
#define SYMBIAN_MBUFMGR_BACKGROUND_ALLOCATION
#ifndef SYMBIAN_MBUFMGR_BACKGROUND_ALLOCATION
static const TInt KMBuf_InitialAllocation = 128;
#endif
void CTestStepCTMbufmgr::CreateInstanceMBufMgrL(TInt aMaxHeapSize)
	{

	RArray<TCommsBufPoolCreateInfo> poolInfoArray;
	TCommsBufPoolCreateInfo createInfo;
#ifdef SYMBIAN_MBUFMGR_BACKGROUND_ALLOCATION
	
	// this method used for tests that dont do background allocation so make the 
	// pool big enough to fill the heap
	createInfo.iBufSize = KMBuf_MBufSize;
	createInfo.iInitialBufs = (aMaxHeapSize/(KMBuf_MBufSize+sizeof(RMBuf)))-1;
	createInfo.iGrowByBufs = KMBuf_MinGrowth;
	createInfo.iMinFreeBufs = KMBuf_GrowthThreshold;
	createInfo.iCeiling = aMaxHeapSize/KMBuf_MBufSize;
#else
	createInfo.iBufSize = KMBuf_MBufSize;
	createInfo.iInitialBufs = KMBuf_InitialAllocation;
	createInfo.iGrowByBufs = KMBuf_MinGrowth;
	createInfo.iMinFreeBufs = KMBuf_GrowthThreshold;
	createInfo.iCeiling = aMaxHeapSize/KMBuf_MBufSize;	
#endif
	poolInfoArray.Append ( createInfo );		

	User::LeaveIfError(iBufPond.Open(poolInfoArray));
	poolInfoArray.Close ();
	}

void CTestStepCTMbufmgr::CreateInstanceMBufMgrL(RArray<TCommsBufPoolCreateInfo>& aPoolCreateInfo)
	{
	User::LeaveIfError(iBufPond.Open(aPoolCreateInfo));
	}
