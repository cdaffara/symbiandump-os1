/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file 
 @internalTechnology
*/

#ifndef TE_ACTIVITYMANAGER_H
#define TE_ACTIVITYMANAGER_H

//	EPOC includes
#include <test/datawrapper.h>
#include <activitymanager.h>

class CTeActivityManager: public CDataWrapper
	{
public:
	static CTeActivityManager*	NewL();

	~CTeActivityManager();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
		
protected:
	CTeActivityManager();

private:
	void	DoCmdDestructor();
	void	DoCmdNew(const TTEFSectionName& aSection);
	void    DoCmdStart(const TTEFSectionName& aSection);
	void    DoCmdSetInactivityTimeout(const TTEFSectionName& aSection);
	void    DoCmdUserActivity();
	void 	DoCmdSetExpectedResultInactivity(const TTEFSectionName& aSection);
	void 	DoCmdSetExpectedResultActivity(const TTEFSectionName& aSection);
	void    DoCmdCheckExpectedEvents(const TTEFSectionName& aSection);
	void 	DoCmdSetCallbackOff();
	void	DoCmdSetSlowCallbacks();
	void 	DoCmdCancel();
	void    DoCmdDummyMethod();
	void	UDEB_Only();
	void	UREL_Only();
	void	ExpectedPanic(const TTEFSectionName& aSection);
	void    DoCmdActivityInjectTest(const TTEFSectionName& aSection);
	
	// Handlers
	static void SlowCallbacks(TBool aSlowCallbacks);
	static TInt HandleInactiveEvent(TAny* aPtr);	
	static TInt HandleActiveEvent(TAny* aPtr);
	static TInt ErrorTestingHandleInactiveEvent(TAny* aPtr);
	static TInt ErrorTestingHandleActiveEvent(TAny* aPtr);

	static TInt ActivityInjectTestL(TAny* aAny);

	// Activity type enum	
	enum TActivityType
		{
		EEventInactivity,
		EEventActivity
		};
		
	//	Helper methods
	void  DestroyData();
	TBool MapAndReturnCActivePriority(CActive::TPriority& aPriority, TDesC& aReadPriority);
	void ProcessTestResult(TActivityType event,TTime aCurrentTime);
	void OomTest();
	void OomTestImpL();
private:
	// Start time
	TTime 					  iStartTime;                     
	// Expected elapsed interval time for Inactivity
	TTimeIntervalMicroSeconds iExpectedIntervalMicroSecondsInactivity;
	// Expected elapsed interval time for Activity
	TTimeIntervalMicroSeconds iExpectedIntervalMicroSecondsActivity;
	// Class object under test
	CUserActivityManager*	  iUserActivityManager;
	// Number of inactive events
	TInt				      iInactiveEvents;
	// Number of active events
	TInt				      iActiveEvents;
	// Number of expected events
	TActivityType			  iExpectedEvent;
	// Used to indicate if callback methods are active
	TBool			  		  iCallbackMethods;
	// Used to make callbacks take a long time
	TBool					  iSlowCallbacks;
	// If false, don't perform commands
	TBool					  iDoCommands;
	// Interval for activity inject command
	TInt 					  iInterval;
	};
	
#endif // TE_ACTIVITYMANAGER_H
