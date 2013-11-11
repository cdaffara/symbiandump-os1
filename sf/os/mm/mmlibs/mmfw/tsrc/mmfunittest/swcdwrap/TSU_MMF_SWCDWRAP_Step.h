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

#ifndef __TSU_MMF_SWCDWRAP_STEP_H__
#define __TSU_MMF_SWCDWRAP_STEP_H__

#include <testframework.h>
#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>

#ifndef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	_LIT(KPddFileName,"ESDRV.PDD");
	_LIT(KLddFileName,"ESOUND.LDD");
#else
	_LIT(KPddFileName,"SOUNDSC.PDD");
	_LIT(KLddFileName,"ESOUNDSC.LDD");
#endif

class CTestSuite;
class CTestSuite_MMF_SWCDWRAP;

/**
 *
 * CTestStep_MMF_SWCDWRAP, which is the base class for all 
 * the MMF SWCDWRAP suite test steps.
 *
 * @lib "TSU_MMF_SWCDWRAP.lib"
 *
 */
class CTestStep_MMF_SWCDWRAP : public CTestStep
	{
public:
	static void CleanUp(TAny *aPtr) ;
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	};


class CTestStep_MMF_SWCDWRAP_U_0001 : public CTestStep_MMF_SWCDWRAP
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0001() ;
	~CTestStep_MMF_SWCDWRAP_U_0001(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0002 : public CTestStep_MMF_SWCDWRAP
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0002() ;
	~CTestStep_MMF_SWCDWRAP_U_0002(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0003 : public CTestStep_MMF_SWCDWRAP
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0003() ;
	~CTestStep_MMF_SWCDWRAP_U_0003(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_SWCDWRAP_U_0004 : public CTestStep_MMF_SWCDWRAP
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0004() ;
	~CTestStep_MMF_SWCDWRAP_U_0004(){} ;
	virtual TVerdict DoTestStepL();
	} ;


class CTestStep_MMF_SWCDWRAP_U_0300 : public CTestStep_MMF_SWCDWRAP
	{
public:
	CTestStep_MMF_SWCDWRAP_U_0300() ;
	~CTestStep_MMF_SWCDWRAP_U_0300(){} ;
	virtual TVerdict DoTestStepL();
	} ;


#endif /* __TSU_MMF_SWCDWRAP_H__ */
