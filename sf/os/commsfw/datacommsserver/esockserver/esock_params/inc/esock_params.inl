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

#ifndef ESOCK_PARAMS_INL
#define ESOCK_PARAMS_INL

#include <comms-infras/esock_params.h>
#include <comms-infras/metadata.h>

inline TConnAPPref* TConnAPPref::NewL()
	{
	Meta::STypeId input = STypeId::CreateSTypeId(TConnAPPref::EUid,TConnAPPref::ETypeId);
	return static_cast<TConnAPPref*>(Meta::SMetaDataECom::NewInstanceL(input));
	}

inline TConnAPPref* TConnAPPref::NewL(TUint aAP)
	{
	TConnAPPref* self = NewL();
	self->SetAP(aAP);
	return self;
	}

inline void TConnAPPref::SetAP(TUint aAP)
	{
	iAP = aAP;
	}
	
inline TInt TConnAPPref::GetAP()
	{
	return iAP;
	}

inline TConnAutoStartPref* TConnAutoStartPref::NewL()
	{
	Meta::STypeId input = STypeId::CreateSTypeId(TConnAutoStartPref::EUid,TConnAutoStartPref::ETypeId);
	return static_cast<TConnAutoStartPref*>(Meta::SMetaDataECom::NewInstanceL(input));
	}


#endif //ESOCK_PARAMS_INL
