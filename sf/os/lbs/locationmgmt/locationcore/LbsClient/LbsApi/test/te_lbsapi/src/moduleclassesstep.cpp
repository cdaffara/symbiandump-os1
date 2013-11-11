// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file ModuleClassesStep.cpp
 @internalTechnology
*/
#include "moduleclassesstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>

CModuleClassesStep::~CModuleClassesStep()
/**
 * Destructor
 */
	{
	}

CModuleClassesStep::CModuleClassesStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KModuleClassesStep);
	}

TVerdict CModuleClassesStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsApiSuiteStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CModuleClassesStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsApiSuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}



TVerdict CModuleClassesStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


void CModuleClassesStep::StartL(TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		TPositionModuleInfo_ConstructionL ();
		break;
	case 1:
		TPositionModuleInfo_GettersL ();
		break;
	case 2:
		TPositionModuleInfo_InvalidL ();
		break;
	case 3:
		TPositionQuality_ConstructionL ();
		break;
	case 4:
		TPositionQuality_SettersL ();
		break;
	case 5:
		TPositionQuality_InvalidL ();
		break;
	case 6:
		TPositionQuality_ComparisonL ();
		break;
	case 7:
		ModuleClasses_ConstructionL ();
		break;
	case 8:
		ModuleClasses_SettersL ();
		break;
	case 9:
		TPositionUpdateOptions_InvalidL ();
		break;
	default:
		break;
		}
	}


//
//
// TPositionModuleInfo
//
//

// 6.1.1. Construction
void CModuleClassesStep::TPositionModuleInfo_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    TPositionModuleInfo posModInfo;

    CHECK_EQUAL_(sizeof(TPositionModuleInfo), posModInfo.PositionClassSize());
    CHECK(EPositionModuleInfoClass == posModInfo.PositionClassType());

    CHECK(KPositionNullModuleId.iUid == posModInfo.ModuleId().iUid);

    TPositionQuality posQuality;
    posModInfo.GetPositionQuality(posQuality);
    CHECK(Math::IsNaN(posQuality.HorizontalAccuracy())); // new pos quality

    CHECK(!posModInfo.IsAvailable());

    TBuf<KPositionMaxModuleName> moduleName;
    posModInfo.GetModuleName(moduleName);
    CHECK_EQUAL_(0, moduleName.Length());

    CHECK_EQUAL_(TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyUnknown),
                            posModInfo.TechnologyType());
    CHECK_EQUAL_(TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceUnknown),
                            posModInfo.DeviceLocation());
    CHECK_EQUAL_(TPositionModuleInfo::TCapabilities(TPositionModuleInfo::ECapabilityNone),
                            posModInfo.Capabilities());

    for(TInt i=0; i<EPositionLastFamily; i++)
            CHECK(0 == posModInfo.ClassesSupported(TPositionClassFamily(i)));

    TVersion version = posModInfo.Version();
    TVersion defaultVersion;
    CHECK_EQUAL_(version.iMajor, defaultVersion.iMajor);
    CHECK_EQUAL_(version.iMinor, defaultVersion.iMinor);
    CHECK_EQUAL_(version.iBuild, defaultVersion.iBuild);

    StandardCleanup();
	}

// 6.1.2. Set and Get
void CModuleClassesStep::TPositionModuleInfo_GettersL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    TPositionModuleInfo posModInfo;

    const TPositionModuleId KDummyModuleId = {0x32};
    posModInfo.SetModuleId(KDummyModuleId);
    CHECK_EQUAL_(KDummyModuleId.iUid, posModInfo.ModuleId().iUid);

    posModInfo.SetIsAvailable(ETrue);
    CHECK(posModInfo.IsAvailable());
    posModInfo.SetIsAvailable(EFalse);
    CHECK(!posModInfo.IsAvailable());

    _LIT(KDummyModuleName, "Testing 1,2,3");
    posModInfo.SetModuleName(KDummyModuleName);
    TBuf<KPositionMaxModuleName> tempBuf;
    posModInfo.GetModuleName(tempBuf);
    CHECK(0 == tempBuf.Compare(KDummyModuleName));

    TPositionQuality quality, quality2;
    const TPositionQuality::TCostIndicator KDummyCostIndicator = TPositionQuality::ECostCharge;
    quality.SetCostIndicator(KDummyCostIndicator);
    posModInfo.SetPositionQuality(quality);

    posModInfo.GetPositionQuality(quality2);
    CHECK_EQUAL_(KDummyCostIndicator, quality2.CostIndicator());

    posModInfo.SetTechnologyType(TPositionModuleInfo::ETechnologyNetwork);
    posModInfo.SetDeviceLocation(TPositionModuleInfo::EDeviceExternal);

    CHECK_EQUAL_(TPositionModuleInfo::TTechnologyType(TPositionModuleInfo::ETechnologyNetwork),
                            posModInfo.TechnologyType());
    CHECK_EQUAL_(TPositionModuleInfo::TDeviceLocation(TPositionModuleInfo::EDeviceExternal),
                            posModInfo.DeviceLocation());

    const TPositionModuleInfo::TCapabilities KDummyCaps =
            TPositionModuleInfo::ECapabilitySpeed |
            TPositionModuleInfo::ECapabilityNmea;

    posModInfo.SetCapabilities(KDummyCaps);
    CHECK_EQUAL_(KDummyCaps, posModInfo.Capabilities());

    for(TUint i=0; i<EPositionLastFamily; i++)
            posModInfo.SetClassesSupported(TPositionClassFamily(i), 2*i);

    for(TUint j=0; j<EPositionLastFamily; j++)
            CHECK(2*j == posModInfo.ClassesSupported(TPositionClassFamily(j)));

    TVersion KDummyVersion(12,31,45);        //Should be const, but Name isn't a const method for some reason!
    posModInfo.SetVersion(KDummyVersion);
    CHECK(posModInfo.Version().Name()==KDummyVersion.Name());

    StandardCleanup();
	}

// 6.1.3. Invalid calls to Setters

TInt Set_Bad_Class_Type_TPositionModuleInfo(TAny* /*aPtr*/)
    {
    TPositionModuleInfo modInfo;
    modInfo.SetClassesSupported(EPositionLastFamily, 0);        //This should panic
    return KErrGeneral;
    }


TInt Get_Bad_Class_Type_TPositionModuleInfo(TAny* /*aPtr*/)
    {
    TPositionModuleInfo modInfo;
    (void)modInfo.ClassesSupported(EPositionLastFamily);        //This should panic
    return KErrGeneral;
    }

void CModuleClassesStep::TPositionModuleInfo_InvalidL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    DO_PANIC_TEST_L(Set_Bad_Class_Type_TPositionModuleInfo, KPosClientFault,
                                EPositionInvalidClassType, KDefaultTestTimeout);
    DO_PANIC_TEST_L(Get_Bad_Class_Type_TPositionModuleInfo, KPosClientFault,
                                EPositionInvalidClassType, KDefaultTestTimeout);

    StandardCleanup();
	}

//
//
// TPositionQuality
//
//

// 6.2.1. Construction
void CModuleClassesStep::TPositionQuality_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    TPositionQuality quality;

    CHECK(Math::IsNaN(quality.HorizontalAccuracy()));
    CHECK(Math::IsNaN(quality.VerticalAccuracy()));

    CHECK(quality.PositionClassSize() == sizeof(TPositionQuality));
    CHECK(quality.PositionClassType() == EPositionQualityClass);

    CHECK(quality.TimeToFirstFix()   == TTimeIntervalMicroSeconds(TInt64(0)));
    CHECK(quality.TimeToNextFix()    == TTimeIntervalMicroSeconds(TInt64(0)));
    CHECK(quality.CostIndicator()    == TPositionQuality::ECostUnknown);
    CHECK(quality.PowerConsumption() == TPositionQuality::EPowerUnknown);

    StandardCleanup();
	}

// 6.2.2. Getter and Setters
void CModuleClassesStep::TPositionQuality_SettersL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    TPositionQuality posQuality;

    // "time to" fixes
    const TTimeIntervalMicroSeconds KTTFF(3425234);
    const TTimeIntervalMicroSeconds KTTNF(45256265);
    const TReal32 KHorizontalAccuracy(TReal32(34252.23452));
    const TReal32 KVerticalAccuracy(TReal32(423.456563));
    const TPositionQuality::TCostIndicator KDummyCost(TPositionQuality::ECostPossible);
    const TPositionQuality::TPowerConsumption KDummyPowerConsumption(TPositionQuality::EPowerMedium);

    posQuality.SetTimeToFirstFix(KTTFF);
    posQuality.SetTimeToNextFix(KTTNF);
    posQuality.SetHorizontalAccuracy(KHorizontalAccuracy);
    posQuality.SetVerticalAccuracy(KVerticalAccuracy);
    posQuality.SetCostIndicator(KDummyCost);
    posQuality.SetPowerConsumption(KDummyPowerConsumption);

    CHECK_EQUAL_(posQuality.TimeToFirstFix().Int64(),    KTTFF.Int64());
    CHECK_EQUAL_(posQuality.TimeToNextFix().Int64(),     KTTNF.Int64());
    CHECK_EQUAL_(posQuality.HorizontalAccuracy(),        KHorizontalAccuracy);
    CHECK_EQUAL_(posQuality.VerticalAccuracy(),          KVerticalAccuracy);
    CHECK_EQUAL_(posQuality.CostIndicator(),             KDummyCost);
    CHECK_EQUAL_(posQuality.PowerConsumption(),          KDummyPowerConsumption);

    // NaNs
    TRealX nan;
    nan.SetNaN();

    posQuality.SetHorizontalAccuracy(nan);
    posQuality.SetVerticalAccuracy(nan);

    CHECK(Math::IsNaN(posQuality.HorizontalAccuracy()));
    CHECK(Math::IsNaN(posQuality.VerticalAccuracy()));

    // "Zeroes"
    posQuality.SetTimeToFirstFix(TTimeIntervalMicroSeconds(0));
    posQuality.SetTimeToNextFix(TTimeIntervalMicroSeconds(0));
    posQuality.SetCostIndicator(TPositionQuality::ECostUnknown);
    posQuality.SetPowerConsumption(TPositionQuality::EPowerUnknown);

    CHECK_EQUAL_(posQuality.TimeToFirstFix().Int64(),    TInt64(0));
    CHECK_EQUAL_(posQuality.TimeToNextFix().Int64(),     TInt64(0));
    CHECK_EQUAL_(posQuality.CostIndicator(),             TPositionQuality::ECostUnknown);
    CHECK_EQUAL_(posQuality.PowerConsumption(),          TPositionQuality::EPowerUnknown);

    StandardCleanup();
	}

// 6.2.3. Invalid Input
TInt TPositionQuality_Set_Negative_HAccuracy(TAny* /*aPtr*/)
	{
    TPositionQuality quality;
    quality.SetHorizontalAccuracy(TReal32(-0.1));
    return KErrGeneral;
	}


TInt TPositionQuality_Set_Negative_VAccuracy(TAny* /*aPtr*/)
	{
    TPositionQuality quality;
    quality.SetVerticalAccuracy(TReal32(-0.1));
    return KErrGeneral;
	}


TInt TPositionQuality_Set_Negative_TTFirstFix(TAny* /*aPtr*/)
	{
    TPositionQuality quality;
    quality.SetTimeToFirstFix(TTimeIntervalMicroSeconds(-1));
    return KErrGeneral;
	}


TInt TPositionQuality_Set_Negative_TTNextFix(TAny* /*aPtr*/)
	{
    TPositionQuality quality;
    quality.SetTimeToNextFix(TTimeIntervalMicroSeconds(-1));
    return KErrGeneral;
	}


void CModuleClassesStep::TPositionQuality_InvalidL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    DO_PANIC_TEST_L(TPositionQuality_Set_Negative_HAccuracy,
            KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TPositionQuality_Set_Negative_VAccuracy,
            KPosClientFault, EPositionBadAccuracy, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TPositionQuality_Set_Negative_TTFirstFix,
            KPosClientFault, EPositionBadTime, KDefaultTestTimeout);
    DO_PANIC_TEST_L(TPositionQuality_Set_Negative_TTNextFix,
            KPosClientFault, EPositionBadTime, KDefaultTestTimeout);

    StandardCleanup();
	}

// 6.2.4 Element Comparison
#define SET_ELEMENT(aId, aType, aPref, aData)   \
    if (aId < KPositionMaxSectionFields) SetElement(aId, aType, aPref, aData);

#pragma warning (push)
#pragma warning (disable: 4127)

void CModuleClassesStep::TPositionQuality_ComparisonL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    TPositionQuality q1, q2;
    TPositionQualityItem::TResult result;

    // set high values
    q1.SetTimeToFirstFix(       TTimeIntervalMicroSeconds(20000));
    q1.SetTimeToNextFix(        TTimeIntervalMicroSeconds(10000));
    q1.SetHorizontalAccuracy(   TReal32(1000));
    q1.SetVerticalAccuracy(     TReal32(2000));
    q1.SetCostIndicator(        TPositionQuality::ECostCharge);
    q1.SetPowerConsumption(     TPositionQuality::EPowerHigh);

    // set low values
    q2.SetTimeToFirstFix(       TTimeIntervalMicroSeconds(2));
    q2.SetTimeToNextFix(        TTimeIntervalMicroSeconds(1));
    q2.SetHorizontalAccuracy(   TReal32(1));
    q2.SetVerticalAccuracy(     TReal32(2));
    q2.SetCostIndicator(        TPositionQuality::ECostZero);
    q2.SetPowerConsumption(     TPositionQuality::EPowerZero);

    for (TInt i = 0; i < q1.HighWaterMark() + 1; i++)
    	{
        q1.Compare(q2, i, result);
        CHECK_EQUAL_(TPositionQualityItem::EIsBetter, result);
    	}

    // more common test :-)

    class TBigQuality1 : public TPositionQualityBase {
        public:
            TBigQuality1()
            	{
                SET_ELEMENT(0, TPositionQualityItem::EUndefined,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt8(100));
                SET_ELEMENT(1, TPositionQualityItem::ETInt8,
                                                  TPositionQualityItem::EPreferSmallerValues, TInt8(100));
                SET_ELEMENT(2, TPositionQualityItem::ETInt16,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt16(100));
                SET_ELEMENT(3, TPositionQualityItem::ETInt32,
                                                  TPositionQualityItem::EPreferSmallerValues, TInt32(100));
                SET_ELEMENT(4, TPositionQualityItem::ETInt64,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt64(100));
                SET_ELEMENT(5, TPositionQualityItem::ETUint8,
                                                  TPositionQualityItem::EPreferSmallerValues, TUint8(100));
                SET_ELEMENT(6, TPositionQualityItem::ETUint16,
                                                  TPositionQualityItem::EPreferGreaterValues, TUint16(100));
                SET_ELEMENT(7, TPositionQualityItem::ETUint32,
                                                  TPositionQualityItem::EPreferSmallerValues, TUint32(100));
            	};
    } bigQ1;
    class TBigQuality2 : public TPositionQualityBase {
        public:
            TBigQuality2()
            	{
                SET_ELEMENT(0, TPositionQualityItem::ETReal32,
                                                  TPositionQualityItem::EPreferGreaterValues, TReal32(100));
                SET_ELEMENT(1, TPositionQualityItem::ETReal64,
                                                  TPositionQualityItem::EPreferSmallerValues, TReal64(100));
                SET_ELEMENT(2,TPositionQualityItem::ETTime,
                                                  TPositionQualityItem::EPreferGreaterValues, TTime(100));
                SET_ELEMENT(3,TPositionQualityItem::ETTimeIntervalMicroSeconds,
                                                  TPositionQualityItem::EPreferSmallerValues, TTimeIntervalMicroSeconds(100));
            	};
    } bigQ2;

    class TLowQuality1 : public TPositionQualityBase {
        public:
            TLowQuality1()
            	{
                SET_ELEMENT(0, TPositionQualityItem::EUndefined,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt8(1));
                SET_ELEMENT(1, TPositionQualityItem::ETInt8,
                                                  TPositionQualityItem::EPreferSmallerValues, TInt8(1));
                SET_ELEMENT(2, TPositionQualityItem::ETInt16,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt16(1));
                SET_ELEMENT(3, TPositionQualityItem::ETInt32,
                                                  TPositionQualityItem::EPreferSmallerValues, TInt32(1));
                SET_ELEMENT(4, TPositionQualityItem::ETInt64,
                                                  TPositionQualityItem::EPreferGreaterValues, TInt64(1));
                SET_ELEMENT(5, TPositionQualityItem::ETUint8,
                                                  TPositionQualityItem::EPreferSmallerValues, TUint8(1));
                SET_ELEMENT(6, TPositionQualityItem::ETUint16,
                                                  TPositionQualityItem::EPreferGreaterValues, TUint16(1));
                SET_ELEMENT(7, TPositionQualityItem::ETUint32,
                                                  TPositionQualityItem::EPreferSmallerValues, TUint32(1));
            	};
    } lowQ1;
    class TLowQuality2 : public TPositionQualityBase {
        public:
            TLowQuality2()
            	{
                SET_ELEMENT(0, TPositionQualityItem::ETReal32,
                                                   TPositionQualityItem::EPreferGreaterValues, TReal32(1));
                SET_ELEMENT(1, TPositionQualityItem::ETReal64,
                                                   TPositionQualityItem::EPreferSmallerValues, TReal64(1));
                SET_ELEMENT(2, TPositionQualityItem::ETTime,
                                                   TPositionQualityItem::EPreferGreaterValues, TTime(1));
                SET_ELEMENT(3, TPositionQualityItem::ETTimeIntervalMicroSeconds,
                                                   TPositionQualityItem::EPreferSmallerValues, TTimeIntervalMicroSeconds(1));
            	};
    } lowQ2;

    // 1st part
    TInt j = 0;
    for (j = 0; j < lowQ1.HighWaterMark() + 1; j++)
    	{
        lowQ1.Compare(bigQ1, j, result);

        if (j % 2 == 0)
        	{
            // we prefer greater values here
            CHECK_EQUAL_(TPositionQualityItem::EIsBetter, result);
        	}
        else // odd ids
        	{
            // we prefer smaller values here
            CHECK_EQUAL_(TPositionQualityItem::EIsWorse, result);
        	}
    	}
    // same for the 2nd part
    for (j = 0; j < lowQ2.HighWaterMark() + 1; j++)
    	{
        lowQ2.Compare(bigQ2, j, result);
        TPositionQualityItem::TResult pref;
        pref = (j % 2 == 0) ? TPositionQualityItem::EIsBetter : TPositionQualityItem::EIsWorse;
        CHECK_EQUAL_(pref, result);
    	}

    // specific test for different preferences and data types

    class TQualityGreaterSigned : public TPositionQualityBase {
        public:
            TQualityGreaterSigned()
            	{
                SET_ELEMENT(0, TPositionQualityItem::ETInt8,
                                                   TPositionQualityItem::EPreferGreaterValues, TInt8(1));
                SET_ELEMENT(1, TPositionQualityItem::ETInt8,
                                                   TPositionQualityItem::EPreferGreaterValues, TInt8(2));
                SET_ELEMENT(2, TPositionQualityItem::ETInt8,
                                                   TPositionQualityItem::EPreferGreaterValues, TInt8(3));
            	};
    } qGS;

    class TQualityReverse : public TPositionQualityBase {
        public:
            TQualityReverse()
            	{
                SET_ELEMENT(0, TPositionQualityItem::ETInt8,
                                                   TPositionQualityItem::EPreferSmallerValues, TInt8(1));
                SET_ELEMENT(1, TPositionQualityItem::ETUint8,
                                                   TPositionQualityItem::EPreferGreaterValues, TUint8(2));
                SET_ELEMENT(2, TPositionQualityItem::ETUint8,
                                                   TPositionQualityItem::EPreferSmallerValues, TUint8(3));

                SET_ELEMENT(3, TPositionQualityItem::ETUint8,
                                                   TPositionQualityItem::EPreferSmallerValues, TUint8(3));
                ResetElement(3);
            	};
    } qR;

    // in all cases KErrArgument should be returned
    CHECK_EQUAL_(KErrArgument, qGS.Compare(qR, 0, result));
    CHECK_EQUAL_(KErrArgument, qGS.Compare(qR, 1, result));
    CHECK_EQUAL_(KErrArgument, qGS.Compare(qR, 2, result));

    StandardCleanup();
	}
#pragma warning (pop)

//
//
// Module Updates, Status, Events classes
//
//

// 7.1.1. Construction
void CModuleClassesStep::ModuleClasses_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // Update Options
    TPositionUpdateOptions updateOptions;

    CHECK(updateOptions.PositionClassSize() == sizeof(TPositionUpdateOptions));
    CHECK(updateOptions.PositionClassType() == EPositionUpdateOptionsClass);

    CHECK_EQUAL_(TInt64(0), updateOptions.UpdateInterval().Int64());
    CHECK_EQUAL_(TInt64(0), updateOptions.UpdateTimeOut().Int64());
    CHECK_EQUAL_(TInt64(0), updateOptions.MaxUpdateAge().Int64());
    CHECK(!updateOptions.AcceptPartialUpdates());

    const TBool KDummyAcceptUpdates = ETrue;
    const TTimeIntervalMicroSeconds KUpdateInterval(23412);
    const TTimeIntervalMicroSeconds KTimeOut(4123523);
    const TTimeIntervalMicroSeconds KMaxUpdateAge(3245);

    TPositionUpdateOptions updateOptions2(
        KUpdateInterval,
        KTimeOut,
        KMaxUpdateAge,
        KDummyAcceptUpdates);

    CHECK_EQUAL_(KUpdateInterval, updateOptions2.UpdateInterval());
    CHECK_EQUAL_(KTimeOut, updateOptions2.UpdateTimeOut().Int64());
    CHECK_EQUAL_(KMaxUpdateAge, updateOptions2.MaxUpdateAge().Int64());
    CHECK(updateOptions2.AcceptPartialUpdates());

    // Module Status
    TPositionModuleStatus moduleStatus;

    CHECK(moduleStatus.PositionClassSize()  == sizeof(TPositionModuleStatus));
    CHECK(moduleStatus.PositionClassType()  == EPositionModuleStatusClass);

    CHECK(moduleStatus.DeviceStatus()       == TPositionModuleStatus::EDeviceUnknown);
    CHECK(moduleStatus.DataQualityStatus()  == TPositionModuleStatus::EDataQualityUnknown);

    // Module status event
    TPositionModuleStatusEvent posModEvent;

    CHECK(posModEvent.PositionClassSize()   == sizeof(TPositionModuleStatusEvent));
    CHECK(posModEvent.PositionClassType()   == EPositionModuleStatusEventClass);

    CHECK(posModEvent.RequestedEvents()     == TPositionModuleStatusEvent::EEventNone);
    CHECK(posModEvent.OccurredEvents()      == TPositionModuleStatusEvent::EEventNone);

    posModEvent.GetModuleStatus(moduleStatus);

    CHECK(moduleStatus.DeviceStatus()       == TPositionModuleStatus::EDeviceUnknown);
    CHECK(moduleStatus.DataQualityStatus()  == TPositionModuleStatus::EDataQualityUnknown);

    //Just do a check here for construction with requested events parameter.
    TPositionModuleStatusEvent posModEvent2(TPositionModuleStatusEvent::EEventDataQualityStatus);
    CHECK(posModEvent2.RequestedEvents() == TPositionModuleStatusEvent::EEventDataQualityStatus);

    StandardCleanup();
	}

// 7.1.2. Setters/Getters
void CModuleClassesStep::ModuleClasses_SettersL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // Update Options
    TPositionUpdateOptions updateOptions;

    const TBool KDummyAcceptUpdates = ETrue;
    const TTimeIntervalMicroSeconds KUpdateInterval(23412);
    const TTimeIntervalMicroSeconds KTimeOut(4123523);
    const TTimeIntervalMicroSeconds KMaxUpdateAge(3245);

    updateOptions.SetAcceptPartialUpdates(KDummyAcceptUpdates);
    updateOptions.SetUpdateInterval(KUpdateInterval);
    updateOptions.SetUpdateTimeOut(KTimeOut);
    updateOptions.SetMaxUpdateAge(KMaxUpdateAge);

    CHECK(updateOptions.AcceptPartialUpdates());
    CHECK_EQUAL_(updateOptions.UpdateInterval().Int64(), KUpdateInterval.Int64());
    CHECK_EQUAL_(updateOptions.UpdateTimeOut().Int64(),  KTimeOut.Int64());
    CHECK_EQUAL_(updateOptions.MaxUpdateAge().Int64(),   KMaxUpdateAge.Int64());

    // Module status
    TPositionModuleStatus moduleStatus;

    const TPositionModuleStatus::TDeviceStatus
            KDummyDeviceStatus(TPositionModuleStatus::EDeviceInactive);
    const TPositionModuleStatus::TDataQualityStatus
            KDummyQualityStatus(TPositionModuleStatus::EDataQualityPartial);

    moduleStatus.SetDeviceStatus(KDummyDeviceStatus);
    moduleStatus.SetDataQualityStatus(KDummyQualityStatus);

    CHECK_EQUAL_(KDummyDeviceStatus,     moduleStatus.DeviceStatus());
    CHECK_EQUAL_(KDummyQualityStatus,    moduleStatus.DataQualityStatus());

    // Module status event
    TPositionModuleStatusEvent posModEvent;

    const TPositionModuleId KDummyModuleId = {0x00355463};
    const TPositionModuleStatusEvent::TModuleEvent
            KDummyRequestedEvent(TPositionModuleStatusEvent::EEventDeviceStatus);
    const TPositionModuleStatusEvent::TModuleEvent
            KDummyOccurredEvent(TPositionModuleStatusEvent::EEventDataQualityStatus);
    const TPositionModuleStatusEvent::TSystemModuleEvent
            KDummySystemEvent(TPositionModuleStatusEvent::ESystemModuleRemoved);

    posModEvent.SetModuleId(KDummyModuleId);
    posModEvent.SetRequestedEvents(KDummyRequestedEvent);
    posModEvent.SetOccurredEvents(KDummyOccurredEvent);
    posModEvent.SetSystemModuleEvent(KDummySystemEvent);

    CHECK_EQUAL_(KDummyModuleId.iUid,    posModEvent.ModuleId().iUid);
    CHECK_EQUAL_(KDummyRequestedEvent,   posModEvent.RequestedEvents());
    CHECK_EQUAL_(KDummyOccurredEvent,    posModEvent.OccurredEvents());
    CHECK_EQUAL_(KDummySystemEvent,      posModEvent.SystemModuleEvent());

    TPositionModuleStatus modStatus2;
    posModEvent.SetModuleStatus(moduleStatus);
    posModEvent.GetModuleStatus(modStatus2);

    CHECK_EQUAL_(modStatus2.DeviceStatus(),      KDummyDeviceStatus);
    CHECK_EQUAL_(modStatus2.DataQualityStatus(), KDummyQualityStatus);

    StandardCleanup();
	}

// 7.2.1. Invalid Input in TPositionUpdateOptions
const TInt timeNEG(-1);
const TInt timeZero(-1);


TInt NegativeTimeConstructionInterval(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions(timeNEG, timeZero, timeZero);
    return KErrGeneral;
	}


TInt NegativeTimeConstructionTimeOut(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions(timeZero, timeNEG, timeZero);
    return KErrGeneral;
	}


TInt NegativeTimeConstructionMaxAge(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions(timeZero, timeZero, timeNEG);
    return KErrGeneral;
	}


TInt NegativeTimeSettingInterval(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateInterval(timeNEG);
    return KErrGeneral;
	}


TInt NegativeTimeSettingTimeOut(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateTimeOut(timeNEG);
    return KErrGeneral;
	}


TInt NegativeTimeSettingMaxAge(TAny* /*aPtr*/)
	{
    TPositionUpdateOptions updateOptions;
    updateOptions.SetMaxUpdateAge(timeNEG);
    return KErrGeneral;
	}


void CModuleClassesStep::TPositionUpdateOptions_InvalidL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // construction
    DO_PANIC_TEST_L(NegativeTimeConstructionInterval, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NegativeTimeConstructionTimeOut, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NegativeTimeConstructionMaxAge, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);

    // setting
    DO_PANIC_TEST_L(NegativeTimeSettingInterval, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NegativeTimeSettingTimeOut, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);
    DO_PANIC_TEST_L(NegativeTimeSettingMaxAge, KPosClientFault,
                                    EPositionBadTime, KDefaultTestTimeout);

    StandardCleanup();
	}

