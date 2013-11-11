/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef CTLBSSIMULATIONPSYPOSTP400_H_
#define CTLBSSIMULATIONPSYPOSTP400_H_

//  INCLUDES
#include "ctlbsportedstepbase.h"

class CT_LbsSimulationPsyPosTp400 : public CT_LbsPortedStepBase
	{
public:
	CT_LbsSimulationPsyPosTp400(CT_LbsServer& aParent, TInt aIndex);

	/*
	 * From CT_LbsPortedStepBase
	 */
	virtual void StartL();
	
	/*
	 * From CT_LbsPortedStepBase
	 */
	virtual TVerdict doTestStepPostambleL();
protected:
	/*
	 * Cleans up after a test.
	 * Always called, even if the test leaves.
	 */
	virtual void CloseTest();

private: //New methods
	TPtrC GetIndexName(TInt aIndex) const;
	void RunTestsL(TInt aIndex);
	TBool TwoClientRequestL(TBool aIsSps, TInt aSecondClientDelay, TInt aMaxUpdateAge, 
			TReal* aLatitudeDiff = NULL, TReal* aLongitudeDiff = NULL);
	TInt PerformSyncRequest(TPositionInfoBase& aModuleInfo, const RPositioner& aPositioner) const;
	void OpenNewSourceFileLC(const TDesC& aName, RPositioner& aPositioner);
	TInt TrpModeL() const;
	void SetTrpModeL(TInt aNewValue);

	//Test Methods
	void TestTrpMultiClientNmeaStoredFixExistL();
	void TestTrpMultiClientNmeaStoredFixNotExistL();
	void TestTrpMultiClientNmeaNoUpdateAgeL();
	void TestTrpMultiClientSpsStoredFixExistL();
	void TestTrpMultiClientSpsStoredFixNotExistL();
	void TestTrpMultiClientSpsNoUpdateAgeL();

private:
	TInt iIndex;
	TInt iOldTrpMode;
	};

#endif /* CTLBSSIMULATIONPSYPOSTP400_H_ */
