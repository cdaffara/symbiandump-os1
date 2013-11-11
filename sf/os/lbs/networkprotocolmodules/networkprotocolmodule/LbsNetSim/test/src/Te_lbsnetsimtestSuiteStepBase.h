/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file Te_lbsnetsimtestSuiteStepBase.h
*/

#if (!defined __TE_LBSNETSIMTEST_STEP_BASE__)
#define __TE_LBSNETSIMTEST_STEP_BASE__
#include <test/testexecutestepbase.h>
// Please add your include here if you have 
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

// Forwards
class CTe_lbsnetsimtestSuiteStepBase;

class CAOTest_Base :
	public CActive,
	public MLbsNetSimTestObserver
	{
	public:
		// Constructors
		CAOTest_Base(CTe_lbsnetsimtestSuiteStepBase* aBase, TInt aDelay, TBool aStepMode);
		~CAOTest_Base();
		
		// CActive
		virtual void RunL() = 0;
		virtual void DoCancel() = 0;
		
		// Others
		virtual void Activate();
		virtual TBool Finished() = 0;
		
		// MLbsNetSimTestObserver
		virtual void Connected();
		virtual void Disconnected();
		virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
		virtual void NotifyReleaseLcsMoLr(TInt aReason);
		virtual void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
		virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
		virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
		virtual void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
		virtual void NotifyMeasurementReportControlFailure(TInt aReason);
		virtual void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);
		virtual void NotifyError(MLbsNetSimTestObserver::EFunction aFunction, TInt aError);
	protected:
		CTe_lbsnetsimtestSuiteStepBase* 	iBase;
		RLbsNetSimTest*						iLbsNetSimTest;
		TInt								iDelay;
		TBool								iStepMode;		
	};
	
class CAOGateway_Base:
	public CActive,
	public MLbsNetSimObserver
	{
	public:
		// Constructors
		CAOGateway_Base(CTe_lbsnetsimtestSuiteStepBase* aBase);
		~CAOGateway_Base();
				
		// CActive
		virtual void RunL() = 0;
		virtual void DoCancel() = 0;
		
		// Others
		void Activate();
		virtual TBool Finished() = 0;
			
		// MLbsNetSimGatewayObserver
		virtual void ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& aInfo, const TLbsNetPosRequestPrivacy& aType);
		virtual void ProcessReleaseLcsLocationNotification(TInt aReason);
		virtual void ProcessFacilityLcsMoLrResult(TInt aReason, const TPositionInfoBase* aPosition);
		virtual void ProcessReleaseLcsMoLr();
		virtual void ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
		virtual void ProcessMeasurementControlLocationError(TInt aError);
		virtual void ProcessResetAssistanceData(TLbsAssistanceDataGroup aMask);
		virtual void ProcessError(TInt aError);
	protected:
		CTe_lbsnetsimtestSuiteStepBase*			iBase;
		RLbsNetSim*								iLbsNetSim;
	};


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_lbsnetsimtestSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_lbsnetsimtestSuiteStepBase();
	CTe_lbsnetsimtestSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	void ActivateTest()
		{
		iTest->Activate();
		}
	void ActivateGateway()
		{
		iGateway->Activate();
		}
		
	TInt State()
		{
		return iState;
		}
	void SetState(TInt aState)
		{
		iState = aState;
		}
	TUid Provider()
		{
		return iProvider;
		}

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
	CActiveScheduler* iAs;
	
	TInt		iDelay;
	TInt		iState;
	TUid		iProvider;
	TBool		iStepMode;
	
	CAOTest_Base*		iTest;
	CAOGateway_Base*	iGateway;
	};
	
#endif
