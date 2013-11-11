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

#ifndef __TSI_MMFCTLFRMNEG_H__
#define __TSI_MMFCTLFRMNEG_H__

#include <testframework.h>
#include <mmf/common/mmfcontroller.h>
#include "TSI_MmfCtlfrmStep.h"

class CTestSuite;
class CTestSuiteMmfCtlfrm;

class RTestMmfCtlfrmI0502 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0502* NewL();
	RTestMmfCtlfrmI0502();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0504 : public RTestStepMmfCtlfrm
	{
public:
	static RTestMmfCtlfrmI0504* NewL();
	RTestMmfCtlfrmI0504();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0511 : public RTestStepMmfCtlfrmTest2	// NB
	{
public:
	static RTestMmfCtlfrmI0511* NewL();
	RTestMmfCtlfrmI0511();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0512 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0512* NewL();
	RTestMmfCtlfrmI0512();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0513 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0513* NewL();
	RTestMmfCtlfrmI0513();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0514 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0514* NewL();
	RTestMmfCtlfrmI0514();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0515 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0515* NewL();
	TVerdict OpenL();
	void Close();
	RTestMmfCtlfrmI0515();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0521 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0521* NewL();
	RTestMmfCtlfrmI0521();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0522 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0522* NewL();
	RTestMmfCtlfrmI0522();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0523 : public RTestStepMmfCtlfrmAudioNoSource
	{
public:
	static RTestMmfCtlfrmI0523* NewL();
	RTestMmfCtlfrmI0523();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0524 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0524* NewL();
	RTestMmfCtlfrmI0524();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0525 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0525* NewL();
	RTestMmfCtlfrmI0525();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0526 : public RTestStepMmfCtlfrmAudioNoSource
	{
public:
	static RTestMmfCtlfrmI0526* NewL();
	RTestMmfCtlfrmI0526();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0527 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0527* NewL();
	RTestMmfCtlfrmI0527();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0531 : public RTestStepMmfCtlfrmAudio
	{
public:
	static RTestMmfCtlfrmI0531* NewL();
	RTestMmfCtlfrmI0531();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0532 : public RTestStepMmfCtlfrmDualAudio // NB
	{
public:
	static RTestMmfCtlfrmI0532* NewL();
	RTestMmfCtlfrmI0532();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0541 : public RTestStepMmfCtlfrmAudioNoSource
	{
public:
	static RTestMmfCtlfrmI0541* NewL();
	RTestMmfCtlfrmI0541();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0542 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0542* NewL();
	RTestMmfCtlfrmI0542();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0551 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0551* NewL();
	RTestMmfCtlfrmI0551();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0552 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0552* NewL();
	RTestMmfCtlfrmI0552();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0553 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0553* NewL();
	RTestMmfCtlfrmI0553();
	virtual TVerdict DoTestStepL();
	};

class RTestMmfCtlfrmI0554 : public RTestStepMmfCtlfrmAudioPrimed
	{
public:
	static RTestMmfCtlfrmI0554* NewL();
	RTestMmfCtlfrmI0554();
	virtual TVerdict DoTestStepL();
	};

#endif // __TSI_MMFCTLFRMNEG_H__
