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

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"
#include <networking/testsuite.h>

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
CMBufManager * CTestStepCTMbufmgr::CreateInstanceMBufMgrL(TInt aMaxHeapSize)
	{
	MMBufSizeAllocator* mbufSizeAllocator;
	CMBufManager* mngr;
	mngr = CreateInstanceMBufMgrL(aMaxHeapSize, mbufSizeAllocator);
	if (iMBMngr)
			{
#ifdef SYMBIAN_MBUFMGR_BACKGROUND_ALLOCATION
			// this method used for tests that dont do background allocation so make the
			// pool big enough to fill the heap
			mbufSizeAllocator->AddL(KMBuf_MBufSize, (aMaxHeapSize/(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
#else
			mbufSizeAllocator->AddL(KMBuf_MBufSize, KMBuf_InitialAllocation, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
#endif
			}
	return mngr;
	}

CMBufManager * CTestStepCTMbufmgr::CreateInstanceMBufMgrL(TInt aMaxHeapSize, MMBufSizeAllocator* &aMBufSizeAllocator)
	{
	__ASSERT_ALWAYS(aMaxHeapSize >= 0, User::Panic(_L("MBufTest"),0));

	iMBMngr = CMBufManager::NewL(aMaxHeapSize, aMBufSizeAllocator);

	return iMBMngr;
	}
