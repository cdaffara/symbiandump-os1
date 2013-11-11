// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration of CTestStepCTMbufmgr class that is
// base class for all test step classes in this test.
// 
//

#if (!defined __TESTSTEPSCTMBUFMGR_H__)
#define __TESTSTEPSCTMBUFMGR_H__

#include <cflog.h>
#include <comms-infras/commsbufpondop.h>
#include <es_mbuf.h>
#include <es_mbman.h>

//#include <networking/teststep.h>
#include <test/testexecutestepbase.h>

//const TUint KMBufMinHeapSize        = 4 * 1024;

//const TUint KMBufDefaultHeapSize    = 256 * 1024;

class CCommsBufManager;
class CTestSuiteCTMbufmgr;
class TCommsBufPoolCreateInfo;
NONSHARABLE_CLASS(CTestStepCTMbufmgr) : public CTestStep
{
public:
	CTestStepCTMbufmgr();
	~CTestStepCTMbufmgr();

	CTestSuiteCTMbufmgr * iEsockSuite;// Pointer to the suite which owns this test
protected:
	void StripeMem(TUint8 *aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar);
	void StripeDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar);
	void CreateInstanceMBufMgrL(TInt aMaxHeapSize);
	void CreateInstanceMBufMgrL(RArray<TCommsBufPoolCreateInfo>& aPoolCreateInfo);

	CActiveScheduler* 				iActSch;
	RCommsBufPondOp 				iBufPond;
};

#endif /* __TESTSTEPSCTMBUFMGR_H__ */

