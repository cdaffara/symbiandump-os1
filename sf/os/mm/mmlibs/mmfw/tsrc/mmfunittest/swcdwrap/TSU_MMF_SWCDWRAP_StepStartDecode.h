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
// TSU_MMF_SWCDWRAP_Step.h
// 
//

#ifndef __TSU_MMF_SWCDWRAP_STEPSTARTDECODE_H__
#define __TSU_MMF_SWCDWRAP_STEPSTARTDECODE_H__

#include "TSU_MMF_SWCDWRAP_Step.h"

class CTestSuite;
class CTestSuite_MMF_SWCDWRAP;
class CTestPlayMMFHwDeviceObserver;

/**
 *
 * CTestStep_MMF_SWCDWRAP, which is the base class for all 
 * the MMF SWCDWRAP suite test steps that test the Start()
 * method when used for decode (play).
 *
 * @lib "TSU_MMF_SWCDWRAP.lib"
 *
 */
class CTestStep_MMF_SWCDWRAP_StartTestDecode : public CTestStep_MMF_SWCDWRAP
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	TVerdict InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid);
protected:
	CActiveScheduler* iActiveScheduler;
	CMMFHwDevice* iHwDevice;
	CTestPlayMMFHwDeviceObserver* iHwDeviceObserver;
	MPlayCustomInterface* iPlayCustomInterface;
	};

class CTestStep_MMF_SWCDWRAP_StartTest2To1Decode : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	};


class CTestStep_MMF_SWCDWRAP_U_0010 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0010() ;
	~CTestStep_MMF_SWCDWRAP_U_0010(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0011 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0011() ;
	~CTestStep_MMF_SWCDWRAP_U_0011(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0012 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0012() ;
	~CTestStep_MMF_SWCDWRAP_U_0012(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0013 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0013() ;
	~CTestStep_MMF_SWCDWRAP_U_0013(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0014 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0014() ;
	~CTestStep_MMF_SWCDWRAP_U_0014(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0015 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0015() ;
	~CTestStep_MMF_SWCDWRAP_U_0015(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0016 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0016() ;
	~CTestStep_MMF_SWCDWRAP_U_0016(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0017 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0017() ;
	~CTestStep_MMF_SWCDWRAP_U_0017(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0018 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0018() ;
	~CTestStep_MMF_SWCDWRAP_U_0018(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0019 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0019() ;
	~CTestStep_MMF_SWCDWRAP_U_0019(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0020 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0020() ;
	~CTestStep_MMF_SWCDWRAP_U_0020(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0021 : public CTestStep_MMF_SWCDWRAP_StartTestDecode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0021() ;
	~CTestStep_MMF_SWCDWRAP_U_0021(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0040 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0040() ;
	~CTestStep_MMF_SWCDWRAP_U_0040(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0041 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0041() ;
	~CTestStep_MMF_SWCDWRAP_U_0041(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0042 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0042() ;
	~CTestStep_MMF_SWCDWRAP_U_0042(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0043 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0043() ;
	~CTestStep_MMF_SWCDWRAP_U_0043(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0044 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0044() ;
	~CTestStep_MMF_SWCDWRAP_U_0044(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0045 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0045() ;
	~CTestStep_MMF_SWCDWRAP_U_0045(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0046 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0046() ;
	~CTestStep_MMF_SWCDWRAP_U_0046(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0047 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0047() ;
	~CTestStep_MMF_SWCDWRAP_U_0047(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0048 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0048() ;
	~CTestStep_MMF_SWCDWRAP_U_0048(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0049 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0049() ;
	~CTestStep_MMF_SWCDWRAP_U_0049(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0050 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Decode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0050() ;
	~CTestStep_MMF_SWCDWRAP_U_0050(){} ;
	virtual TVerdict DoTestStepL();
	} ;


#endif /* __TSU_MMF_SWCDWRAP_STEPSTARTDECODE_H__ */
