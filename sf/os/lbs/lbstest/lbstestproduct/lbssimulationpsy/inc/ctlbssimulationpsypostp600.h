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


#ifndef CTLBSSIMULATIONPSYPOSTP600_H_
#define CTLBSSIMULATIONPSYPOSTP600_H_

#include "ctlbsportedstepbase.h"

class CT_LbsSimulationPsyPosTp600 : public CT_LbsPortedStepBase
	{
private:
	class TPositionRequest;

public:

	CT_LbsSimulationPsyPosTp600(CT_LbsServer& aParent, TInt aIndex);

	virtual void StartL();
	virtual TVerdict doTestStepPostambleL();

protected:
	virtual void CloseTest();

private:

	TPtrC GetIndexNameL(TInt aIndex) const;
	void RunTestsL(TInt aIndex);
	void SetCommonDataSetL(const TBool aCommonDataSet) const;
	void GetCommonDataSetL(TInt& aCommonDataSet) const;
	void OpenPositionersLC(RPositioner& aPositioner);
	
	void CheckRequestTimeL(const TRequestStatus& aStatus, TInt aDelta, TBool aFirst, TInt aExpected);
	void GetDistanceL(const TRequestStatus& aStatus, const TPositionInfo& aPosInfo, TReal32& aDistance);
	void GetDistanceL(const TPositionRequest& aRequest, TReal32& aDistance);
	void CheckNthFixL(const TPositionInfo& aPosInfo, const TCoordinate* aFixes, TInt aIndex);	
	
	typedef void (CT_LbsSimulationPsyPosTp600::*TCompareFunction)(const TPositionRequest& aRequest1, const TPositionRequest& aRequest2, const TTime& aTimeCompleted, TInt aIndex);	

	void CheckSPSTrackingWithIndependentDataSetL(
			const TPositionRequest& aRequest1, 
			const TPositionRequest& aRequest2, 
			const TTime& aTimeCompleted,
			TInt aIndex);
	
	void CheckNMEATrackingWithIndependentDataSetL(
			const TPositionRequest& aRequest1, 
			const TPositionRequest& aRequest2, 
			const TTime& aTimeCompleted,
			TInt aIndex);

	void CheckSPSTrackingWithCommonDataSetL(
			const TPositionRequest& aRequest1, 
			const TPositionRequest& aRequest2, 
			const TTime& aTimeCompleted,
			TInt aIndex);
	
	void CheckNMEATrackingWithCommonDataSetL(
			const TPositionRequest& aRequest1, 
			const TPositionRequest& aRequest2, 
			const TTime& aTimeCompleted,
			TInt aIndex);
	
	void RunTestL(const TDesC& aDataFile, TBool aTrp, TCompareFunction aCheckL);

private:
	class TPositionRequest
		{
		public:
			TPositionRequest(RPositioner& aPositioner);
			~TPositionRequest();			
			void MakeRequestL();

		public:
			RPositioner& iPositioner;
			TRequestStatus iStatus;
			TTime iRequestTime;
			TPositionInfo iPositionInfo;
		};

private:
	TInt iIndex;
	TInt iOldTrpMode;
	};

#endif /* CTLBSSIMULATIONPSYPOSTP600_H_ */
