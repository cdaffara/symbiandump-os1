// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFAUDIOSERVERSESSION_H__
#define __MMFAUDIOSERVERSESSION_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfAudioClientServer.h"


class CMMFAudioServer;
class CMMFDevSoundServer;

class CMMFAudioServerSession : public CMmfIpcSession
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioServerSession* NewL(RHandleBase& aDevSoundSessionHandle);
	~CMMFAudioServerSession();
	void CreateL(const CMmfIpcServer& aServer);
	void ServiceL(const RMmfIpcMessage& aMessage);
	TInt AudioServerSessionId(void) {return iAudioServerSessionId;};
	void SendEventToClient(/*TMMFAudioServerEvent& aEvent*/);

private:
	CMMFAudioServerSession();
	void ConstructL();
	void SetDevSoundSessionHandle(RHandleBase& aDevSoundSessionHandle);
private:
	TInt iAudioServerSessionId;
	RHandleBase iDevSoundSessionHandle;
	};

#endif
