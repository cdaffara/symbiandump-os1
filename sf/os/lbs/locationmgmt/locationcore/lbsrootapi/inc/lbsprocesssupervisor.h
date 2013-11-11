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
// lbsrootapi.h
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __LBS_PROCESSSUPERVISOR_H__
#define __LBS_PROCESSSUPERVISOR_H__

#include <e32base.h>
#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>
#include <e32property.h>
#include "lbsprocessuiddefs.h"

/**
 * Class which allows processes to register to be notified for closedown,
 * and to confirm closedown when it is completed.
 */
class RLbsProcessSupervisor 
	{
public:
	/**
	 * Values for closedown request property which indicate whether the
	 * process is to close itself down or reset itself.
	 */
	enum TProcessSignal
		{
		EProcessSignalUnknown = 0,
		EProcessCloseDown,
		EProcessReset
		};

public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsProcessSupervisor();
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();

	IMPORT_C void CloseDownProcess();
	IMPORT_C void ResetProcess();

	void NotifyCloseDownRequest(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyCloseDownRequest();
	
	TInt ProcessSignal();

private:
    RProperty   iRequestCloseDown;
    TUid		iModuleId;
	};

/**
 * Mixin class defining a function implemented by LBS processes/servers started 
 * by LBS Root and which is called when the process/server is to close itself 
 * down
 */
class MLbsProcessCloseDown
    {
public:
    virtual void OnProcessCloseDown()  = 0;
    };

/**
 * Mixin class defining a function implemented by LBS processes/servers started 
 * by LBS Root and which is called when the process/server is to 'reset' itself
 * (e.g. clear internal cache, state variable). This is expected mainly to be 
 * used for test purposes.
 */
class MLbsProcessReset
    {
public:
    virtual void OnProcessReset()  = 0;
    };

	
class CLbsCloseDownRequestDetector : public CActive
	{
public:
	// Not all process need to implement a reset function, so allow a default
	// value of NULL for the reset observer.
	IMPORT_C static CLbsCloseDownRequestDetector* NewL(
									MLbsProcessCloseDown* aCloseDownObserver,
													TUid aProcessUid,
										MLbsProcessReset* aResetObserver = NULL);
	~CLbsCloseDownRequestDetector();

protected:
	CLbsCloseDownRequestDetector(MLbsProcessCloseDown* aCloseDown,
									MLbsProcessReset* aResetObserver);
	void ConstructL(TUid aProcessUid);
     
protected:
	//From CActive
	virtual void RunL();
	virtual void DoCancel();

private:	
	RLbsProcessSupervisor iLbsSupervisor;
	MLbsProcessCloseDown* iCloseDownObserver;
	MLbsProcessReset* iResetObserver;
    };
    
#endif //__LBS_PROCESSSUPERVISOR_H__
