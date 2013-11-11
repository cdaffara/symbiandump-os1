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
// This is the class implementation for the Hybrid MoLr Tests
// 
//


// EPOC includes.
#include <e32math.h>

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
// LBS test includes.
#include "ctlbsuebasedmolrself.h"
#include "ctlbsueassistedmolrselfgpsinaccurate.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

void CT_LbsUEAssistedMOLRSelfGPSInaccurate::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify position.


	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}

	// Expect either the final network A-GPS position, depending on test case.
	else if (iState == ERefLocReceived)
		{
		iState = EFinalLocReceived;

		TEST(aErr == KPositionQualityLoss);
		
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				// Expect the final network position.
				case 1:
					{
					T_LbsUtils utils;
					TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
		
					TEST(utils.Compare_PosInfo(aPosInfo, networkPosInfo));
					TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
					TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
					TEST(aPosInfo.ModuleId() == TUid::Uid(0x1028fABE)); // id of Network PSY
					}
					break;
				
				// Expect the A-GPS position.
				case 2:
					{
					TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
					TEST(aPosInfo.PositionClassType() == EPositionInfoClass | EPositionCourseInfoClass | EPositionSatelliteInfoClass);
					TEST(aPosInfo.ModuleId() == TUid::Uid(0x10281D44)); // id of AGPSManager

					}
					break;
				
				default:
					TEST(EFalse);
					break;
				}
			}
		
		else
			{
			TEST(EFalse);
			}	
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

