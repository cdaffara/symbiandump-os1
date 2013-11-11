// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSI_MMFCTLFRM_H__
#define __TSI_MMFCTLFRM_H__

#include <testframework.h>
#include <mmf/common/mmfcontroller.h>
#include "TSI_MmfCtlfrmStep.h"

class CTestSuite;
class CTestSuiteMmfCtlfrm;

class RTestMmfCtlfrmI0001 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0001* NewL();
	RTestMmfCtlfrmI0001();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0002 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0002* NewL();
	RTestMmfCtlfrmI0002();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0003 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0003* NewL();
	RTestMmfCtlfrmI0003();
	virtual TVerdict DoTestStepL();
	};

// 

// Test 0101 has been removed.
// As plugins are loaded on the Z drive it isn't practical to
// test this automatically - we would require a new controller
// to be loaded on C drive and for MMF to pick that up

class RTestMmfCtlfrmI0102 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0102* NewL();
	RTestMmfCtlfrmI0102();
	virtual TVerdict DoTestStepL();
	};

// Tests 0103, 0104 removed
// API changed, functionality now mirrored in 0105, 0106

class RTestMmfCtlfrmI0105 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0105* NewL();
	RTestMmfCtlfrmI0105();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0106 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0106* NewL();
	RTestMmfCtlfrmI0106();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0107 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0107* NewL();
	RTestMmfCtlfrmI0107();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0108 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0108* NewL();
	RTestMmfCtlfrmI0108();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0109 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0109* NewL();
	RTestMmfCtlfrmI0109();
	virtual TVerdict DoTestStepL();
	};

//

class RTestMmfCtlfrmI0111 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0111* NewL();
	RTestMmfCtlfrmI0111();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0112 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0112* NewL();
	RTestMmfCtlfrmI0112();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0113 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0113* NewL();
	RTestMmfCtlfrmI0113();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0114 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0114* NewL();
	RTestMmfCtlfrmI0114();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0117 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0117* NewL();
	RTestMmfCtlfrmI0117();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0118 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0118* NewL();
	RTestMmfCtlfrmI0118();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0119 : public RTestStepMmfCtlfrmVideo
	{
public:
	static RTestMmfCtlfrmI0119* NewL();
	RTestMmfCtlfrmI0119();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0120 : public RTestStepMmfCtlfrmVideo
	{
public:
	static RTestMmfCtlfrmI0120* NewL();
	RTestMmfCtlfrmI0120();
	virtual TVerdict DoTestStepL();
	};

//

class RTestMmfCtlfrmI0121 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0121* NewL();
	RTestMmfCtlfrmI0121();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0122 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0122* NewL();
	RTestMmfCtlfrmI0122();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0123 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0123* NewL();
	RTestMmfCtlfrmI0123();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0124 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0124* NewL();
	RTestMmfCtlfrmI0124();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0125 : public RTestStepMmfCtlfrmDualAudio // NB
	{
public:
	static RTestMmfCtlfrmI0125* NewL();
	RTestMmfCtlfrmI0125();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0128 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0128* NewL();
	RTestMmfCtlfrmI0128();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0129 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0129* NewL();
	RTestMmfCtlfrmI0129();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0131 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0131* NewL();
	RTestMmfCtlfrmI0131();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0132 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0132* NewL();
	RTestMmfCtlfrmI0132();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0141 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0141* NewL();
	RTestMmfCtlfrmI0141();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0142 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0142* NewL();
	RTestMmfCtlfrmI0142();
	TVerdict SearchForEventWithCancel( TMMFEvent& aExpectedEvent );
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0143 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0143* NewL();
	RTestMmfCtlfrmI0143();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0144 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0144* NewL();
	RTestMmfCtlfrmI0144();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0151 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0151* NewL();
	RTestMmfCtlfrmI0151();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0152 : public RTestStepMmfCtlfrmTest1
	{
public:
	static RTestMmfCtlfrmI0152* NewL();
	RTestMmfCtlfrmI0152();
	virtual TVerdict DoTestStepL();
	};

#endif // __TSI_MMFCTLFRM_H__
