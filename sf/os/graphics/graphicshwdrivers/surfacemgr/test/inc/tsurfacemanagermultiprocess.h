// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSURFACEMANAGERMULTIPROCESS_H__
#define __TSURFACEMANAGERMULTIPROCESS_H__

#include "tsmgmultprocessshared.h"

class CTSurfaceManagerMultiProcess : public CTGraphicsBase
	{
public:
	CTSurfaceManagerMultiProcess(CTestStep* aStep);
	~CTSurfaceManagerMultiProcess();
protected:
	//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestMapSurfaceExistinChunkL();
	void TestSynchronizeCacheMultiProcessL();
	void TestCloseChannelsMultiProcessL();
	void TestCloseChannelsMultiProcess2L();
	void TestSurfaceInfoUnopenedSurfaceL();
	void TestCloseChannelsMultiProcess1L();
	void TestSurfaceInfoUsingSurfaceIdL();
	void TestOpeningSurfaceUsingSurfaceIdL();
	void TestOpenSurfaceInvalidParamsL();
	void TestClosingUnopenedSurfaceL();
	void CreateOpenCloseThreeProcessL();
	void TestSurfaceAccessWhenCreatingProcessDiesL();
	void TestClosingSurfaceWhenCreatingProcessDiesL();
	void TestCloseSurfaceInThirdProcessL();
	void TestNoAccessWhenSurfaceClosedTwoProcessL();
	void TestOpeningOnProcessAfterClosingOnOtherL();
	void TestAccessAfterClosingL();
	void TestClosingAfterClosingOnOtherProcessL();
	void TestErrSufaceAccessNotOpenL();
	void TestReadFromBufferInSecondProcessL();
	void TestGetSurfaceHintMultiProcessL();
	void TestSetSurfaceHintMultiProcessL();
	void TestAddSurfaceHintMultiProcessL();
	void TestOutofMemoryCasesL();
private:
	TLeaveInfo iInfo2;
	};

	
class CTSurfaceManagerMultiProcessStep : public CTGraphicsStep
	{
public:
	CTSurfaceManagerMultiProcessStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTSurfaceManagerMultiProcessStep,"TSurfaceManagerMultiProcess");


#endif
