
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

#ifndef GLOBALEFFECTTEST_H
#define GLOBALEFFECTTEST_H

#include <testframework.h>

#include <mmf/common/mmfglblaudioeffect.h>

class CTestGlobalEffect; // forward

class RGlobalEffectPrimitiveTest : public RTestStep
	{
public:
	static RGlobalEffectPrimitiveTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RGlobalEffectPrimitiveTest(TBool aAllocTest);
	
	};

class RGlobalEffectStdTest : public RTestStep
	{
public:
	static RGlobalEffectStdTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RGlobalEffectStdTest(TBool aAllocTest);
	
	};

class RGlobalEffectMainTest : public RTestStep
	{
public:
	static RGlobalEffectMainTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RGlobalEffectMainTest(TBool aAllocTest);
	
	};

class RGlobalEffectCallbackTest : public RTestStep,
                                  public MMmfGlobalAudioEffectObserver
	{
public:
	static RGlobalEffectCallbackTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// from MMmfGlobalAudioEffectObserver
	void GAEEventNotificationL(CMmfGlobalAudioEffect* aEffect, TUid aEventUid, const TDesC8& aParam);
	
private:
	RGlobalEffectCallbackTest(TBool aAllocTest);
	
	CTestGlobalEffect* iEffect;	// not owned
	TBool iCalledBack;
	TVerdict iResult;
	};

#endif // GLOBALEFFECTTEST_H
