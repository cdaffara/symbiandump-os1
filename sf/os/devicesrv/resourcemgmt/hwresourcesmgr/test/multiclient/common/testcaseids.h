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

#ifndef TESTCASEIDS_H
#define TESTCASEIDS_H

enum THWRMLightTestFAPIType
    {
    EHWRMApiTypeNoAPI = 0,
    EHWRMApiTypeXAPI, 	// Extended API
    EHWRMApiTypeSAPI,	// SDK API
    EHWRMApiTypeEAPI	// Enhanced API
    };
    
// Test case ids
enum THWRMLightTestId
	{
	/*BASIC	CASES 
	*********************************/
	EBasicTestIdFirst = 1, 				//< DO NOT MOVE
	/*New basic test cases here*/
	EAllSupportedTargetsLightTest,
	EAllTargetsLightTest,
	ESingleTargetLightTest1,
	
	ECombinedTargetsLightTest1,
	ECombinedTargetsLightTest2,
		
	EAllTargetsLightOnDurationTest,
	ESingleTargetLightOnDurationTest1,
	
	EAllTargetsLightOffDurationTest,
	ESingleTargetLightOffDurationTest1,
	
	EAllTargetsLightIntensityTest,
	ESingleTargetLightIntensityTest1,
	
	EAllTargetsLightBlinkTest,
	ESingleTargetLightBlinkTest1,
	
	EAllTargetsLightBlinkDurationTest,
	ESingleTargetLightBlinkDurationTest1,
	
	EAllTargetsLightBlinkOnOffTest,
	ESingleTargetLightBlinkOnOffTest1,

	EAllTargetsLightOnDurationRGBTest,
	EAllTargetsLightOnDurationRGBTest1,
	EAllTargetsLightBlinkDurationRGBTest,
	EAllTargetsLightBlinkDurationRGBTest1,
	
	//test cases with hard coded targets
	ELightDifferentTargetsTest,	
	ELightInvalidDurationTest,	
	ELightIntensityTest2,
	
	/*end new basic test cases*/	
	EBasicTestIdLast, 					//< ENDMARK, DO NOT MOVE

	
	/* MULTICLIENT CASES
	*********************************/
	EMulticlientTestIdFirst,	 		//< DO NOT MOVE
	/*New multiclient test cases here*/
	EMCPrimaryDisplayLight,
	EMCDisplayOnKbBlink,
	EMCDisplayOnKbReserved,
	EMCDisplayAndKbBlink,
	EMCDisplayRGBColor,
	EMCVibraBasicUsage,
	EMCCombinedLightAndVibra1,
	EMCCombinedLightAndVibra2,	
	EMCFmTxBasicUsage,
	/*end new multiclient test cases*/
	EMulticlientTestIdLast,				//< ENDMARK, DO NOT MOVE

	
	/*SPECIAL CASES
	*******************************/
	ESpecialTestIdFirst,				//< DO NOT MOVE
	/*New special test cases here*/
	ERepeatInvalidParamsTest,
	EMaliciousVibraTest,
	EWaitForChargingNotify,
	/*end new special test case*/
	ESpecialIdLast 						//< ENDMARK, DO NOT MOVE
	};

#endif
