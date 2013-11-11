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

#ifndef __MMFBTAUDIOSERVERSESSION_H__
#define __MMFBTAUDIOSERVERSESSION_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfBtAudioClientServer.h"


class CMMFAudioServer;
class CMMFDevSoundServer;

class CMMFAudioServerSession : public CMmfIpcSession
/**
*@internalTechnology
*/
	{
public:
	static CMMFAudioServerSession* NewL(TName& aDevSoundServerName);
	~CMMFAudioServerSession();
	void CreateL(const CMmfIpcServer& aServer);
	void ServiceL(const RMmfIpcMessage& aMessage);
	TInt AudioServerSessionId(void) {return iAudioServerSessionId;};
	void SendEventToClient(/*TMMFAudioServerEvent& aEvent*/);

private:
	CMMFAudioServerSession(TName& aDevSoundServerName);
	void ConstructL();
	TInt DoGetDevSoundServerNameLengthL(const RMmfIpcMessage& aMessage);
	TInt DoGetDevSoundServerNameL(const RMmfIpcMessage& aMessage);
private:
	TInt iAudioServerSessionId;
	TName iDevSoundServerName;
	CBufFlat* iDevSoundServerNameFlat;
	};

#endif
