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
//

/**
 @file
 @internalComponent
*/

#include "mmcameraserver.h"
#include "mmcameraserversession.h"
#include "mmcameraservercontroller.h"
#include "mmcameraservershutdown.h"


CMMCameraServer* CMMCameraServer::NewLC()
	{
	CMMCameraServer* self = new(ELeave) CMMCameraServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMMCameraServer::CMMCameraServer() 
	: CServer2 (CActive::EPriorityStandard),
    iCamSessionCount(0),
    iCamControllerQ(_FOFF(CMMCameraServerController,iCamCntrlLink))
	{
	}

CMMCameraServer::~CMMCameraServer()
	{
	if (iShutdown)
		{
		iShutdown->Cancel();
		}

	while (!iCamControllerQ.IsEmpty())   // free the memory on the heap used for controller queues 
		{
		CMMCameraServerController *pCntrl = iCamControllerQ.First();
		delete pCntrl;
		}

	delete iPolicyManager;
	delete iShutdown;
	}

void CMMCameraServer::ConstructL()
	{
	iPolicyManager = CMMCameraServerPolicyManager::NewL();
	StartL(KMMCameraServerName);

	iShutdown = CMMCameraServerShutdown::NewL();
	iShutdown->Start();
	}

void CMMCameraServer::AddSession()
	{
    ++iCamSessionCount;
    if(iCamSessionCount == 1)
        {
    	iShutdown->Cancel();
        }
	}

void CMMCameraServer::DropSession()
	{
	--iCamSessionCount; 
	if(iCamSessionCount == 0)
	    {
	    if (!iShutdown->IsActive())
			{
	    	iShutdown->Start();
			}
	    }
	}

/**
 * Provides a Camera controller object for the specified camera index.
 */
void CMMCameraServer::GetCameraControllerL(TInt aCameraIndex, CMMCameraServerController*& aCameraController)
	{
	TDblQueIter<CMMCameraServerController> controllerIterator(iCamControllerQ);
	controllerIterator.SetToFirst();
	CMMCameraServerController* pCntrl = controllerIterator;

	// loop through iterator to find desired camera controller
	while(pCntrl)
		{
		if(aCameraIndex == pCntrl->CameraIndex())
			{
			// found required controller
			aCameraController = controllerIterator;
			break;
			}
		controllerIterator++;
		pCntrl = controllerIterator;
		}

	// No controllers exist for specified camera index so create new one and append to queue
	if(!pCntrl)
		{
		aCameraController = CMMCameraServerController::NewL(aCameraIndex);
		iCamControllerQ.AddLast(*aCameraController);
		}
	}

void CMMCameraServer::GetCameraControllerQueryL(CMMCameraServerControllerQuery*& aCameraControllerQuery)
	{
	aCameraControllerQuery = CMMCameraServerControllerQuery::NewL();
	}

CSession2* CMMCameraServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
	{
	// Check that the version is OK
    TVersion version(KECamServMajorVerNumber, KECamServMinorVerNumber, KECamServBuildVerNumber);
    if (!User::QueryVersionSupported(version, aVersion))
    	{
    	User::Leave(KErrNotSupported);
    	}

	iPolicyManager->OnConnectL(aMessage);
	
	return new(ELeave) CMMCameraServerSession();
	}

TInt CMMCameraServer::RunError(TInt aError)
	{
	Message().Complete(aError); //send error back to client
	// should have :- if(!IsActive())
	ReStart();

	return (KErrNone);
	}

void CMMCameraServer::PanicClient(const RMessage2& aMessage, TServerPanic aPanic)
	{
	_LIT(KPanic, "MMCameraServer");
	aMessage.Panic(KPanic, aPanic);
	}
