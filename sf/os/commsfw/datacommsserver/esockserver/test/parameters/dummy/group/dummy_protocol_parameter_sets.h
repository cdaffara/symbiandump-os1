// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __DUMMY_PROTOCOL_PARAMETER_SETS_H__
#define __DUMMY_PROTOCOL_PARAMETER_SETS_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/metatypearray.h>
#include <in_sock.h>
#include <es_sock.h>
#include <comms-infras/ss_protocolparameterset.h>

const TInt32 KUidDummyParameterSetImpl = 0x10285D40;
const TInt32 KUidDummyProtocolParameterSetType = 0x10285D41;

using Meta::SMetaDataECom;


class CDummyProtocolParameterSet : public ESock::CSubConnectionProtocolParameterSet
/** Dummy protocol parameter set for
@internalComponent
*/
	{
public:
	inline static CDummyProtocolParameterSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CDummyProtocolParameterSet* NewL();
	void ConstructL();

protected:
	DATA_VTABLE

	TBool iStatus;
	};


class CDummyParamsFactory : public CBase
/** Factory for dummy protocol parameter sets
@internalComponent
*/
	{

public:
	static const TInt KFactoryUid = KUidDummyParameterSetImpl;
	inline static CSubConGenericParameterSet* NewL(TAny* aConstructionParameters);
	};


#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include "dummy_protocol_parameter_sets.inl"
#else
#include <test/dummy_protocol_parameter_sets.inl>
#endif

#endif	// __DUMMY_PROTOCOL_PARAMETER_SETS_H__

