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
//

#ifndef __TSI_MMF_A2DP_DS_H__
#define __TSI_MMF_A2DP_DS_H__

#include "TSI_MMF_A2DP_STEP.h"
#include "TSI_MMF_A2DP_SUITE.h"


class RTestStepA2dp;

//DevSound: Initialize headset test
class RTestStepA2dpInitializeHeadset : public RTestStepA2dp
	{
public:
	RTestStepA2dpInitializeHeadset(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Reinitial disconnected headset test
class RTestStepA2dpReinitialDisconnectHeadset : public RTestStepA2dp
	{
public:
	RTestStepA2dpReinitialDisconnectHeadset(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Reinitialize headset test
class RTestStepA2dpReinitializeHeadset : public RTestStepA2dp
	{
public:
	RTestStepA2dpReinitializeHeadset(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Capabilities test (Sample rate and Channels)
class RTestStepA2dpCapabilities : public RTestStepA2dp
	{
public:
	RTestStepA2dpCapabilities(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: PlayInit test
class RTestStepA2dpPlayInit : public RTestStepA2dp
	{
public:
	RTestStepA2dpPlayInit(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Play EOF PCM16 test
class RTestStepA2dpPlayEofPcm16 : public RTestStepA2dp
	{
public:
	RTestStepA2dpPlayEofPcm16(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Play EOF PCM8 test
class RTestStepA2dpPlayEofPcm8 : public RTestStepA2dp
	{
public:
	RTestStepA2dpPlayEofPcm8(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};	

//DevSound: Disconnect when play
class RTestStepA2dpDisconnectPlay : public RTestStepA2dp
	{
public:
	RTestStepA2dpDisconnectPlay(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Play simple tone
class RTestStepA2dpPlaySimpleTone : public RTestStepA2dp
	{
public:
	RTestStepA2dpPlaySimpleTone(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Two Thread tests
class RTestStepA2dpTwoThread : public RTestStepA2dp
	{
public:
	RTestStepA2dpTwoThread(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//DevSound: Negative test: Initialize DevSound when BT headset is not available
class RTestStepA2dpInitializeHeadsetNeg : public RTestStepA2dp
	{
public:
	RTestStepA2dpInitializeHeadsetNeg(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

#endif// __TSI_MMF_A2DP_DS_H__



