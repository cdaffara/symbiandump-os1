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
// This is the test server for Lbs AGPS PSY tests.
// 
//

#if (!defined __TE_LBSAGPSPSYSUITE_H__)
#define __TE_LBSAGPSPSYSUITE_H__

#include <test/testexecuteserverbase.h>

#include "te_lbspsystaticdata.h"
#include "LbsInternalInterface.h"

/**
This is the test server for Lbs AGPS PSY tests.
*/
class CTe_LbsAgpsPsySuite : public CTestServer, public MTe_LbsPsyStaticData
	{
public:
	static CTe_LbsAgpsPsySuite* NewL();
	virtual ~CTe_LbsAgpsPsySuite();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	//From MTe_PsySpecificData
	virtual TUid ModuleUid() const;
	virtual const RLbsPositionUpdateRequests::TChannelIdentifer& UpdateReqChanId() const;
	virtual TUint CountPositioner();
	virtual CTestExecuteLogger& Logger() const;
	
protected:
	void ConstructL();
	
private:
	CTestStep* CreateTestStepL(const TDesC& aStepName);
	
	TBuf8<KMaxFileName> iFirstExe;
	TUint iPositionerCount;
	CTestStep* iCurrentStep;
	};

#endif //__TE_LBSAGPSPSYSUITE_H__
