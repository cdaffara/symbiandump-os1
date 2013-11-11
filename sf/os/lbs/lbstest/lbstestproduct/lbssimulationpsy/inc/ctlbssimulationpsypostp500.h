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


#ifndef CTLBSSIMULATIONPSYPOSTP500_H_
#define CTLBSSIMULATIONPSYPOSTP500_H_

#include "ctlbsportedstepbase.h"

class CT_LbsSimulationPsyPosTp500 : public CT_LbsPortedStepBase
	{
public:

	CT_LbsSimulationPsyPosTp500(CT_LbsServer& aParent, TInt aIndex);

	// From CT_LbsPortedStepBase	
	virtual void StartL();

protected:

	// From CT_LbsPortedStepBase
	virtual void CloseTest();

private:

	TPtrC GetIndexNameL(TInt aIndex) const;
	void RunTestsL(TInt aIndex);
	void SetPartialUpdateL(TBool aPartial, RPositioner& aPositioner);
	TBool IsPositionInfoPartial(const TPositionInfo& aPosInfo);
	void SetCommonDataSetL(const TBool aCommonDataSet) const;
	void GetCommonDataSetL(TInt& aCommonDataSet) const;
	void OpenPositionersLC(RPositioner& aPositioner);
	void WaitForPositionChange(const TBool aNmea, const TInt aTime);
	void CheckPositionDataL(const TBool aNmea,
			const TBool aSimultaneousRequest, const TPosition& aPos1,
			const TPosition& aPos2);
	void PerformStaggeredRequestsL(RPositioner& aPositioner,
			const TBool aIsNmea, const TBool aCommonDataSet,
			const TBool aSwitchDataMode, TPositionInfo& aPosInfo1,
			TPositionInfo& aPosInfo2);
	void PerformSimultaneousRequestsL(RPositioner& aPositioner,
			TPositionInfo& aPosInfo1, TPositionInfo& aPosInfo2,
			const TInt aError);

	void TestMultipleClientAccessL(const TBool aIsNmea,
			const TBool aRequestInProgress = EFalse,
			const TBool aCommonDataSet = ETrue,
			const TBool aSwitchDataMode = EFalse);
	void TestMultipleClientChangeDataModeL(const TBool aIsNmea,
			const TBool aCommonDataSet, const TBool aSwitchDataMode = ETrue);

	void TestMultipleClientPartialFixL();

	void TestNmeaPartialFixL();

private:
	TInt iIndex;
	TInt iOldTrpMode;
	};

#endif /* CTLBSSIMULATIONPSYPOSTP500_H_ */
