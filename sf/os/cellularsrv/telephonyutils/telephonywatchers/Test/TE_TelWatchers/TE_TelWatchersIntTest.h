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
// Telephony Watchers Integration Test header file.
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERINTTEST_H__
#define __TE_TELWATCHERINTTEST_H__


class CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe();
	~CTelWatchersIntTestRetrieveSignalStrengthWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};
	

class CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe();
	~CTelWatchersIntTestRetrieveChargerStatusWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};


class CTelWatchersIntTestNetworkStatusWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestNetworkStatusWithPublishSubscribe();
	~CTelWatchersIntTestNetworkStatusWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};

	
class CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe();
	~CTelWatchersIntTestIncomingCallStatusWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();

	void IncomingCallL();
	static TInt IncomingCallThreadFunction(TAny* aPtr);
	};


class CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe();
	~CTelWatchersIntTestOutgoingCallStatusWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();

	void OutgoingCallL();
	static TInt OutgoingCallThreadFunction(TAny* aPtr);
	};
	

class CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe();
	~CTelWatchersIntTestIndicatorKErrNotSupportedScenarioWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};


class CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe : public CTelWatchersIntTestStep
	{
public:
	CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe();
	~CTelWatchersIntTestIndicatorKErrEtelModemNotDetectedScenarioWithPublishSubscribe();

	virtual enum TVerdict doTestStepL();
	};

#endif // __TE_TELWATCHERINTTEST_H__

