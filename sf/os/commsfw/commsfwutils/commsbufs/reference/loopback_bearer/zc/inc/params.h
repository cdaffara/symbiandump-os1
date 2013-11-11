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
#ifndef __ZEROCOPY_LOOPBACK_PARAMS_H__
#define __ZEROCOPY_LOOPBACK_PARAMS_H__

#include <e32base.h>
#include <e32std.h>	

#include <comms-infras/metadata.h>
#include <es_prot.h>

class TZeroCopyLoopbackPref : public Meta::SMetaDataECom
	{
public:
	inline static TZeroCopyLoopbackPref* NewL();
	inline static TZeroCopyLoopbackPref* NewL(TInt aAP);
	
	inline TZeroCopyLoopbackPref();
	
	enum
	{
		EUid = 0x10286A10,
		ETypeId = 2
	};

	inline void setAP(TInt aAP);
	inline TInt AP();
		
	
protected:
	DATA_VTABLE
	
	TInt iAP;
	};

inline TZeroCopyLoopbackPref* TZeroCopyLoopbackPref::NewL()
	{
	Meta::STypeId input = STypeId::CreateSTypeId(TZeroCopyLoopbackPref::EUid,TZeroCopyLoopbackPref::ETypeId);
	return static_cast<TZeroCopyLoopbackPref*>(Meta::SMetaDataECom::NewInstanceL(input));
	}

inline TZeroCopyLoopbackPref* TZeroCopyLoopbackPref::NewL(TInt aAP)
	{
	TZeroCopyLoopbackPref* self = NewL();
	self->setAP(aAP);
	return self;
	}

inline TZeroCopyLoopbackPref::TZeroCopyLoopbackPref()
	{
	}

inline void TZeroCopyLoopbackPref::setAP(TInt aAP)
	{
	iAP = aAP;
	}

inline TInt TZeroCopyLoopbackPref::AP()
	{
	return iAP;
	}

class CZeroCopyLoopbackParamaterFactory : public CBase
/** Connection generic parameter set factory.

@internalComponent
*/
	{
public:
	static SMetaDataECom* NewL(TAny* aConstructionParameters);
	};



#endif // __ZEROCOPY_LOOPBACK_PARAMS_H__