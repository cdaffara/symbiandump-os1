/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@released
*/

#ifndef __LBSROOT_H__
#define __LBSROOT_H__

#include <f32file.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "LbsCommon.h"
#include "lbsrootapi.h"
#include "LbsExtendModuleInfo.h"
#include "lbsqualityprofile.h"
#include "lbssystemstatelistener.h"


class CProcessGuardian;
class TPositionModuleInfoExtended;
class CProcessOverseer;
class CProcessCloseTimer;

const TInt KMaxInfoModules = 5;


_LIT(KLbsRootPanic, "LBS Root Panic");

enum
    {
    ELbsPanicNoIniFile, // no .cre file found at startup
    // invalid values found when loading module info
    ELbsPanicBadFirstFixValue, 
    ELbsPanicBadNextFixValue, 
	KLbsPanicSystemStartFailure,
	// Invalid lbsprofile.ini contents
	ELbsPanicInvalidProfileIniFile,
	// Invalid CenRep contents on startup
	ELbsPanicInvalidCenRepContents
    };


class CLbsSystem : public CActive,
                   public MLbsSystemStartupObserver,
                   public MLbsSystemCloseDownObserver,
                   public MLbsStartupObserver
    {
    
enum
    {  
 	ELbsSystemUp,
 	ELbsSystemClosing,
 	ELbsSystemDown
    };    
    
// Values used to latch requests reeceived while the system is 
// coming up or going down    
enum
    {
	EActionNone,
	EActionBringLbsUp,
	EActionTakeLbsDown
    };
public:
    static CLbsSystem* NewL();
    ~CLbsSystem();
	void StopLbsNow();

public: // From MLbsSystemStartupObserver
    void OnStartupRequest();

public: // From MLbsSystemCloseDownObserver
    void OnCloseDownRequest(TBool aConfirmCloseDownComplete);
    void OnResetRequest();				// TEST ONLY
	void OnCompleteCloseDownRequest();	// TEST ONLY
	
public: // From MLbsStartupObserver
    void HandleStartupCompletionL();

public: // From CActive
    void RunL();
    void DoCancel();

protected:
    CLbsSystem();
    void ConstructL();

private:
    void StartProcessesL();
    TBool LbsIsPersistentL();
    TBool LbsRootIsPersistentL();
    void StartLbsL();
    void StopLbsPart1();
    void StopLbsPart2();
    void GetModuleInfoL(RFs& aFs,
                        TPositionModuleInfoExtended& aExtendedModuleInfo, 
                  const TDesC& aFileName);
    void InitializeModuleInfoL();
    
    void GetQualityProfileInfoL(RFs& aFs, 
    							RArray<TQualityProfile>& aQualityArray,
    							const TDesC& aFileName);
    void InitializeQualityProfileInfoL();
    
    void DefineLbsPropertiesL();
    void DeleteLbsProperties();
    void DeleteLbsPropertiesL();

private:
    CProcessOverseer*    iOverseer;
    CLbsSystemStartupRequestHandler* iStartupRequestHandler;
    CLbsSystemCloseDownRequestHandler* iCloseDownRequestHandler;

	TBool					iSystemIsUp;
	TBool					iLbsAlwaysUp;
	TBool					iLbsRootAlwaysUp;
	TInt					iState;
	TInt					iPendingAction;
	TBool					iConfirmCloseDownComplete;
	TBool					iReportStartupFailure;
	TInt					iUsers;
	TPositionModuleInfoExtended iModuleInfoArray[KMaxInfoModules];
	TBool					iCompleteCloseDown;
	
	CLbsSystemStateListener*    iSystemStateListener;
	};
  

class MLbsProcessGuardianObserver
    {
public:
	//
	// Called when the process a CProcessGuardian is monitoring
	// exits. A process may exit normally after being told to 
	// shutdown, or it may exit because it hase panicked, in 
	// which case aExitType will be EExitPanic.
	//
	virtual void OnProcessExit(const CProcessGuardian* aGuardian, 
							   TExitType aExitType,
							   const TDesC& aExitCategory,
							   TInt aExitReason) = 0;

	virtual void OnProcessCloseTimeout(const CProcessGuardian* aGuardian) =0;

    };

/**
 *
 * CProcessGuardian creates and watches a process.
 * Any panic in the monitored process is reported to the observer.
 *
 */
NONSHARABLE_CLASS( CProcessGuardian ): public CActive
	{	
public:
	static CProcessGuardian* NewL(const TDesC& aProcessName, 
								  TUid aUid, 
								  TBool aIsServer, 
								  MLbsProcessGuardianObserver& aObserver);
	virtual ~CProcessGuardian();
	
	TUid ProcessUid();
	TBool IsRunning();
	
	TInt Startup();
	TInt CloseDown();

protected:
	void RunL();
	void DoCancel();
        
private:
	CProcessGuardian(TUid aUid, TBool aIsServer, MLbsProcessGuardianObserver& aObserver);
	void ConstructL(const TDesC& aProcessName);
	TInt FindProcessAndLogon();
	
private:
	enum
	    {
		EProcessStateIdle,
		EProcessStateRunning,
		EProcessStateClosingDown
	    };
    
private:
	HBufC*					  iProcessName;
	TBool					  iIsServer;
	RProcess                  iProcess;
	TUid                      iUid;
	MLbsProcessGuardianObserver& iObserver;
	RLbsProcessSupervisor     iLbsSupervisor;
	TInt                      iState;
	CProcessCloseTimer*	      iCloseTimer;
	};

/**
 *
 * CProcessOverseer sets off the essential processes of the LBS system
 * and monitors their state. If any panics, then all are brought down and restarted.
 *
 */
NONSHARABLE_CLASS( CProcessOverseer ): public CBase, 
                                       public MLbsProcessGuardianObserver
    {
enum
    {
	EOverseerIdle,
	EOverseerStarting,
	EOverseerRunning,
	EOverseerClosingProcesses
    };
    
enum
    {
    EOverseerActionNone,
    EOverseerActionCloseProcesses
    };
    
public:    
	static CProcessOverseer* NewL();
	~CProcessOverseer();

	TInt StartupL();
	void CloseDown(TRequestStatus* aCloseDownCompletionStatus);
	void ResetProcesses();
	void StopProcesses();
	
	TBool IsRunning();
	
private: // From MLbsProcessGuardianObserver
	void OnProcessExit(const CProcessGuardian* aGuardian, 
					   TExitType aExitType,
					   const TDesC& aExitCategory,
					   TInt aExitReason);
	
	void OnProcessCloseTimeout(const CProcessGuardian* aGuardian);

protected:
    void ConstructL();    
    CProcessOverseer();

private:
	void AttachLbsProcessesL();
    void StartLbsProcessesL();
    void StopLbsProcesses();
	void CloseNextProcess();
    
private:	
    TInt              iState;
    TInt              iCreateStatus; // holds any error found on trying to start the processes
    TInt              iSystemRestartCount;
    TRequestStatus*   iCloseDownCompletionStatus;
    TInt 			  iCurrentProcess;
    RPointerArray<CProcessGuardian> iLbsProcess;
    };

 
/**
 *
 * CProcessCloseTimer reports timeouts when waiting for LBS processes to close
 *
 */
NONSHARABLE_CLASS( CProcessCloseTimer ): public CTimer
	{
public:
	static CProcessCloseTimer* NewL(const CProcessGuardian& aGuardian,
									 MLbsProcessGuardianObserver& aObserver);
	~CProcessCloseTimer();
	
	void Start(const TTimeIntervalMicroSeconds32 aDelay);
	void Stop();
	
protected: 
	// from CActive
	void RunL();

private:
	CProcessCloseTimer(const CProcessGuardian& aGuardian,
						  MLbsProcessGuardianObserver& aObserver);
	void ConstructL();

private:
	const CProcessGuardian& iGuardian;
	MLbsProcessGuardianObserver& iObserver;
	};
 

#endif // __LBSROOT_H__
