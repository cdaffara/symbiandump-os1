// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_SWCDWRAP_STEPSTARTCONVERT_H__
#define __TSU_MMF_SWCDWRAP_STEPSTARTCONVERT_H__

#include "TSU_MMF_SWCDWRAP_Step.h"

class CTestSuite;
class CTestSuite_MMF_SWCDWRAP;
class CTestConvertMMFHwDeviceObserver;

/**
 *
 * CTestStep_MMF_SWCDWRAP_StartTestConvert, which is the base class for all 
 * the MMF BASECL suite test steps that test the Start()
 * method when used for convertion.
 *
 * @lib "TSU_MMF_SWCDWRAP.lib"
 *
 * @xxxx
 *
 */
class CTestStep_MMF_SWCDWRAP_StartTestConvert : public CTestStep_MMF_SWCDWRAP
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	TVerdict InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid);
protected:
	CActiveScheduler* iActiveScheduler;
	CMMFHwDevice* iHwDevice;
	CTestConvertMMFHwDeviceObserver* iHwDeviceObserver;
	};


/**
 *
 * CTestStep_MMF_SWCDWRAP_StartTest2To1Convert, which is the base class for all 
 * the MMF SWCDWRAP suite test steps that test the Start()
 * method when used for convertion when using the 2 to 1 buffer ratio codec.
 *
 * @lib "TSU_MMF_SWCDWRAP.lib"
 *
 */
class CTestStep_MMF_SWCDWRAP_StartTest2To1Convert : public CTestStep_MMF_SWCDWRAP_StartTestConvert
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	};


class CTestStep_MMF_SWCDWRAP_U_0200 : public CTestStep_MMF_SWCDWRAP_StartTestConvert
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0200() ;
	~CTestStep_MMF_SWCDWRAP_U_0200(){} ;
	virtual TVerdict DoTestStepL();
private:
	TBool CompareL( RFile& aFile1, RFile& aFile2, TUint aCompareLength );
	} ;


class CTestStep_MMF_SWCDWRAP_U_0240 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Convert
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0240() ;
	~CTestStep_MMF_SWCDWRAP_U_0240(){} ;
	virtual TVerdict DoTestStepL();
	} ;

#endif /* __TSU_MMF_SWCDWRAP_STEPSTARTCONVERT_H__ */
