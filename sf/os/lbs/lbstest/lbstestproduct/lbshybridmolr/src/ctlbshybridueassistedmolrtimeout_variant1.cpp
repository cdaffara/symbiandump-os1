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
// This is the class implementation for the Hybrid Extended Client Timeout Tests
// EPOC includes.
// 
//

// LBS includes.
#include <lbs/lbsloccommon.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridueassistedmolrtimeout.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

void CT_LbsHybridUEAssistedMOLRTimeOut::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// This is determined by the client call to NotifyPositionUpdate, may as well check.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		
		
	// Verify error + position.

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		
		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting gps pos.
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
					TEST(aPosInfo.ModuleId() == TUid::Uid(0x10281D43)); // id of NetLocManager
					}
					break;
				
				// Expect the A-GPS position.
				case 2:
				case 3:
					{
					// Verify A-GPS position values, hardcoded from the ini file (this should be automated really).
					const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(aPosInfo);
					TPosition pos;
					posInfo.GetPosition(pos);
					
					TESTL((pos.Latitude() == 49.2) && (pos.Longitude() == 3.5) && (pos.Altitude() == 50.0));
					if (testCaseId == 2)
						{
						TESTL((pos.HorizontalAccuracy() == 80) && (pos.VerticalAccuracy() == 80));
						}
						
					else
						{
						TESTL((pos.HorizontalAccuracy() == 2000) && (pos.VerticalAccuracy() == 2000));
						}
					
					TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
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

