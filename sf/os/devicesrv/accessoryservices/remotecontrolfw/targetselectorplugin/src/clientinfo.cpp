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

/**
 @file
 @internalComponent
*/

#include <remcon/clientinfo.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

EXPORT_C TClientInfo::TClientInfo()
	{
	LOG_FUNC
	}

EXPORT_C TClientInfo::~TClientInfo()
	{
	LOG_FUNC
	}

EXPORT_C TProcessId& TClientInfo::ProcessId()
	{
	return iProcessId;
	}

EXPORT_C TProcessId TClientInfo::ProcessId() const
	{
	return iProcessId;
	}

EXPORT_C RMessage2& TClientInfo::Message()
	{
	return iMessage;
	}

EXPORT_C const RMessage2& TClientInfo::Message() const
	{
	return iMessage;
	}

EXPORT_C TSecureId& TClientInfo::SecureId()
	{
	return iSecureId;
	}

EXPORT_C TSecureId TClientInfo::SecureId() const
	{
	return iSecureId;
	}
