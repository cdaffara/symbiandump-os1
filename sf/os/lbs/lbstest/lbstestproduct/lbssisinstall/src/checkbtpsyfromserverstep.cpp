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
// Example CTestStep derived implementation
// 
//

/**
 @file checkbtpsyfromserverstep.cpp
 @internalTechnology
*/

#include "checkbtpsyfromserverstep.h"

CCheckBTPSYFromServerStep::~CCheckBTPSYFromServerStep()
/**
 * Destructor
 */
	{
	delete iScheduler;
	}

CCheckBTPSYFromServerStep::CCheckBTPSYFromServerStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KCheckBTPSYFromServerStep);
	}

TVerdict CCheckBTPSYFromServerStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsSisInstallStepBase::doTestStepPreambleL();
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);	
	return TestStepResult();
	}

TVerdict CCheckBTPSYFromServerStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		GetIntFromConfig(ConfigSection(), KTe_BTExpected, iExpectToFindBTPsy);
		LocationRequestTestL();
		}
	return TestStepResult();
	}

TVerdict CCheckBTPSYFromServerStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

void CCheckBTPSYFromServerStep::LocationRequestTestL()
	{
	TInt err = KErrNone;
	TPositionModuleInfo modInfo;
	RPositionServer posServer;
	TBool foundBTPsy = EFalse;

	INFO_PRINTF1(KOpenMLFWTxt);
	err = posServer.Connect();
	AssertTrueSecL(err == KErrNone, KMLFWNotOpenTxt, err);
	CleanupClosePushL(posServer);

	CPosModules* modules = CPosModules::OpenL();
	CleanupStack::PushL(modules);
	CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();

	CPosModuleIdList* idList = modules->ModuleIdListLC();
	for (TInt i = 0; i < idList->Count(); i++)
		{
		TPositionModuleInfo info;
		modules->GetModuleInfoL((*idList)[i], info);
		INFO_PRINTF2(_L("Module 0x%x"),info.ModuleId().iUid);

		if(info.ModuleId().iUid == btGPSPsyIdInt)
			{
			foundBTPsy = ETrue;
			if (!info.IsAvailable())
				{
				moduleUpdate->SetUpdateAvailability(ETrue);
				modules->UpdateModuleL(info.ModuleId(), *moduleUpdate);
				}
			continue;			
			}
		}
	
	if(iExpectToFindBTPsy && foundBTPsy)
		{
		INFO_PRINTF1(_L("Found BT GPS PSY, as expected. Getting module info\n"));
		err = posServer.GetModuleInfoById(btGPSPsyModuleId, modInfo);
		if(err == KErrNone)
			{
			CheckBTPSYModuleInfoL(modInfo);
			}
		else
			{
			INFO_PRINTF2(_L("Failed. GetModuleInfoById returned %d\n"), err);
			SetTestStepResult(EFail);
			}
		}
	else if(!iExpectToFindBTPsy && !foundBTPsy)
		{
		INFO_PRINTF1(_L("Did not find BT GPS PSY, as expected\n"));
		}
	else
		{
		INFO_PRINTF3(_L("Failed. Expect to find BT? %d. Found BT? %d\n"), iExpectToFindBTPsy, foundBTPsy);
		SetTestStepResult(EFail);
		}

	if (err!=KErrNone)
		{
		SetTestStepError(err);
		}

	CleanupStack::PopAndDestroy(4, &posServer); // posServer
	}

void CCheckBTPSYFromServerStep::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
	{
	if (!aCondition)
		{
		TBuf<KMaxLogBuffer> buf;
		buf.Format(aErrorMsg, aErrorCode);
		ERR_PRINTF1(buf);
		SetTestStepResult(EFail);
		User::LeaveIfError(aErrorCode);
		}
	}

void CCheckBTPSYFromServerStep::CheckBTPSYModuleInfoL(TPositionModuleInfo aInfo)
	{
	TBuf<KMaxLogBuffer> name;

	aInfo.GetModuleName(name);

	INFO_PRINTF1( name );

	AssertTrueSecL(KErrNone == name.Compare( _L( "Bluetooth GPS" )), _L( "Wrong name for PSY" ), KErrNone);

	AssertTrueSecL(aInfo.ModuleId() == btGPSPsyModuleId, _L( "Wrong module id: %d" ), aInfo.ModuleId().iUid);
	AssertTrueSecL(aInfo.IsAvailable(), _L("Not available"), KErrNone);
	AssertTrueSecL(aInfo.Version().iMajor == 60, _L("Wrong version should be 60 is %d"), aInfo.Version().iMajor);
	AssertTrueSecL(aInfo.TechnologyType()
			== TPositionModuleInfo::ETechnologyTerminal, 
	_L("Not a terminal, but: %d"), aInfo.TechnologyType() );
	AssertTrueSecL(aInfo.DeviceLocation()
			== TPositionModuleInfo::EDeviceExternal, 
	_L("Not external"), aInfo.DeviceLocation() );
	AssertTrueSecL(aInfo.ClassesSupported(EPositionInfoFamily)
			== (EPositionInfoClass | EPositionGenericInfoClass
					| EPositionCourseInfoClass | EPositionSatelliteInfoClass ),
			_L("Not expected classes supported: %d"),
			aInfo.ClassesSupported(EPositionInfoFamily) );
	TPositionModuleInfo::TCapabilities capability =
			(TPositionModuleInfo::ECapabilityHorizontal
					| TPositionModuleInfo::ECapabilityVertical
					| TPositionModuleInfo::ECapabilitySpeed
					| TPositionModuleInfo::ECapabilityDirection
					| TPositionModuleInfo::ECapabilitySatellite
					| TPositionModuleInfo::ECapabilityNmea );
	AssertTrueSecL(aInfo.Capabilities() == capability, _L("No correct capabilities: %d"), aInfo.Capabilities() );

	TPositionQuality modQ;
	aInfo.GetPositionQuality(modQ);

	TInt32 hAcc = 0;
	TInt32 vAcc = 0;
	Math::Int(hAcc, modQ.HorizontalAccuracy() );
	Math::Int(vAcc, modQ.VerticalAccuracy() );

	AssertTrueSecL(
			modQ.TimeToFirstFix() == TTimeIntervalMicroSeconds(80000000), _L("Wrong TTFF value %d, wanted 80"),
			(TInt) modQ.TimeToFirstFix().Int64());
	AssertTrueSecL(modQ.TimeToNextFix() == TTimeIntervalMicroSeconds(1000000),
			_L("Wrong TTNF value %d, wanted 1"), modQ.TimeToNextFix().Int64());
	AssertTrueSecL(modQ.HorizontalAccuracy() == 10.0, _L(""), hAcc);
	AssertTrueSecL(modQ.VerticalAccuracy() == 30.0, _L(""), vAcc);
	AssertTrueSecL(modQ.CostIndicator() == TPositionQuality::ECostZero, _L(""), modQ.CostIndicator() );
	AssertTrueSecL(modQ.PowerConsumption() == TPositionQuality::EPowerMedium,
			_L(""), modQ.PowerConsumption() );
	}

// End of file
