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

#ifndef hwrmtestbase_h
#define hwrmtestbase_h

#include <test/tefunit.h>
#include <hwrmvibra.h>
#include <hwrmlight.h>
#include <hwrm/hwrmextendedlight.h>
#include <hwrm/hwrmenhancedlight.h>
#include <hwrmpower.h>
#include <hwrm/hwrmfmtx.h>
#include <rmocklight.h>
#include <rmockpowerstate.h>
#include <rmockvibra.h>
#include <rmockpower.h>
#include <rmockfmtx.h>
#include <e32property.h>
#include <hwrmpowerstatesdkpskeys.h>
#include "endstatetimer.h"
#include "pspropertyobserver.h"

const TInt KMaxNoOpenSessionsForResource = 3;
const TInt KMaxNoStateTransitions = 5;

const TInt KDefaultVibraMinOffTime = 0;

// temp, may need to move but in original test was in main.cpp
const TUint32 KHWRMTestLightIntensityValue      = 0x10000007;
const TUint32 KHWRMTestLightSensitivityValue    = 0x10000008;


class CHWRMTestBase : 	public CTestFixture , 
						public MHWRMVibraObserver, 
						public MHWRMLightObserver, 
						public MHWRMExtendedLightObserver,
						public MHWRMBatteryPowerObserver,
						public MHWRMFmTxObserver,
						public MTimerExpired, 
						public MPsPropertyObserver,
						public MHWRMVibraFeedbackObserver
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
						,
						public MHWRMBatteryChargingStatusObserver,
						public MHWRMBatteryFullChargingTimeObserver,
						public MHWRMBatteryChargingCurrentObserver
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	{
public:// from MHWRMVibraObserver
    virtual void VibraModeChanged(CHWRMVibra::TVibraModeState aStatus);
    virtual void VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus);
    // from MHWRMVibraFeedbackObserver
    virtual void VibraFeedbackModeChanged(CHWRMVibra::TVibraFeedbackModeState aMode);

public: // from MHWRMLightObserver
    virtual void LightStatusChanged(TInt aTarget, CHWRMLight::TLightStatus aStatus);

public: //from MHWRMExtendedLightObserver
	virtual void LightStatusChanged(TInt aTarget, CHWRMExtendedLight::TLightStatus aStatus);

public: //from MHWRMBatteryPowerObserver

	virtual void PowerMeasurement(TInt aErrorCode, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement);
public: //from MHWRMFmTxObserver
	virtual void FmTxStatusChanged(TFmTxState aState);
	virtual void FmTxFrequencyChanged(TInt aFrequency);
	
public: // from MPsPropertyObserver
	virtual void PsPropertyChanged(TInt aKey, TInt aValue);
	
public: //from MTimerExpired
	void EndStateTimerExpired(TInt aError);

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
public:
	//from MHWRMBatteryChargingStatusObserver
    void ChargingStatusChange(TInt aErrorCode, CHWRMPower::TBatteryChargingStatus aChrgStatus);
	//from MHWRMBatteryFullChargingTimeObserver
	void BatteryFullChargingTimeChange(TInt aErrorCode, TUint aTime);
	//from MHWRMBatteryChargingCurrentObserver
	void AverageChargingCurrentChange(TInt aErrorCode, TInt aMeasurement);
#endif //SYMBIAN_HWRM_EXTPOWERINFO

public:
	// SetUp and TearDown code
	void SetupL();
	void TearDownL();
	inline void SetTestFail();
	
protected: //useful functions
	template <class T>
	void AddTestStateL(T* aObject, void (T::*aTestStateFptr)(),TReal aMaxDurationIsSecs);
	
	void OpenSessionsL(TInt aNoLightSessionsToOpen, TInt aNoVibraSessionsToOpen, TInt aNoExtLightSessionsToOpen = 0, TInt aNoFmTxSessionsToOpen = 0, TInt aNoPowerSessionsToOpen = 0);
	void OpenPowerStateSessionL();
	void OpenLightSessionEnhancedL(TInt aNoEnhancedLightSessionsToOpen);
	inline CHWRMVibra& GetVibraSessionL(TInt aIndex);
	inline CHWRMLight& GetLightSessionL(TInt aIndex);
	inline CHWRMExtendedLight& GetExtLightSessionL(TInt aIndex);
	inline CHWRMEnhancedLight& GetEnhancedLightSessionL(TInt aIndex);
	inline CHWRMPower& GetPowerSessionL(TInt aIndex);
	inline CHWRMFmTx& GetFmTxSessionL(TInt aIndex);
	inline CHWRMVibra& GetVibraSessionWithCallbackRegisteredL();
	inline CHWRMLight& GetLightSessionWithCallbackRegisteredL();
	inline CHWRMExtendedLight& GetExtLightSessionWithCallbackRegisteredL();
	inline CHWRMEnhancedLight& GetEnhancedLightSessionWithCallbackRegisteredL();
	inline CHWRMPower& GetPowerSessionWithCallbackRegisteredL();
	inline CHWRMFmTx& GetFmTxSessionWithCallbackRegisteredL();
	
	void SetCRVibraAttributeL(TUint32 aVibraKey, TInt aValue);
	TInt SetCRVibraAttribute(TUint32 aVibraKey, TInt aValue);
	void GetCRVibraAttributeL(TUint32 aVibraKey, TInt& aValue);
	void GetVibraTimeAttributesL();
	
	void SetCRLightAttributes(TUint32 aLightKey, TInt aValue);
	void GetCRLightAttributes(TUint32 aLightKey, TInt &aValue);
	void SetLightControlAttributes(TInt aIntensity, TInt aSensitivity, TInt aInUse);
	void GetLightControlAttributes();
	
	void SetCRPowerAttributeL(TUint32 aVibraKey, TInt aValue);
	void GetCRPowerAttributeL(TUint32 aVibraKey, TInt &aValue);
	void GetPowerAttributesL();
	void SetPowerAttributesMeasurementProcessingTime(TUint aMeasurementProcessingTime);
	
	void SetCRFmTxAttributeL(TUint32 aVibraKey, TInt aValue);
	void GetCRFmTxAttributeL(TUint32 aVibraKey, TInt &aValue);
	void GetFmTxFrequencyStepSizeL();
	void SetFmTxFrequencyRangeJapanL();
	void SetFmTxFrequencyRangeEuropeL();
	void SetFmTxInactivityTimeOutL(TUint aInactivityTimeOut);
	
	void ResetDefaultsL();
		
	inline RMockLight& GetMockLight();
	inline RMockVibra& GetMockVibra();

	inline RMockFmTx& GetMockFmTx();

	inline RMockPowerState& GetMockPowerState();	
	inline RMockPower& GetMockPower();
	
	void ExpectVibraStatusNotificationL(CHWRMVibra::TVibraStatus aStatus);
	void ExpectVibraModeNotificationL(CHWRMVibra::TVibraModeState aState);
	void ExpectVibraModeNotificationL();
	
	void ExpectVibraFeedbackModeNotificationL(CHWRMVibra::TVibraFeedbackModeState aStatus);

	void ExpectLightStatusNotificationL(TInt aTarget, CHWRMLight::TLightStatus aStatus);
	void ExpectExtLightStatusNotificationL(TInt aTarget, CHWRMExtendedLight::TLightStatus aStatus);
	
	void ExpectFmTxStatusNotificationL(TFmTxState aStatus);
	void ExpectFmTxStatusNotificationL(TFmTxState aStatus1, TFmTxState aStatus2);
	void ExpectFmTxFrequencyNotificationL(TInt aFrequency);	
	
	void ExpectBatteryLevelNotificationL(EPSHWRMBatteryLevel aBatteryLevel);
	void ExpectBatteryStatusNotificationL(EPSHWRMBatteryStatus aStatus);
	void ExpectExtendedBatteryStatusNotificationL(EPSHWRMBatteryStatus aStatus);
	void ExpectedChargingStatusNotificationsL(EPSHWRMChargingStatus aStatus);
	void ExpectedExtendedChargingStatusNotificationsL(EPSHWRMChargingStatus aStatus);
	void ExpectBatteryPowerMeasurementsErrorL(TInt aErrorCode);	
	
	// TO DO: temp solution, need to add counters
	void CheckPluginInsensitivity();
	
	void ExpectLeave(TInt aError);
	void EndExpectLeaveL(TInt aErr, const TText8* aFile,TInt aLine);
	void CheckMockSYResultL();
	void CheckAllExpectedNotificationsReceived();
	
	void ExecuteTestL();
	void CheckForEndOfTransition();
	
	void CleanupPropertyObservers();
	void BatteryLevelChanged(TInt aBatteryLevel);
	void BatteryStatusChanged(TInt aBatteryStatus);
	void ExtendedBatteryStatusChanged(TInt aBatteryStatus);
	void ChargingStatusChanged(TInt aChargingStatus);
	void ExtendedChargingStatusChanged(TInt aChargingStatus);
	
	const TDesC& MapVibraModeState(CHWRMVibra::TVibraModeState aState);
	const TDesC& MapVibraModeStatus(CHWRMVibra::TVibraStatus aStatus);
	const TDesC& MapLightStatus(CHWRMLight::TLightStatus aStatus);
	const TDesC& MapFmTxStatus(TFmTxState aStatus);
	
	void SetIgnoreStatusNotifications(TBool aIgnore);
	TBool IgnoringStatusNotifications() const;
	
	void GetTargetsOn(TInt& aTargetsOnMask) const;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	void OpenPowerStateSessionforChrgStatusL();
	void SetPowerChargingCurrentMeasurementProcessingTime(TUint aCallbackProcessingTime);
	void ExpectBatteryChargingStatusErrorL(TInt aErrorCode);
	void ExpectBatteryChargingTimeMeasurementsErrorL(TInt aErrorCode);
	void ExpectBatteryChargingCurrentMeasurementsErrorL(TInt aErrorCode);
#endif
	
protected:
	TFixedArray<CHWRMVibra*,KMaxNoOpenSessionsForResource> iVibraSessions;
	TFixedArray<CHWRMLight*,KMaxNoOpenSessionsForResource> iLightSessions;
	TFixedArray<CHWRMExtendedLight*,KMaxNoOpenSessionsForResource> iExtLightSessions;
	TFixedArray<CHWRMPower*,KMaxNoOpenSessionsForResource> iPowerSessions;
	TFixedArray<CHWRMFmTx*,KMaxNoOpenSessionsForResource> iFmTxSessions;
	TFixedArray<CHWRMEnhancedLight*,KMaxNoOpenSessionsForResource> iEnhancedLightSessions;
	
	RMockLight iMockLight;
	TBool iIsMockLightSessionOpen;
	RMockVibra iMockVibra;
	TBool iIsMockVibraSessionOpen;
	RMockPowerState iMockPowerState;
	TBool iIsMockPowerStateSessionOpen;
	RMockPower iMockPower;
	TBool iIsMockPowerSessionOpen;
	RMockFmTx iMockFmTx;
	TBool iIsMockFmTxSessionOpen;
	
	struct TStepStateBase
		{
		virtual void ExecuteTestState() =0;
		TReal iMaxDurationInSecs;
		};

	template <class T>
	struct TStepState : public TStepStateBase
		{
		typedef void (T::*TTestStateFptr)();
		T* iObject;
		TTestStateFptr iStateFptr;
		virtual void ExecuteTestState();
		};
	
	RPointerArray<TStepStateBase> iStepStates;
	CActiveScheduler* iScheduler;
	CEndStateTimer* iEndStateTimer;
	TInt iTestResult;
	TInt iCurrentState;
	TInt iExpectedLeaveCode;
	
	RArray<CHWRMVibra::TVibraStatus> 	iExpectedVibraStatusNotifications;
	RArray<CHWRMVibra::TVibraModeState> iExpectedVibraModeNotifications;
    RArray<CHWRMVibra::TVibraFeedbackModeState>	iExpectedVibraFeedbackModeNotifications;
	RArray<EPSHWRMBatteryLevel> iExpectedBatteryLevelNotifications;
	RArray<EPSHWRMBatteryStatus> iExpectedBatteryStatusNotifications;
	RArray<EPSHWRMBatteryStatus> iExpectedExtendedBatteryStatusNotifications;
	RArray<EPSHWRMChargingStatus> iExpectedChargingStatusNotifications;
	RArray<EPSHWRMChargingStatus> iExpectedExtendedChargingStatusNotifications;
	RArray<TInt> iExpectedBatteryPowerMeasurementsError;
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	RArray<TInt> iExpectedBatteryChargingTimeMeasurementsError;
	RArray<TInt> iExpectedBatteryChargingCurrentMeasurementsError;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	
	struct TLightNotification
		{
		TInt iTarget;
		CHWRMLight::TLightStatus iStatus;
		};
	RArray<TLightNotification> iExpectedLightNotifications;

	struct TExtLightNotification
		{
		TInt iTarget;
		CHWRMExtendedLight::TLightStatus iStatus;
		};
	RArray<TExtLightNotification> iExpectedExtLightNotifications;
	
	struct TFmTxNotification
		{
		TFmTxState iState1;
		TFmTxState iState2;
		};
	RArray<TFmTxNotification> iExpectedFmTxStatusNotifications;
	
	RArray<TInt> iExpectedFmTxFrequencyNotifications;
	
	//vibra control attributes
	TInt iVibraMinOffTime;
	TInt iVibraMaxOnTime;
	TInt iPulseMaxDuration;
	TInt iPulseMaxIntensity;

	//light control attributes
	TInt iLightDefaultIntensity;
	TInt iLightDefaultSensitivity;
	TInt iLightSensorEnabled;
	
	RProperty iLightIntensityProperty;
	RProperty iLightSensitivityProperty;
	
	TInt iPowerMaxReportingPeriodMicroSeconds;
	TInt iPowerBaseIntervalMicroSeconds;
	
	TUint iPowerAttributesMeasurementProcessingTime;// milliseconds
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	TUint iPowerChargingCurrentMeasurementProcessingTime; //milliseconds
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	
	TInt iFmTxFrequencyStepSize; // KHz
	
	TBool iSensorSupported;
	
#ifdef INSECURE_AUDIO_POLICY_KEYS
	//fmtx control attributes
	// Used to mimic Audio Policy behaviour
	RProperty iAudioRoutedProperty;
#endif // INSECURE_AUDIO_POLICY_KEYS  	
	
	// P&S property observers
	CPsPropertyObserver* iBatteryLevelObserver;
	CPsPropertyObserver* iBatteryStatusObserver;
	CPsPropertyObserver* iExtendedBatteryStatusObserver;
	CPsPropertyObserver* iChargingStatusObserver;
	CPsPropertyObserver* iExtendedChargingStatusObserver;
	
	TBool iTearingDown;
	TBool iIgnoreStatusNotifications;
	};
	
//inlines

template <class T>
void CHWRMTestBase::AddTestStateL(T* aObject, void (T::*aTestStateFptr)(),TReal aMaxDurationIsSecs)
	{
	TStepState<T>* testState = new (ELeave)TStepState<T>;
	CleanupStack::PushL(testState);
	testState->iStateFptr = aTestStateFptr;
	testState->iObject = aObject;
	testState->iMaxDurationInSecs = aMaxDurationIsSecs;
	iStepStates.AppendL(testState);
	CleanupStack::Pop(testState);
	}

template <class T>
void CHWRMTestBase::TStepState<T>::ExecuteTestState()
	{
	(iObject->*iStateFptr)();
	}

inline CHWRMVibra& CHWRMTestBase::GetVibraSessionL(TInt aIndex)
	{
	if(!iVibraSessions[aIndex])
		User::Leave(KErrNotFound);
	
	return *iVibraSessions[aIndex];
	}
inline CHWRMLight& CHWRMTestBase::GetLightSessionL(TInt aIndex)
	{
	if(!iLightSessions[aIndex])
		User::Leave(KErrNotFound);
	
	return *iLightSessions[aIndex];
	}

inline CHWRMFmTx& CHWRMTestBase::GetFmTxSessionL(TInt aIndex)
	{
	if(!iFmTxSessions[aIndex])
		User::Leave(KErrNotFound);
	
	return *iFmTxSessions[aIndex];
	}

inline CHWRMExtendedLight& CHWRMTestBase::GetExtLightSessionL(TInt aIndex)
	{
	if(!iExtLightSessions[aIndex])
		User::Leave(KErrNotFound);
	
	return *iExtLightSessions[aIndex];
	}

inline CHWRMEnhancedLight& CHWRMTestBase::GetEnhancedLightSessionL(TInt aIndex)
	{
	if(!iEnhancedLightSessions[aIndex])
		User::Leave(KErrNotFound);
	
	return *iEnhancedLightSessions[aIndex];
	}

inline CHWRMPower& CHWRMTestBase::GetPowerSessionL(TInt aIndex)
{
	if(!iPowerSessions[aIndex])
		User::Leave(KErrNotFound);

	return *iPowerSessions[aIndex];
}

inline CHWRMVibra& CHWRMTestBase::GetVibraSessionWithCallbackRegisteredL()
	{
	if(!iVibraSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);
	
	return *iVibraSessions[0];
	}

inline CHWRMLight& CHWRMTestBase::GetLightSessionWithCallbackRegisteredL()
	{
	if(!iLightSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);

	return *iLightSessions[0];
	}

inline CHWRMExtendedLight& CHWRMTestBase::GetExtLightSessionWithCallbackRegisteredL()
	{
	if(!iExtLightSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);

	return *iExtLightSessions[0];	
	}

inline CHWRMEnhancedLight& CHWRMTestBase::GetEnhancedLightSessionWithCallbackRegisteredL()
	{
	if(!iEnhancedLightSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);

	return *iEnhancedLightSessions[0];	
	}

inline CHWRMPower& CHWRMTestBase::GetPowerSessionWithCallbackRegisteredL()
	{
	if(!iPowerSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);
	
	return *iPowerSessions[0];
	}

inline CHWRMFmTx& CHWRMTestBase::GetFmTxSessionWithCallbackRegisteredL()
	{
	if(!iFmTxSessions[0]) //its always @ idx = 0
		User::Leave(KErrNotFound);

	return *iFmTxSessions[0];
	}

inline RMockLight& CHWRMTestBase::GetMockLight()
	{
	return iMockLight;
	}

inline RMockVibra& CHWRMTestBase::GetMockVibra()
	{
	return iMockVibra;
	}

inline RMockPowerState& CHWRMTestBase::GetMockPowerState()
	{
	return iMockPowerState;
	}

inline RMockPower& CHWRMTestBase::GetMockPower()
	{
	return iMockPower;
	}

inline RMockFmTx& CHWRMTestBase::GetMockFmTx()
	{
	return iMockFmTx;
	}

inline void CHWRMTestBase::SetTestFail()
	{
	if(!iTestResult)
		{
		iTestResult = KErrTEFUnitFail;
		}
	}

inline void CHWRMTestBase::ExpectLeave(TInt aError)
	{
	iExpectedLeaveCode = aError;
	}

inline void CHWRMTestBase::SetIgnoreStatusNotifications(TBool aIgnore)
	{
	iIgnoreStatusNotifications = aIgnore;
	}

inline TBool CHWRMTestBase::IgnoringStatusNotifications() const
	{
	return iIgnoreStatusNotifications;
	}

#endif // hwrmtest_h
