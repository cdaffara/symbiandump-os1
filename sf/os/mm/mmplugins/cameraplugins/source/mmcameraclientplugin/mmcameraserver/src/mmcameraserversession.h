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
// mmcamerasession.h
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef MMCAMERASERVERSESSION_H
#define MMCAMERASERVERSESSION_H

#include "mmcameraserver.h"
#include "mmcameraserverpolicymanager.h"


class CMMCameraServerSession : public CSession2
	{
friend class CCameraServerController;

public:
	CMMCameraServerSession();
	~CMMCameraServerSession();  // Close the camera channel. Remove Session from the server list

	void ServiceL(const RMessage2& aMessage);
	void CreateL();       // Add Session to the Server list. Should open the camera channel. Set Handle no.

	// Expose session parameters for use with policy manager
	TBool CollaborativeClient() const { return iCollaborativeClient; }
	TBool MMCapability() const { return iMMCapability; }
	TInt Handle() const { return iHandle; }
	TInt Priority() const { return iPriority; }
	TInt CameraIndex() const { return iCameraIndex; }

	void SetHandle(TInt aHandle);
	void SetPriority(TInt aPriority);
	void SetReserved(TBool aReserved);
	void SetMMCapability(TBool aMMCapability);
	void CompleteOverthrow();
	CMMCameraServerController* CameraController();  // Handle to Camera Controller

private:
	void Disconnect(const RMessage2& aMessage);

	void CamerasAvailable(const RMessage2& aMessage);

	void OpenCamera(const RMessage2& aMessage);

	void Reserve(const RMessage2& aMessage);
	void Release(const RMessage2& aMessage);
	void CancelReserve(const RMessage2& aMessage);

	void PowerOn(const RMessage2& aMessage);
	void PowerOff();
	void CancelPowerOn();

	void PrepareDirectViewFinder(const RMessage2& aMessage);
	void StartDirectViewFinder(const RMessage2& aMessage);
	void StopDirectViewFinder(const RMessage2& aMessage);
	void PauseDirectViewFinder(const RMessage2& aMessage);
	void ResumeDirectViewFinder(const RMessage2& aMessage);
	void SetDirectViewFinderMirror(const RMessage2& aMessage);
	void GetDirectViewFinderMirror(const RMessage2& aMessage);
	void DirectViewFinderState(const RMessage2& aMessage);
	void DirectViewFinderProperties(const RMessage2& aMessage);

	void SetCameraParameter(const RMessage2& aMessage);
	void GetCameraParameter(const RMessage2& aMessage);

	void CameraHandle(const RMessage2& aMessage);

private:
	CMMCameraServer* Server();   // Handle to Server

	void PanicClient(const RMessage2& aMessage, TServerPanic aPanic);

public:
    TDblQueLink iCamSessionLink;    // Link Object for Policy manager

private:
	CMMCameraServerController* iCameraController;  // Handle to Camera Controller
	CMMCameraServer* iCameraServer;
	CMMCameraServerPolicyManager* iPolicyManager;

	TInt iCameraIndex;   // details needed
	TInt iHandle;
	TInt iPriority;
	TBool iMMCapability;  // comes packed in RMessage2. Checked in open camera call
	TBool iCollaborativeClient;

	TBool iReserved;
	TBool iPoweredUp;

    RMessagePtr2 iOverthrowMessage;
  	};

#endif // MMCAMERASERVERSESSION_H
