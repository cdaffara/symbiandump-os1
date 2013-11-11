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
 @internalTechnology
*/

#ifndef MMCAMERASERVER_H
#define MMCAMERASERVER_H

#include "mmcameraclientinterface.h"
#include "mmcameraserverpolicymanager.h"

const TInt KECamServMajorVerNumber = 0;
const TInt KECamServMinorVerNumber = 0;
const TInt KECamServBuildVerNumber = KUidMMCameraServerID;

class CMMCameraServerController;
class CMMCameraServerControllerQuery;
class CMMCameraServerShutdown;

/**
 * Standard server class derived from CServer2 to construct the server and manage connecting clients.
 * 
 * Also creates policy manager and camera controller to be used by client sessions.
 */
class CMMCameraServer : CServer2
	{
friend class CMMCameraServerSession;

public:
	static CMMCameraServer* NewLC(); //symbian doc says CServer2 should be returned ???

	~CMMCameraServer();

	void AddSession();
	void DropSession();

	inline CMMCameraServerPolicyManager* PolicyManager() {return iPolicyManager;};

protected:
	virtual TInt RunError(TInt aError);

private:
	CMMCameraServer();   // Named Constructor Idiom concept used

	void ConstructL();

	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	void PanicClient(const RMessage2& aMessage, TServerPanic aPanic);

	void GetCameraControllerL(TInt aCameraIndex, CMMCameraServerController*& aCameraController);
	void GetCameraControllerQueryL(CMMCameraServerControllerQuery*& aCameraControllerQuery);

private:
	TInt iCamSessionCount;

	TDblQue<CMMCameraServerController> iCamControllerQ;   //Double link list of CMMCameraServerController 
	CMMCameraServerPolicyManager* iPolicyManager;
	CMMCameraServerShutdown* iShutdown;
	};

#endif // MMCAMERASERVER_H
