// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Telephony Watchers Unit Test header file.
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERSUNITTEST_H__
#define __TE_TELWATCHERSUNITTEST_H__


class CTelWatchersUnitTestCallStateChangesWithPublishSubscribe : public CTelWatchersUnitTestStep
	{
public:
	CTelWatchersUnitTestCallStateChangesWithPublishSubscribe();
	~CTelWatchersUnitTestCallStateChangesWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};


class CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe : public CTelWatchersUnitTestStep
	{
public:
	CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe();
	~CTelWatchersUnitTestBatteryChargerChangesWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};


class CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe : public CTelWatchersUnitTestStep
	{
public:
	CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe();
	~CTelWatchersUnitTestNetworkAvailabilityChangesWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};


class CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe : public CTelWatchersUnitTestStep
	{
public:
	CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe();
	~CTelWatchersUnitTestSimulteCommDBModemChangeWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};
	

class CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe : public CTelWatchersUnitTestStep
	{
public:
	CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe();
	~CTelWatchersUnitTestCommDBChangeWhenWatcherDisabledWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};

#endif // __TE_TELWATCHERSUNITTEST_H__

