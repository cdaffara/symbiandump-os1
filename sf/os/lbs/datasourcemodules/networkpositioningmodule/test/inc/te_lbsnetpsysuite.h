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
// This is the test server for Lbs Net PSY tests.
// 
//

#if (!defined __TE_LBSNETPSYSUITE_H__)
#define __TE_LBSNETPSYSUITE_H__

#include <test/testexecuteserverbase.h>

#include "te_lbspsystaticdata.h"

/**
This is the test server for Lbs Net PSY tests.
*/
class CTe_LbsNetPsySuite : public CTestServer, public MTe_LbsPsyStaticData
	{
public:
	static CTe_LbsNetPsySuite* NewL();
	virtual ~CTe_LbsNetPsySuite();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	//From MTe_PsySpecificData
	virtual TUid ModuleUid() const;
	virtual const RLbsPositionUpdateRequests::TChannelIdentifer& UpdateReqChanId() const;
	virtual TUint CountPositioner();
	virtual CTestExecuteLogger& Logger() const;

protected:
	void ConstructL();

private:
	void InitializeQualityProfileInfoL();
	void GetQualityProfileInfoL(RFs& aFs, RArray<TQualityProfile>& aQualityArray,
	                                      const TDesC& aFileName);
private:
	CTestStep* CreateTestStepL(const TDesC& aStepName);
	
	TBuf8<KMaxFileName> iFirstExe;
	TUint iPositionerCount;
	CTestStep* iCurrentStep;
	};

#endif //__TE_LBSNETPSYSUITE_H__
