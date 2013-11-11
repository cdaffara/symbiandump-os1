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

#include <centralrepository.h>
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>

#include "lbsrootapi.h"
#include "lbsprocesssupervisor.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"


/* Data published on the RProperty used to send a close down
   (or reset) signal to a process.
*/
struct SCloseDownData
	{
	TUid iModuleId; /* Uid of the process this is intended for. */
	RLbsProcessSupervisor::TProcessSignal iSignal; /* Signal - either close down or reset. */
	};


EXPORT_C void RLbsProcessSupervisor::InitializeL()
	{
	const TSecurityPolicy KReadPolicy(ECapabilityLocation);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);
		
	// Define a single property for shutting down all the processes
	TInt err = RProperty::Define(KLbsCloseDownBaseKey, 
								 RProperty::EByteArray, 
								 KReadPolicy, 
								 KWritePolicy,
								 sizeof(SCloseDownData));
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	if (err != KErrAlreadyExists)
		{
		// Set the initial data in the property
		SCloseDownData initialData;
		initialData.iModuleId = TUid::Uid(0);
		initialData.iSignal = EProcessSignalUnknown;
		TPckg<SCloseDownData> initialDataPckg(initialData);
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), 
										  KLbsCloseDownBaseKey, 
										  initialDataPckg));
			
		// Save the Uid of the process which defined the properties
		// in the cenrep file owned by LbsRoot.
		CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
		User::LeaveIfError(rep->Set(KProcessSupervisorCategoryKey, 
									TInt(RProcess().SecureId())));
		CleanupStack::PopAndDestroy(rep);
		}
	}

EXPORT_C void RLbsProcessSupervisor::ShutDownL()
	{
	// Delete the properties created in InitializeL().	
	RProperty::Delete(KLbsCloseDownBaseKey);
	}

EXPORT_C RLbsProcessSupervisor::RLbsProcessSupervisor() :
	iModuleId(TUid::Uid(0))
	{
	}
	
EXPORT_C void RLbsProcessSupervisor::OpenL(TUid aModuleId)
	{
	__ASSERT_DEBUG(iRequestCloseDown.Handle()==NULL, User::Invariant());		//Handle open.
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	TInt err = rep->Get(KProcessSupervisorCategoryKey, category);
	User::LeaveIfError(err);
	categoryUid = TUid::Uid(category);
	CleanupStack::PopAndDestroy(rep);
	
	iModuleId = aModuleId;
	User::LeaveIfError(iRequestCloseDown.Attach(categoryUid, KLbsCloseDownBaseKey));
	}
	
EXPORT_C void RLbsProcessSupervisor::Close()
	{
	if(iRequestCloseDown.Handle() != NULL)
		{
		iRequestCloseDown.Cancel();
		iRequestCloseDown.Close();
		}
	}

	
void RLbsProcessSupervisor::NotifyCloseDownRequest(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iRequestCloseDown.Handle()!=NULL, User::Invariant());		//Handle not open.
	iRequestCloseDown.Subscribe(aStatus);
	}

EXPORT_C void RLbsProcessSupervisor::CancelNotifyCloseDownRequest()
	{
	__ASSERT_DEBUG(iRequestCloseDown.Handle()!=NULL, User::Invariant());		//Handle not open.
	iRequestCloseDown.Cancel();
	}

EXPORT_C void RLbsProcessSupervisor::CloseDownProcess()
    {
	__ASSERT_DEBUG(iRequestCloseDown.Handle()!=NULL, User::Invariant());
	SCloseDownData data;
	data.iModuleId = iModuleId;
	data.iSignal = EProcessCloseDown;
	TPckg<SCloseDownData> dataPckg(data);
	iRequestCloseDown.Set(dataPckg);
    }

EXPORT_C void RLbsProcessSupervisor::ResetProcess()
    {
	__ASSERT_DEBUG(iRequestCloseDown.Handle()!=NULL, User::Invariant());		//Handle not open.
	SCloseDownData data;
	data.iModuleId = iModuleId;
	data.iSignal = EProcessReset;
	TPckg<SCloseDownData> dataPckg(data);
	iRequestCloseDown.Set(dataPckg);
    }

TInt RLbsProcessSupervisor::ProcessSignal()
	{
	SCloseDownData data;
	TPckg<SCloseDownData> dataPckg(data);
	TInt err = iRequestCloseDown.Get(dataPckg);
	if(err != KErrNone)
		{
		return(err);
		}
	
	// If this signal is intended for this module 
	// then return it.
	if (data.iModuleId == iModuleId
		|| data.iSignal == RLbsProcessSupervisor::EProcessReset)
		{
		return (data.iSignal);	
		}
	
	return KErrNotFound;
	}

//======================================================================

EXPORT_C CLbsCloseDownRequestDetector* CLbsCloseDownRequestDetector::NewL(
                                              MLbsProcessCloseDown* aCloseDownObserver, 
                                                              TUid aProcessUid,
												MLbsProcessReset* aResetObserver)
    {
	CLbsCloseDownRequestDetector* self;
	self = new (ELeave)CLbsCloseDownRequestDetector(aCloseDownObserver, 
													aResetObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessUid);
	CleanupStack::Pop(self);
	return(self);
    }
    
CLbsCloseDownRequestDetector::~CLbsCloseDownRequestDetector()
    {
    Cancel();
	iLbsSupervisor.Close();
    }

CLbsCloseDownRequestDetector::CLbsCloseDownRequestDetector(
									MLbsProcessCloseDown* aCloseDownObserver,
										MLbsProcessReset* aResetObserver) 
: CActive(EPriorityStandard),
  iCloseDownObserver(aCloseDownObserver),
  iResetObserver(aResetObserver)
    {
	CActiveScheduler::Add(this);
    }

void CLbsCloseDownRequestDetector::ConstructL(TUid aProcessUid)
	{
	iLbsSupervisor.OpenL(aProcessUid);
	iLbsSupervisor.NotifyCloseDownRequest(iStatus);
	SetActive();
	}
    
void CLbsCloseDownRequestDetector::RunL()
    {
    // Immediately re-subscribe for further signals
    iLbsSupervisor.NotifyCloseDownRequest(iStatus);
    SetActive();
    
    // Process the close down signal
    TInt processSignal = iLbsSupervisor.ProcessSignal();
    switch(processSignal)
    	{
    	case RLbsProcessSupervisor::EProcessCloseDown:
    		{
    		iCloseDownObserver->OnProcessCloseDown();
    		break;
    		}
    		
    	case RLbsProcessSupervisor::EProcessReset:
    		{
    		// Process is to be signalled to reset itself. 
    		// Not all processes implement this, so check 
    		// the observer has been defined befoere calling it.
    		if(iResetObserver)
    			{
    			iResetObserver->OnProcessReset();    		
    			}
    		break;
    		}
    		
		default:
			{
			// Some sort of error. Don't do anything, but allow 
			// further requests.
			break;
			}
    	}
    }
    
void CLbsCloseDownRequestDetector::DoCancel()
    {
	iLbsSupervisor.CancelNotifyCloseDownRequest();
    }

