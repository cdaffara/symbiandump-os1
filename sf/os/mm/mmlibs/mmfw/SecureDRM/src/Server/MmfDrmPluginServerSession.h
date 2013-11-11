// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFDRMPLUGINSERVERSESSION_H
#define MMFDRMPLUGINSERVERSESSION_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <caf/caf.h>
using namespace ContentAccess;

#include "MmfDrmPluginClientServer.h"


class CMMFDRMPluginServer;

class CMMFDRMPluginServerSession : public CMmfIpcSession
/**
*@internalTechnology
*/
	{
public:
	static CMMFDRMPluginServerSession* NewL();
	~CMMFDRMPluginServerSession();
	void CreateL(const CMmfIpcServer& aServer);
	void ServiceL(const RMmfIpcMessage& aMessage);
	
private:
	CMMFDRMPluginServerSession();
	
	void DoLaunchControllerL(const RMmfIpcMessage& aMessage);
	void DoPanicControllerThreadL(const RMmfIpcMessage& aMessage);
	void DoKillControllerThreadL(const RMmfIpcMessage& aMessage);
	void SetControllerServerInfo(RHandleBase& aControllerSessionHandle,
							TThreadId& aControllerThreadId);
	void DoSetThreadPriorityL(const RMmfIpcMessage& aMessage);

	void DoOpenContentByFilePathL(const RMmfIpcMessage& aMessage);
	void DoOpenContentByFileHandleL(const RMmfIpcMessage& aMessage);
	void GetContentInitDataL(const TDesC8& aInitData, HBufC*& aUniqueId,
							 TBool& aUIEnabled, ContentAccess::TIntent& aIntent);
	TInt DoEvaluateContentIntent(const RMmfIpcMessage& aMessage);
	void DoGetContentMimeTypeL(const RMmfIpcMessage& aMessage);
	void DoGetContentFileHeaderL(const RMmfIpcMessage& aMessage);
	void DoSetDrmPluginServerTimeout(const RMmfIpcMessage& aMessage);
	
private:
	RHandleBase iControllerSessionHandle;
	TThreadId iControllerThreadID;
	
	// Used by CMMFUtilityFileInfo
	ContentAccess::CData* iData;
	};

#endif
