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

#ifndef ESOCK_PARAMS_INTERAL_INL
#define ESOCK_PARAMS_INTERAL_INL

#include <comms-infras/esock_params_internal.h>
#include <comms-infras/metadata.h>

inline TConnCSRPref* TConnCSRPref::NewL()
	{
	Meta::STypeId input = STypeId::CreateSTypeId(TConnCSRPref::EUid,TConnCSRPref::ETypeId);
	return static_cast<TConnCSRPref*>(Meta::SMetaDataECom::NewInstanceL(input));
	}

inline TUint TConnCSRPref::Scope() const
    {
    return iScope;
    }

inline void TConnCSRPref::SetScope(TUint aScope)
    {
    iScope = aScope;
    }

inline TUint TConnCSRPref::Flags() const
    {
    return iFlags;
    }

inline void TConnCSRPref::SetFlags(TUint aFlags)
    {
    iFlags = aFlags;
    }

inline Den::TSubSessionUniqueId TConnCSRPref::SubSessionUniqueId() const
	{
	return iSubSessionUniqueId;
	}

inline void TConnCSRPref::SetSubSessionUniqueId(Den::TSubSessionUniqueId aSubSessionUniqueId)
	{
	iSubSessionUniqueId = aSubSessionUniqueId;
	}

#endif //ESOCK_PARAMS_INTERAL_INL

