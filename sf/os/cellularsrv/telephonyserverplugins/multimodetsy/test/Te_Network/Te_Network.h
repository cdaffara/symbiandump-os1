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

#if (!defined __TS_NETWORK_H__)
#define __TS_NETWORK_H__

#include "Te_NetworkTestStepBase.h"

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <mmlist.h>

//for retrive network information
struct KnownNetwork
	{
	TUint iMCC;
	TUint iMNC;
	const TText* iShortName;
	const TText* iLongName;
	};

class CServerInfo : public CNetworkTestStep
	{
public:
	CServerInfo();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetNetworkCaps : public CNetworkTestStep
	{
public:
	CGetNetworkCaps();
	virtual enum TVerdict doTestStepL( void );	
	};

class CDisconnectedNetworkTest : public CNetworkTestStep
	{
public:
	CDisconnectedNetworkTest();
	virtual enum TVerdict doTestStepL( void );	
	};

class CGetNetworkRegistrationStatus : public CNetworkTestStep
	{
public:
	CGetNetworkRegistrationStatus();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetNetworkRegistrationStatusCancel : public CNetworkTestStep
	{
public:
	CGetNetworkRegistrationStatusCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CNotifyNetworkRegistrationStatusChange : public CNetworkTestStep
	{
public:
	CNotifyNetworkRegistrationStatusChange();
	virtual enum TVerdict doTestStepL( void );
	};

class CNotifyNetworkRegistrationStatusChangeCancel : public CNetworkTestStep
	{
public:
	CNotifyNetworkRegistrationStatusChangeCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetCurrentMode : public CNetworkTestStep
	{
public:
	CGetCurrentMode();
	virtual enum TVerdict doTestStepL( void );	
	};

class CNotifyModeChange : public CNetworkTestStep
	{
public:
	CNotifyModeChange();
	virtual enum TVerdict doTestStepL( void );
	};

class CNotifyModeChangeCancel : public CNetworkTestStep
	{
public:
	CNotifyModeChangeCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetCurrentNetwork : public CNetworkTestStep
	{
public:
	CGetCurrentNetwork();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetCurrentNetworkCancel : public CNetworkTestStep
	{
public:
	CGetCurrentNetworkCancel();
	virtual enum TVerdict doTestStepL( void );	
	};

class CNotifyCurrentNetworkChange : public CNetworkTestStep
	{
public:
	CNotifyCurrentNetworkChange();
	virtual enum TVerdict doTestStepL( void );
	};

class CNotifyCurrentNetworkChangeCancel : public CNetworkTestStep
	{
public:
	CNotifyCurrentNetworkChangeCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetHomeNetwork : public CNetworkTestStep
	{
public:
	CGetHomeNetwork();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetHomeNetworkCancel : public CNetworkTestStep
	{
public:
	CGetHomeNetworkCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CDetectedNetworkTest : public CNetworkTestStep
{
public:
	CDetectedNetworkTest();
	virtual enum TVerdict doTestStepL( void );

private:
	CActiveScheduler* iTestScheduler;

	class CGetDetectedNetworks : CActive
		{
	public:
		static CGetDetectedNetworks* NewLC(RMobilePhone& aPhone);
		~CGetDetectedNetworks();
		void Start();
		CMobilePhoneNetworkList* RetrieveList();
		TInt CompleteCode();
	private:
		CGetDetectedNetworks(RMobilePhone& aPhone);
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

class CGetNetworkSelectionSetting : public CNetworkTestStep
	{
public:
	CGetNetworkSelectionSetting();
	virtual enum TVerdict doTestStepL( void );
	};

class CSetNetworkSelectionSetting : public CNetworkTestStep
	{
public:
	CSetNetworkSelectionSetting();
	virtual enum TVerdict doTestStepL( void );
	};

class CSetNetworkSelectionSettingCancel : public CNetworkTestStep
	{
public:
	CSetNetworkSelectionSettingCancel();
	virtual enum TVerdict doTestStepL( void );	
	};

class CNotifyNetworkSelectionSettingChange : public CNetworkTestStep
	{
public:
	CNotifyNetworkSelectionSettingChange();
	virtual enum TVerdict doTestStepL( void );	
	};

class CNotifyNetworkSelectionSettingChangeCancel : public CNetworkTestStep
	{
public:
	CNotifyNetworkSelectionSettingChangeCancel();
	virtual enum TVerdict doTestStepL( void );
	};

class CGetNITZInfo : public CNetworkTestStep
	{
public:
	CGetNITZInfo();
	virtual enum TVerdict doTestStepL( void );
	};

class CNotifyNITZInfoChange : public CNetworkTestStep
	{
public:
	CNotifyNITZInfoChange();
	virtual enum TVerdict doTestStepL( void );	
	};

class CNotifyNITZInfoChangeCancel : public CNetworkTestStep
	{
public:
	CNotifyNITZInfoChangeCancel();
	virtual enum TVerdict doTestStepL( void );	
	};

#endif // __TS_NETWORK_STEP_H__
