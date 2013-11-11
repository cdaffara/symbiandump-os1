// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mmcamerapolicymanager.cpp
// 
//

/**
 @file
 @internalComponent
*/

#include "mmcameraserversession.h"
#include "mmcameraserverpolicymanager.h"
#include "mmcameraservercontroller.h"


CMMCameraServerPolicyManager* CMMCameraServerPolicyManager::NewL()
	{
	CMMCameraServerPolicyManager* self = new (ELeave) CMMCameraServerPolicyManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
CMMCameraServerPolicyManager::CMMCameraServerPolicyManager() : iReservedSessionQ(_FOFF(CMMCameraServerSession,iCamSessionLink)),
												iIter(iReservedSessionQ)
	{
	}

void CMMCameraServerPolicyManager::ConstructL()
	{
	}

CMMCameraServerPolicyManager::~CMMCameraServerPolicyManager()
	{	
	}

/**
 * Enforces client connection policies.
 * 
 * Called by the server as the client connects
 */
void CMMCameraServerPolicyManager::OnConnectL(const RMessage2& aMessage)
	{
	// All users must have UserEnvironment capability
	if (!KMMCameraServerPolicyUserEnvironment.CheckPolicy(aMessage, __PLATSEC_DIAGNOSTIC_STRING("CMMCameraServerPolicyManager::OnConnectL KMMCameraServerPolicyUserEnvironment")))
		{
		User::Leave(KErrPermissionDenied);
		}
	}

/**
 * Performs further MultimediaDD policy check on clients connecting to the server.
 * 
 * Called at the start of the Sessions ServiceL routine
 */
void CMMCameraServerPolicyManager::ServiceHandlerL(const RMessage2& aMessage)
	{
	// If client is connecting to the server, make sure correct capabilities are set
	if (aMessage.Function() == ECamOpenCamera)
		{
		// Insert MMCapability into message buffer
		TOpenCamera parameters;
		TOpenCameraPckg openCamBuf(parameters);
		aMessage.ReadL(TInt(0), openCamBuf);

		parameters = openCamBuf();

		if (KMMCameraServerPolicyMultimediaDD.CheckPolicy(aMessage, __PLATSEC_DIAGNOSTIC_STRING ("CMMCameraServerPolicyManager::ServiceHandlerL KMMCameraServerPolicyMultimediaDD")))
			{
			parameters.iMMCapability = ETrue;
			}
		else
			{
			parameters.iMMCapability = EFalse;
			}

	    TOpenCameraPckg buf(parameters);

	    aMessage.WriteL(TInt(0), buf);	
		}

	// Add code here if necessary to check
	// clients ability to access particular
	// functions
	}

/**
 * Used by ReserveClient() to find first reserved client session.
 */
CMMCameraServerSession* CMMCameraServerPolicyManager::FindFirstInQue(TInt aIndex)
	{
	CMMCameraServerSession* pS = NULL;
	iIter.SetToFirst();

	while ((pS = iIter++) != NULL)
		{
		if (pS->CameraIndex() == aIndex)
			{
			break;
			}
		}

	return pS;
	}

/**
 * Used by CheckControlOvertaking() to find last client in queue that has reserved the camera.
 */
CMMCameraServerSession* CMMCameraServerPolicyManager::FindLastInQue(TInt aIndex)
	{
	CMMCameraServerSession *pS, *ret = NULL;
	iIter.SetToFirst();

	while ((pS = iIter++) != NULL)
		{
		if (pS->CameraIndex() == aIndex)
			{
			ret = pS;
			}
		}

	return ret;
	}

/**
 * Attempts to reserve the specific camera which the client requested.
 */
TBool CMMCameraServerPolicyManager::ReserveClient(CMMCameraServerSession* aSession)
	{
	TInt camIndex = aSession->CameraIndex();

	// Check if this is the first client reserving the camera device
	if (FindFirstInQue(camIndex) == NULL)
		{
		ReserveUpdate(aSession);
		return ETrue;
		}
	else 
		{
		// If not first client, check if it is a collaborative client with the one which has already reseved the camera
		if(aSession->CollaborativeClient())
			{
			// Get the First session in the queue
			// All collaborative clients require the same MMCapability
			CMMCameraServerSession* firstSession = FindFirstInQue(camIndex);

			if (aSession->MMCapability() == firstSession->MMCapability())
				{
				// Set the priority of the collaborative client to that of the first client
				aSession->SetPriority(firstSession->Priority());
				iReservedSessionQ.AddLast(*aSession);
				return ETrue;
				}
			}

		// See if new client has higher priority than current client
		if(CheckControlOvertaking(aSession)) 
			{
			CMMCameraServerSession* pS = NULL;
			iIter.SetToFirst();

			aSession->CameraController()->Reset();

			while ((pS = iIter++) != NULL)
				{
				// We Deque and overthrow those clients that 
				// belong to the same camIndex as this client
				if (pS->CameraIndex() == camIndex)
					{
					// set reserve status of previously reserved client to EFalse. 
					pS->SetReserved(EFalse);
					pS->CompleteOverthrow();

					//removes from controller's 'reserved session' queue
					pS->iCamSessionLink.Deque();
					pS->iCamSessionLink.iNext = NULL;
					}
				}

			// update details wrt new reserved client. 
			ReserveUpdate(aSession);
			return ETrue;
			}
		}

	return EFalse;
	}

/**
 * Updates reserve status of clients. Used by ReserveClient().
 */
void CMMCameraServerPolicyManager::ReserveUpdate(CMMCameraServerSession* aSession)
	{
	aSession->SetHandle(aSession->CameraController()->CameraHandle());

	iReservedSessionQ.AddLast(*aSession);
	}

/**
 * Determines whether overthrowing client has higher priority than current client.
 * Used by ReserveClient().
 */
TBool CMMCameraServerPolicyManager::CheckControlOvertaking(CMMCameraServerSession* aSession)
	{
	TInt camIndex = aSession->CameraIndex();
	CMMCameraServerSession* lastSession = FindLastInQue(camIndex);

	if(aSession->MMCapability() && !(lastSession->MMCapability()))  
		{
		return ETrue;
		}
	
	if(aSession->MMCapability() == lastSession->MMCapability())
		{
		if(aSession->Priority() > lastSession->Priority())
			{
			return ETrue;
			}
		}

	return EFalse;
	}

/**
 * Releases the camera device.
 */
void CMMCameraServerPolicyManager::ReleaseClient(CMMCameraServerSession* aSession)
	{
	if(!iReservedSessionQ.IsEmpty())
		{
		aSession->iCamSessionLink.Deque();
		aSession->iCamSessionLink.iNext = NULL;
		}
	}
