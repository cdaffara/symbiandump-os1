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
// Header for TE_RConnectionSuite class. This is the container
// class for all the RConnection multihoming test steps
// 
//

/**
 @file
*/

#if (!defined __TE_RConnectionSUITE_H__)
#define __TE_RConnectionSUITE_H__

#include <e32std.h>
#include <c32comm.h>
#include <test/testexecuteserverbase.h>
#include <es_sock.h>
#include <in_sock.h>



#define MAX_NAME_LENGTH		10			//< Maximum length of connection name

_LIT (KTxtVersion,"1.00");				//< Version string for ScheduleTest
_LIT (KConnLogString, "Name [%S], RConnection[%x], RSocketServ[%x], Clients: %i");


class TE_RConnectionStep;

/*class TE_RConnectionSuite : public CTestSuite
{
public:
	virtual		~TE_RConnectionSuite();
	void		InitialiseL(void);
	void		AddTestStepL(TE_RConnectionStep* ptrTestStep );
	TPtrC		GetVersion(void);	
	
private:
	CActiveScheduler* iScheduler;
};
*/
void CommInitL(TBool);


class TE_RConnectionSuiteTestServer: public CTestServer
	{
	public:
	static TE_RConnectionSuiteTestServer* NewL();
	~TE_RConnectionSuiteTestServer();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	private:
	const TPtrC ServerName() const;
	
	CTestStep* CreateRConnTestStep1(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep2(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep3(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep4(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep5(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep6(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep7(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep8(const TDesC& aStepName);
	CTestStep* CreateRConnTestStep9(const TDesC& aStepName);
	CTestStep* CreateUpsTestStep(const TDesC& aStepName);
	};



#endif /* __TE_RConnectionSUITE_H__ */

