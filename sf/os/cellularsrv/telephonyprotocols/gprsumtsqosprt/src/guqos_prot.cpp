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

#include "guqos.h"



class CExtensionModule : public CProtocolFamilyBase
	{
public:
	CExtensionModule();
	virtual ~CExtensionModule();
	virtual TInt Install();
	virtual CProtocolBase* NewProtocolL(TUint aSockType,TUint aProtocol);
	virtual TUint ProtocolList(TServerProtocolDesc*& aProtocolList);
	};



// DLL Install
GLDEF_C TInt E32Dll()
	{
	return(KErrNone);
	}



// Force export of non-mangled name
extern "C" { IMPORT_C CProtocolFamilyBase *Install(); }
EXPORT_C CProtocolFamilyBase *Install()
	{
	return new CExtensionModule;
	}



// CExtensionModule
CExtensionModule::CExtensionModule()
	{
	}


CExtensionModule::~CExtensionModule()
	{
	}


TInt CExtensionModule::Install()
	{
	return KErrNone;
	}


TUint CExtensionModule::ProtocolList(TServerProtocolDesc*& aProtocolList)
	{
	aProtocolList = new TServerProtocolDesc[1];
	if (aProtocolList == NULL)
		return 0;

	CModuleGuqos::Identify(aProtocolList[0]);
	return 1;
	}

CProtocolBase* CExtensionModule::NewProtocolL(TUint aSockType,TUint aProtocol)
	{
	switch (aProtocol)
		{
	case KModuleGUQoS:
		if (aSockType == KSockDatagram)
			return CModuleGuqos::NewL();
		break;
	default:
		break;
		}
	User::Leave(KErrNotSupported);
	return NULL;	//not reached really (shut off a warning)
	}
