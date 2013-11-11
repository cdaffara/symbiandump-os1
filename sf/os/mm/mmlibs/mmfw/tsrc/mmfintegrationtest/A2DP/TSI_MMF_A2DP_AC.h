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

#ifndef __TSI_MMF_A2DP_AC_H__
#define __TSI_MMF_A2DP_AC_H__

#include "TSI_MMF_A2DP_STEP.h"
#include "TSI_MMF_A2DP_SUITE.h"


class RTestStepA2dp;


//AclntPlayer: New player
class RTestStepA2dpAclPlyNew : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclPlyNew(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntPlayer: Open Close Open
class RTestStepA2dpAclPlyOpenCloseOpen : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclPlyOpenCloseOpen(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntPlayer: Disconnect the link, and then reinitialize
class RTestStepA2dpAclPlyReInitDisconn : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclPlyReInitDisconn(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntPlayer: Re-Initialize the audio player
class RTestStepA2dpAclPlyReInit : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclPlyReInit(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntRecorder: New recorder
class RTestStepA2dpAclRecNew : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclRecNew(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntRecorder:  Open Close Open
class RTestStepA2dpAclRecOpenCloseOpen : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclRecOpenCloseOpen(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntTone:  New Tone Utility
class RTestStepA2dpAclToneNew : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclToneNew(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntOutputStream:  New OutputStream
class RTestStepA2dpAclStrmNew : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclStrmNew(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntOutputStream:  OutputStream write
class RTestStepA2dpAclStrmWrite : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclStrmWrite(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntOutputStream:  OutputStream write, and then stop
class RTestStepA2dpAclStrmWriteStop : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclStrmWriteStop(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};

//AclntOutputStream Negative test:  New OutputStream when the BT headset is not available
class RTestStepA2dpAclStrmNewNeg : public RTestStepA2dp
	{
public:
	RTestStepA2dpAclStrmNewNeg(const TDesC& aTestName);
	TVerdict DoTestStepL();
	};



#endif// __TSI_MMF_A2DP_AC_H__



