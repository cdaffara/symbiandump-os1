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

/**
 @file
 @internalComponent 
*/

#if (!defined __TE_MISC_H__)
#define __TE_MISC_H__

#include "Te_MiscTestStepBase.h"

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <mmlist.h>

class CServerInfo : public CMiscTestStep
	{
public:
	CServerInfo();
	virtual enum TVerdict doTestStepL( void );
	};

class CSignalAndBatteryStrengthTest : public CMiscTestStep
	{
public:
	CSignalAndBatteryStrengthTest();
	virtual enum TVerdict doTestStepL( void );
	};

class CManufacturerInfoTest : public CMiscTestStep
	{
public:
	CManufacturerInfoTest();
	virtual enum TVerdict doTestStepL( void );
	};

class CSubscriberIdTest : public CMiscTestStep
	{
public:
	CSubscriberIdTest();
	virtual enum TVerdict doTestStepL( void );
	};

class CPhoneStoreTest : public CMiscTestStep
	{
public:
	CPhoneStoreTest();
	virtual enum TVerdict doTestStepL( void );
	};

class COwnNumberTest : public CMiscTestStep
	{
public:
	COwnNumberTest();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetCaps : public CMiscTestStep
	{
public:
	CGetCaps();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetLinesStatus : public CMiscTestStep
	{
public:
	CGetLinesStatus();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetMultimodeCaps : public CMiscTestStep
	{
public:
	CGetMultimodeCaps();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetNetworkCaps : public CMiscTestStep
	{
public:
	CGetNetworkCaps();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetCurrentMode : public CMiscTestStep
	{
public:
	CGetCurrentMode();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetAsyncNetList : public CMiscTestStep
	{
public:
	CGetAsyncNetList();
	virtual enum TVerdict doTestStepL( void );

private:
	CActiveScheduler* iTestScheduler;

	class CTestAsyncGetNetList : public CActive
		{
	public:
		static CTestAsyncGetNetList* NewLC(RMobilePhone& aPhone);
		~CTestAsyncGetNetList();
		void Start();
		CMobilePhoneNetworkList* RetrieveList();
		TInt CompleteCode();

	private:
		CTestAsyncGetNetList(RMobilePhone& aPhone);
		void ConstructL();
		void RunL();
		void DoCancel();

	private:		
		CRetrieveMobilePhoneDetectedNetworks* iRetrieve;
		RMobilePhone& iPhone;

	private:
		TInt iCompleteCode;	
		};
	};

class CIsSupportedByModuleTest : public CMiscTestStep
	{
public:
	CIsSupportedByModuleTest();
	TBool doIsSupportedByModuleTest(TInt aMixin);
	virtual enum TVerdict doTestStepL( void );
	};

#endif // __TE_MISC_STEP_H__
