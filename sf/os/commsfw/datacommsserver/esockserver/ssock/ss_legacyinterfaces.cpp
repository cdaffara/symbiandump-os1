// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/
#include "ss_legacyinterfaces.h"

#include <comms-infras/api_ext_list.h>
#include <comms-infras/api_ext_msg.h>
#include <cdblen.h>
#include <commdb.h>
#include <ss_glob.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_connlegacy.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/metatype.h>
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_connlegacy.h>
#include "SS_conn.H"

#include <comms-infras/ss_nodemessages_internal_esock.h>
#include "ss_internal_activities.h" 
// although we don't need the XConnectionQueryBase::LoadL() method
// including this header forces us to link against commsdataobjects.lib
// when compiling with Armv5
#include <comms-infras/connectionqueryset.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/ss_roles.h>
#include <ss_sock.h>
#include "SS_rslv.H"
#include <comms-infras/ss_subconnflow.h>
#include <elements/responsemsg.h>

using namespace ESock;
using namespace Elements;
using namespace Messages;
using namespace MeshMachine;



EXPORT_C ALegacySubConnectionActiveApiExt::~ALegacySubConnectionActiveApiExt()
	{
	iRequests.ResetAndDestroy();
	}


void ALegacySubConnectionActiveApiExt::IsSubConnectionActiveRequest(CLegacySubConnectionActiveResponder& aResponder)
	{
	// Create a request structure for this client.
	CClientRequest* request = new CClientRequest(aResponder.ClientId(), aResponder);
	if (!request)
		{
		aResponder.Error(KErrNoMemory);
		return;
		}

	// Find the data monitoring provider associated with the parent node.
	ADataMonitoringProvider* dmProvider = NULL;
	TRAP_IGNORE(dmProvider = static_cast<ADataMonitoringProvider*>(iParent->FetchNodeInterfaceL(EDataMonitoringApiExt)));
	if (!dmProvider)
		{
		// There's no way the ipproto CPR and SCPR can't be data monitoring providers
		aResponder.Error(KErrCorrupt);
		request->iResponder = NULL;
		delete request;
		return;
		}
	TDataVolumes* dmVolumes = dmProvider->DataVolumesPtr();

	// Initialise the request structure with the current data counts for
	// comparison when the timer fires.
	request->iInitialSentVolume = dmVolumes->iSentBytes;
	request->iInitialReceivedVolume = dmVolumes->iReceivedBytes;

	// Create a new timer for this request and record it in the request structure.
	CActivityTimer* timer = NULL;
	TRAP_IGNORE(timer = CActivityTimer::NewL(*this, request, KActivityTimerPriority));
	if (!timer)
		{
		aResponder.Error(KErrNoMemory);
		request->iResponder = NULL;
		delete request;
		return;
		}

	request->iTimer = timer;

	// Store the request and kick off the timer.
	TInt error = iRequests.Append(request);
	if (error!=KErrNone)
		{
		aResponder.Error(error);
		request->iResponder = NULL;
		delete request;
		return;
		}

	request->iTimer->After(request->iResponder->TimerPeriod() * KMicrosecondsInASecond);
	}

void ALegacySubConnectionActiveApiExt::CancelIsSubConnectionActiveRequest(TSubSessionUniqueId aClientId)
	{
	// Find the outstanding request, remove it from the array, complete the client, delete the timer, delete the request.
	for(TInt i=0;i<iRequests.Count();i++)
		{
		if(iRequests[i]->iClientId == aClientId)
			{
			CClientRequest* request = iRequests[i];
			iRequests.Remove(i);

			ASSERT(request);
			delete request;
			}
		}
	}

EXPORT_C void ALegacySubConnectionActiveApiExt::CancelExtensionInterface(TSubSessionUniqueId aClientId)
	{
	CancelIsSubConnectionActiveRequest(aClientId);
	}

void ALegacySubConnectionActiveApiExt::CheckSubConnectionActivityL(CClientRequest* aRequest)
	{
	ASSERT(aRequest);

	ADataMonitoringProvider* dmProvider = static_cast<ADataMonitoringProvider*>(iParent->FetchNodeInterfaceL(EDataMonitoringApiExt));
	User::LeaveIfError(dmProvider ? KErrNone : KErrCorrupt); // There's no way the ipproto CPR and SCPR can't be data monitoring providers
	TDataVolumes* dmVolumes = dmProvider->DataVolumesPtr();

	TUint newSentDataVolume = dmVolumes->iSentBytes;
	TUint newReceivedDataVolume = dmVolumes->iReceivedBytes;

	TBool dataTransferred = (newSentDataVolume != aRequest->iInitialSentVolume) ||
							(newReceivedDataVolume != aRequest->iInitialReceivedVolume);

	// If the data transferred volumes haven't change but the client thinks the connection is active...
	if (aRequest->iResponder->ClientPerceivedState())
		{
		if(dataTransferred)	// ...and it is, so just start another timer cycle
			{
			aRequest->iInitialSentVolume = newSentDataVolume;
			aRequest->iInitialReceivedVolume = newReceivedDataVolume;

			aRequest->iTimer->After(aRequest->iResponder->TimerPeriod() * KMicrosecondsInASecond);
			}
		else				// ...tell them it's not
			{
			aRequest->SubConnectionActive(dataTransferred);

			for(TInt i=0;i<iRequests.Count();i++)
				{
				if(iRequests[i]->iClientId == aRequest->iClientId)
					{
					CClientRequest* request = iRequests[i];
					iRequests.Remove(i);
					delete request;
					}
				}
			}
		}
	else					// client believes subconnection is idle...
		{
		if(dataTransferred)
			{
			aRequest->SubConnectionActive(dataTransferred);
			

			for(TInt i=0;i<iRequests.Count();i++)
				{
				if(iRequests[i]->iClientId == aRequest->iClientId)
					{
					CClientRequest* request = iRequests[i];
					iRequests.Remove(i);
					delete request;
					}
				}
			}
		else				// ...and it is, so just start another timer cycle
			{
			aRequest->iInitialSentVolume = newSentDataVolume;
			aRequest->iInitialReceivedVolume = newReceivedDataVolume;

			aRequest->iTimer->After(aRequest->iResponder->TimerPeriod() * KMicrosecondsInASecond);
			}
		}
	}


CClientRequest::~CClientRequest()
	{
	delete iTimer;
	if (iResponder)
		{
		iResponder->Complete(KErrCancel);
		}
	}


void CClientRequest::SubConnectionActive(TBool aState)
	{
	iResponder->SubConnectionActive(aState);
	iResponder = NULL;
	}

/**
	Construct a new CActivityTimer()

	@param aOwner The owning ALegacySubConnectionActiveApiExt (on which we call methods upon timer completion)
	@param aPriority The priority of the active object underlying this timer object
	@return A pointer to the newly constructed CActivityTimer object

	@internalComponent
*/
ESock::CActivityTimer* ESock::CActivityTimer::NewL(ALegacySubConnectionActiveApiExt& aOwner, CClientRequest* aRequest, TInt aPriority)
	{
	ESock::CActivityTimer* newActivityTimer =
		new(ELeave) ESock::CActivityTimer(aOwner, aRequest, aPriority);

	CleanupStack::PushL(newActivityTimer);
	newActivityTimer->ConstructL();
	CleanupStack::Pop(newActivityTimer);

	return newActivityTimer;
	}

/**
	Call the owning object's check activity method.

	@internalComponent
*/
void ESock::CActivityTimer::RunL()
	{
	iOwner.CheckSubConnectionActivityL(iRequest);
	}


