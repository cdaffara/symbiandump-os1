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
// mmcamerapolicymanager.h
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef MMCAMERASERVERPOLICYMANAGER_H
#define MMCAMERASERVERPOLICYMANAGER_H

#include <e32std.h>
#include <e32hashtab.h>
#include <e32cmn.h>

class CMMCameraServerSession;

typedef TDblQue<CMMCameraServerSession> TCamSessionQue;
typedef TDblQueIter<CMMCameraServerSession> TCamSessionIter;


// ECAM Server Policies
static _LIT_SECURITY_POLICY_C1 (KMMCameraServerPolicyUserEnvironment, ECapabilityUserEnvironment);
static _LIT_SECURITY_POLICY_C1 (KMMCameraServerPolicyMultimediaDD, ECapabilityMultimediaDD);

/**
 * Manages policy for the ECAM server.
 * 
 * Handles both Platsec capability checks and Reserve/Release of camera devices.
 */  
class CMMCameraServerPolicyManager : public CBase
	{
public:
	static CMMCameraServerPolicyManager* NewL();

	~CMMCameraServerPolicyManager();

	void ReleaseClient(CMMCameraServerSession* aSession);
	TBool ReserveClient(CMMCameraServerSession* aSession);

	void OnConnectL (const RMessage2& aMessage);
	void ServiceHandlerL(const RMessage2& aMessage);

private:
	CMMCameraServerPolicyManager();   // Named Constructor Idiom concept used

	void ConstructL();
	CMMCameraServerSession* FindFirstInQue(TInt aIndex);
	CMMCameraServerSession* FindLastInQue(TInt aIndex);
	void ReserveUpdate(CMMCameraServerSession* aSession);
	TBool CheckControlOvertaking(CMMCameraServerSession* aSession); // informs whether the new client can overtake camera control from the previously reserved ones 

private:
	TCamSessionQue iReservedSessionQ;
	TCamSessionIter iIter;

	TInt iCameraIndex;
    TInt iHandle;
    TInt iPriority;
	TBool iMMCapability;   
	};

#endif // MMCAMERASERVERPOLICYMANAGER_H
