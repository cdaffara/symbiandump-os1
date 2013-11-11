// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/
#ifndef __LBS_SYSTEMSTARTER_H__
#define __LBS_SYSTEMSTARTER_H__

#include <e32base.h>
#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>
#include <e32property.h>
#include "lbstimer.h"

class CLbsSystemStarter;


const TInt KDefaultStartupTimeout = 30;

// Responses to a request to start the LBS system
enum
    {
    ELbsStartupSuccess,
    ELbsStartupFail,
    ELbsStartupRootNotFound
    };
// Responses to a request to close down the LBS system
enum
    {
    ELbsCloseDownRejected,
    ELbsCloseDownInProgress,
    ELbsCloseDownComplete,
    ELbsCloseDownRootNotFound
    };

class RLbsSystemController
    {
public:
enum
    {
	ELbsCloseDown = 1,
	ELbsCloseDownAndConfirm,
	ELbsResetOnly, // Just tell the process to clear the cache etc. rather than close down completely
	ELbsTotalCloseDown // Close the LBS Root process as well.
    };
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsSystemController();
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();
	
	IMPORT_C void NotifySystemStartRequest(TRequestStatus& aStatus);
	IMPORT_C TInt RequestSystemStartup();
	IMPORT_C void SystemStartComplete(TInt aStartupResult);
	IMPORT_C void NotifySystemCloseDownRequest(TRequestStatus& aStatus);
	IMPORT_C TInt RequestSystemCloseDown();
	IMPORT_C TInt RequestSystemCloseDown(TBool aNoWaitForResponse);
	IMPORT_C void SystemCloseDownResponse(TInt aResponse);
	IMPORT_C TInt GetCloseDownType();
	IMPORT_C void RequestSystemReset();
	IMPORT_C TInt RequestCompleteCloseDown();
	
	TInt GetStartupData(TDes8& aData);

private:
	TBool CheckLbsRootProcessIsUp();
	
private: 
	RProperty    iLbsStartupRequest;
	RProperty    iLbsStartupComplete;
	RProperty    iLbsCloseDownRequest;
	RProperty    iLbsCloseDownComplete;
	};

/*
 *
 * Observer used to pass system startup requests to the LBS Root process
 */
class MLbsSystemStartupObserver
	{
public:
	virtual void OnStartupRequest() = 0;
	};
    
/*
 * Active object which monitors the property which signals
 * when startup is required.
 */
class  CLbsSystemStartupRequestHandler : public CActive 
	{
public:
	IMPORT_C static CLbsSystemStartupRequestHandler* NewL(
                                 MLbsSystemStartupObserver* aObserver, 
                                                         TUid aProcessUid);
	IMPORT_C ~CLbsSystemStartupRequestHandler();
	IMPORT_C void ConfirmStartup(TInt aStartupStatus);

protected:
	CLbsSystemStartupRequestHandler(MLbsSystemStartupObserver* aObserver);
	void ConstructL(TUid aProcessUid);

protected: // From CActive
	void RunL();
	void DoCancel();
    
private:
	MLbsSystemStartupObserver* iObserver;
	RLbsSystemController iLbsSystemController;    
	};
    
/*
 *
 * Observer used to pass system startup requests to the LBS Root process
 */
class MLbsSystemCloseDownObserver
    {
public:
    virtual void OnCloseDownRequest(TBool aConfirmCloseDownComplete) = 0;
    virtual void OnResetRequest() = 0;
    virtual void OnCompleteCloseDownRequest() = 0;
    };
    
/*
 * Active object which monitors the property which signals
 * when closedown is required.
 */
class  CLbsSystemCloseDownRequestHandler : public CActive 
	{
public:
	IMPORT_C static CLbsSystemCloseDownRequestHandler* NewL(
							MLbsSystemCloseDownObserver* aObserver, 
													TUid aProcessUid);
    IMPORT_C ~CLbsSystemCloseDownRequestHandler();
    IMPORT_C void ConfirmCloseDown(TInt aResponse);

protected:
    CLbsSystemCloseDownRequestHandler(MLbsSystemCloseDownObserver* aObserver);
    void ConstructL(TUid aProcessUid);

protected: // From CActive
    void RunL();
    void DoCancel();
    
private:
    MLbsSystemCloseDownObserver* iObserver;
    RLbsSystemController iLbsSystemController;    
    };
    
#endif //__LBS_SYSTEMSTARTER_H__
