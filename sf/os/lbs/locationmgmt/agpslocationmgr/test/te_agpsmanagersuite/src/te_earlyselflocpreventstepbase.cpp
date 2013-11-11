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
//

/**
 @file te_earlyselflocpreventstepbase.cpp
*/
 
#include "te_earlyselflocpreventstepbase.h"
#include "te_agpsmanagersuitedefs.h"
#include <lbs/lbslocdatasourceclasstypes.h>


CTe_EarlySelfLocPreventStepBase::CTe_EarlySelfLocPreventStepBase() : iOriginalMode(CLbsAdmin::EGpsModeUnknown)
	{
	}

TVerdict CTe_EarlySelfLocPreventStepBase::doTestStepL()
	{
	if(TestStepResult()!=EPass)
		{
		return TestStepResult();
		}
	
	TRAPD(err, doTestPreambleImplL());
	if(err==KErrNone)
		{
		TRAP(err, doTestStepImplL());
		if(err!=KErrNone)
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	if(iAdmin)
		{
		if(iOriginalMode!=CLbsAdmin::EGpsModeUnknown)
			{
			// Reset Admin to to value it had before the test changed it. 
			if(iAdmin->Set(KLbsSettingRoamingGpsMode, iOriginalMode)!=KErrNone)
				{
				SetTestStepResult(EFail);
				}
			}
		
		delete iAdmin;
		iAdmin = NULL;
		}
	
	return TestStepResult();
	}

void CTe_EarlySelfLocPreventStepBase::doTestPreambleImplL()
	{
	iAdmin = CLbsAdmin::NewL();
	TESTL(KErrNone==iAdmin->Get(KLbsSettingRoamingGpsMode, iOriginalMode));
	TESTL(KErrNone==iAdmin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
				
	// Configure H/W Capabilities as Terminal Based only
	TESTL(KErrNone==iCapabilitiesProperty.Set(KManagerTestSuiteUid, CTe_AgpsManagerStepBase::ETestCapabilitiesKey,
			TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased));
	
	iRequestedTargetTimeNRH.UniversalTime();
		
	iRequestedQuality.SetMaxFixTime(KStandardMaxFixTime);
	iRequestedQuality.SetMinHorizontalAccuracy(KStandardAccuracy);
	iRequestedQuality.SetMinVerticalAccuracy(KStandardAccuracy);
			
	TLbsNetPosMethodInt posMethods[1];
	posMethods[0].SetPosMethod(KLbsPositioningMeansGps,
			TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
	iRequestedPosMethods.SetPosMethods(posMethods, 1);
			
	iResultedStandardPositionPtr = &iResultedStandardPosition;
	iResultedStandardPosition.SetUpdateType(EPositionUpdateGeneral);
	iResultedStandardPosition.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal | 
				TPositionModuleInfo::ETechnologyAssisted);
	iResultedStandardPosition.SetPositionModeReason(EPositionModeReasonNone);
	
	TPosition resultedStandardPos;
	resultedStandardPos.SetCurrentTime();
	resultedStandardPos.SetAccuracy(KHighAccuracy, KHighAccuracy);
	resultedStandardPos.SetCoordinate(KStandardLatitude,KStandardLongitude);
	iResultedStandardPosition.SetPosition(resultedStandardPos);
	}
