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

#ifndef MMFDRMPLUGINCLIENTSERVER_H
#define MMFDRMPLUGINCLIENTSERVER_H

// opcodes used in message passing between client and server
enum TMMFDRMPluginServerRequest
	{
	EMMFControllerLaunchRequest,
	EMMFControllerSessionHandle,
	EMMFControllerThreadPanic,
	EMMFControllerThreadKill,
	EMMFControllerSetThreadPriority,
	EMMFDRMContentOpenByFilePath,
	EMMFDRMContentOpenByFileHandle,
	EMMFDRMContentEvaluateIntent,
	EMMFDRMContentGetMimeType,
	EMMFDRMContentGetFileHeader,
	EMMFSetDrmPluginServerTimeout
	};

#endif
