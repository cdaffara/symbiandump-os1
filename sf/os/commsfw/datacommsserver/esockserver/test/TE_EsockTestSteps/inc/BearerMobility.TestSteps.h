/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file BearerMobility.TestSteps.h
*/
#if (!defined BEARERMOBILITY_TESTSTEPS_H)
#define BEARERMOBILITY_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"

#include <comms-infras/mobilitymessagesecom.h>


// Accept new carrier
//-------------------

class Cacceptcarrierrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cacceptcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kacceptcarrierrcommsmobilityapiext,"acceptcarrierrcommsmobilityapiextStep");


// Reject new carrier
//-------------------

class Crejectcarrierrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Crejectcarrierrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Krejectcarrierrcommsmobilityapiext,"rejectcarrierrcommsmobilityapiextStep");


// Mobility notification
//----------------------

class Cmobilitynotifyrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cmobilitynotifyrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kmobilitynotifyrcommsmobilityapiext,"mobilitynotifyrcommsmobilityapiextStep");

// Cancel mobility notification
//-----------------------------
class Ccancelnotifyrcommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Ccancelnotifyrcommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	virtual TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kcancelnotifyrcommsmobilityapiext,"cancelnotifyrcommsmobilityapiextStep");


// Cancel notification
//--------------------

class CCancelMobilityNotificationStep : public CTe_EsockStepBase
	{
public:
	CCancelMobilityNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
	};

_LIT(KCancelMobilityNotificationStep,"CancelMobilityNotificationStep");


// Migrate carrier
//----------------

class Cmigratercommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cmigratercommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(Kmigratercommsmobilityapiext,"migratercommsmobilityapiextStep");



// Ignore
//-------

class Cignorercommsmobilityapiext : public CTe_EsockStepBase
	{
public:
	Cignorercommsmobilityapiext(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
	};

_LIT(Kignorercommsmobilityapiext,"ignorercommsmobilityapiextStep");


// Check mobility notification
//----------------------------
class CCheckMobilityNotificationStep : public CTe_EsockStepBase
	{
public:
	CCheckMobilityNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TEventContainerParams iParams; //current params (.ini)
    TInt iOldAP;
    TInt iNewAP;
    TBuf<KMaxTestExecuteNameLength> iEventName;
    TInt iExpectedError;
    TInt iExpectedError2;
	};

_LIT(KCheckMobilityNotificationStep,"CheckMobilityNotificationStep");


// Close mobility API extension
//-----------------------------
class CCloseRCommsMobilityAPIExtStep : public CTe_EsockStepBase
	{
public:
	CCloseRCommsMobilityAPIExtStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
	};

_LIT(KCloseRCommsMobilityAPIExtStep,"CloseRCommsMobilityAPIExtStep");


// Open mobility API extension
//----------------------------
class COpenRCommsMobilityAPIExtStep : public CTe_EsockStepBase
	{
public:
	COpenRCommsMobilityAPIExtStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(KOpenRCommsMobilityAPIExtStep,"OpenRCommsMobilityAPIExtStep");



/**
Class implementing CMigrateToPreferredCarrierStep

@internalComponent
*/
class CMigrateToPreferredCarrierStep : public CTe_EsockStepBase
	{
public:
	CMigrateToPreferredCarrierStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TMigrateToPreferredCarrierParams iParams; //current params (.ini)
	};

_LIT(KMigrateToPreferredCarrierStep,"MigrateToPreferredCarrierStep");


/**
Class implementing CIgnorePreferredCarrierStep

@internalComponent
*/
class CIgnorePreferredCarrierStep : public CTe_EsockStepBase
	{
public:
	CIgnorePreferredCarrierStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TMigrateToPreferredCarrierParams iParams; //current params (.ini)
	};

_LIT(KIgnorePreferredCarrierStep,"IgnorePreferredCarrierStep");

/**
Class implementing CNewCarrierAcceptedStep

@internalComponent
*/
class CNewCarrierAcceptedStep : public CTe_EsockStepBase
	{
public:
	CNewCarrierAcceptedStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TMigrateToPreferredCarrierParams iParams; //current params (.ini)
	};

_LIT(KNewCarrierAcceptedStep,"NewCarrierAcceptedStep");


/**
Class implementing CNewCarrierRejectedStep

@internalComponent
*/
class CNewCarrierRejectedStep : public CTe_EsockStepBase
	{
public:
	CNewCarrierRejectedStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TMigrateToPreferredCarrierParams iParams; //current params (.ini)
	};

_LIT(KNewCarrierRejectedStep,"NewCarrierRejectedStep");


_LIT(KTe_AvailabilityNotificationResponse,"AvailabilityNotifResponse");


/**
Class implementing GetAvailabilityAPIStep

@internalComponent
*/
class CGetAvailabilityAPIStep : public CTe_EsockStepBase
	{
public:
	CGetAvailabilityAPIStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iAvailabilityExtName;
    TPtrC iConnectionName;
	};

_LIT(KGetAvailabilityAPIStep,"GetAvailabilityAPIStep");


/**
Class implementing GetMobilityAPIStep

@internalComponent
*/
class CGetMobilityAPIStep : public CTe_EsockStepBase
	{
public:
	CGetMobilityAPIStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iMobilityExtName;
    TPtrC iConnectionName;
	};

_LIT(KGetMobilityAPIStep,"GetMobilityAPIStep");


/**
Class implementing RegisterForMobilityNotificationStep

@internalComponent
*/
class CRegisterForMobilityNotificationStep : public CTe_EsockStepBase
	{
public:
	CRegisterForMobilityNotificationStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TEventContainerParams iParams; //current params (.ini)
	TBuf<KMaxTestExecuteNameLength> iEventName;
	};

_LIT(KRegisterForMobilityNotificationStep,"RegisterForMobilityNotificationStep");

#endif // BEARERMOBILITY_TESTSTEPS_H

