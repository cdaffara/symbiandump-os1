/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __LEGACY_LOOPBACK_PARAMS_H__
#define __LEGACY_LOOPBACK_PARAMS_H__

#include <e32base.h>
#include <e32std.h>	

#include <comms-infras/metadata.h>
#include <es_prot.h>

class TLegacyLoopbackPref : public Meta::SMetaDataECom
	{
public:
	inline static TLegacyLoopbackPref* NewL();
	inline static TLegacyLoopbackPref* NewL(TInt aAP);
	
	inline TLegacyLoopbackPref();
	
	enum
	{
		EUid = 0x102871C6,
		ETypeId = 2
	};

	inline void setAP(TInt aAP);
	inline TInt AP();
		
	
protected:
	DATA_VTABLE
	
	TInt iAP;
	};

inline TLegacyLoopbackPref* TLegacyLoopbackPref::NewL()
	{
	Meta::STypeId input = STypeId::CreateSTypeId(TLegacyLoopbackPref::EUid,TLegacyLoopbackPref::ETypeId);
	return static_cast<TLegacyLoopbackPref*>(Meta::SMetaDataECom::NewInstanceL(input));
	}

inline TLegacyLoopbackPref* TLegacyLoopbackPref::NewL(TInt aAP)
	{
	TLegacyLoopbackPref* self = NewL();
	self->setAP(aAP);
	return self;
	}

inline TLegacyLoopbackPref::TLegacyLoopbackPref()
	{
	}

inline void TLegacyLoopbackPref::setAP(TInt aAP)
	{
	iAP = aAP;
	}

inline TInt TLegacyLoopbackPref::AP()
	{
	return iAP;
	}

class CLegacyLoopbackParamaterFactory : public CBase
/** Connection generic parameter set factory.

@internalComponent
*/
	{
public:
	static SMetaDataECom* NewL(TAny* aConstructionParameters);
	};



#endif // __LEGACY_LOOPBACK_PARAMS_H__