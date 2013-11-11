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
 @file te_BearerMobilityTestSuite.cpp
*/

#if (!defined TE_BEARERMOBILITYTESTSUITE_H)
#define TE_BEARERMOBILITYTESTSUITE_H

#include <comms-infras/es_mobility_apiext.h>

#include <e32std.h>
#include <e32base.h>
#include <cdbstore.h>
#include <es_sock.h>
#include <in_sock.h>
#include <es_enum.h>
#include <commdbconnpref.h>
#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>

#include <comms-infras/es_availability.h>

_LIT(KErrTestStepAstray, "Test step state has gone astray");

// Test server
//------------
class CBearerMobilitySuiteTestServer: public CTestServer
	{
	public:
	static CBearerMobilitySuiteTestServer* NewL();
	~CBearerMobilitySuiteTestServer();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};


// Test Steps
//-----------
class CBearerMobilityTestStepBase : public CTestStep
	{
public:
	virtual ~CBearerMobilityTestStepBase() {}

protected:
	void SetAvailabilityL(TInt aAccessPointId, ESock::TAvailabilityStatus aStatus);
	};


class CBearerMobilityTestStepWithAOBase : public CBearerMobilityTestStepBase, public MMobilityProtocolResp
	{
public:
	virtual ~CBearerMobilityTestStepWithAOBase() {}

protected:
	void BumpTestStepState(void) {iTestState++;}
	TInt GetTestStepState() const {return iTestState;}

private:
	TInt iTestState;
	};


class CBearerMobilityTestStep_761402 : public CBearerMobilityTestStepWithAOBase
	{
public:
	virtual ~CBearerMobilityTestStep_761402() {}
	enum TVerdict doTestStepL(void);

private:
	// MMobilityProtocolResp
	void PreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless);
	void NewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless);
	void Error(TInt aError);

	RConnection iConnection;
	RSocketServ iSocketServ;
	CActiveCommsMobilityApiExt* iMobilityExtension;
	};

class CBearerMobilityTestStep_761403 : public CBearerMobilityTestStepBase
	{
public:
	virtual ~CBearerMobilityTestStep_761403() {}
	enum TVerdict doTestStepL(void);
	};

class CBearerMobilityTestStep_761404 : public CTestStep
	{
public:
	virtual ~CBearerMobilityTestStep_761404() {}
	enum TVerdict doTestStepL(void);
	};
	
class CBearerMobilityTestStep_761405 : public CTestStep
	{
public:
	virtual ~CBearerMobilityTestStep_761405() {}
	enum TVerdict doTestStepL(void);
	};

#endif /* TE_BEARERMOBILITYTESTSUITE_H */

