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

#ifndef __TSU_MMF_SWCDWRAP_STEPSTARTENCODE_H__
#define __TSU_MMF_SWCDWRAP_STEPSTARTENCODE_H__

#include "TSU_MMF_SWCDWRAP_Step.h"

class CTestSuite;
class CTestSuite_MMF_SWCDWRAP;
class CTestRecordMMFHwDeviceObserver;

/**
 *
 * CTestStep_MMF_SWCDWRAP, which is the base class for all 
 * the MMF SWCDWRAP suite test steps that test the Start()
 * method when used for decode (play).
 *
 * @xxxx
 *
 */
class CTestStep_MMF_SWCDWRAP_StartTestEncode : public CTestStep_MMF_SWCDWRAP
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	TVerdict InitializeTestStepUsingHwDeviceL(TUid aHwDeviceUid);
protected:
	CActiveScheduler* iActiveScheduler;
	CMMFHwDevice* iHwDevice;
	CTestRecordMMFHwDeviceObserver* iHwDeviceObserver;
	MRecordCustomInterface* iRecordCustomInterface;
	};

class CTestStep_MMF_SWCDWRAP_StartTest2To1Encode : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	};


class CTestStep_MMF_SWCDWRAP_U_0100 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0100() ;
	~CTestStep_MMF_SWCDWRAP_U_0100(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0101 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0101() ;
	~CTestStep_MMF_SWCDWRAP_U_0101(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0102 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0102() ;
	~CTestStep_MMF_SWCDWRAP_U_0102(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0103 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0103() ;
	~CTestStep_MMF_SWCDWRAP_U_0103(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0104 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0104() ;
	~CTestStep_MMF_SWCDWRAP_U_0104(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0105 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0105() ;
	~CTestStep_MMF_SWCDWRAP_U_0105(){} ;
	virtual TVerdict DoTestStepL();
	} ;
	
class CTestStep_MMF_SWCDWRAP_U_0106 : public CTestStep_MMF_SWCDWRAP_StartTestEncode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0106() ;
	~CTestStep_MMF_SWCDWRAP_U_0106(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0140 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Encode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0140() ;
	~CTestStep_MMF_SWCDWRAP_U_0140(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0141 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Encode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0141() ;
	~CTestStep_MMF_SWCDWRAP_U_0141(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0142 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Encode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0142() ;
	~CTestStep_MMF_SWCDWRAP_U_0142(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0143 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Encode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0143() ;
	~CTestStep_MMF_SWCDWRAP_U_0143(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0144 : public CTestStep_MMF_SWCDWRAP_StartTest2To1Encode
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0144() ;
	~CTestStep_MMF_SWCDWRAP_U_0144(){} ;
	virtual TVerdict DoTestStepL();
	} ;

#endif /* __TSU_MMF_SWCDWRAP_STEPSTARTENCODE_H__ */
