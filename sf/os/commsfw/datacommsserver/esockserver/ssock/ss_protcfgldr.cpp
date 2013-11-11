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
 @internalTechnology
*/
#include "ss_protcfgldr.h"

#include <commsdattypesv1_1.h> // CommsDat
#include <ecom/ecom.h>


using namespace ESock;
using namespace CommsDat;

EXPORT_C CProtocolConfigLoader* CProtocolConfigLoader::NewL(CommsDat::CMDBSession* aDbs, const TUid aProtCfgLdrUid)
    {
    CProtocolConfigLoader *loader = CProtocolConfigLoader::NewL(aProtCfgLdrUid);
    loader->SetDatabaseSession(aDbs);

    return loader;
    }

EXPORT_C CProtocolConfigLoader* CProtocolConfigLoader::NewL(const TUid aProtCfgLdrUid)
    {
    TAny* protoCfgLdr =
	REComSession::CreateImplementationL(aProtCfgLdrUid, _FOFF(CProtocolConfigLoader, iDtor_ID_Key));

    return reinterpret_cast<CProtocolConfigLoader*>(protoCfgLdr);
    }

EXPORT_C CProtocolConfigLoader::~CProtocolConfigLoader()
    {
    // notify ECom of the deletion of the object
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

EXPORT_C void CProtocolConfigLoader::ListImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    REComSession::ListImplementationsL(KCProtocolConfigLoaderUid, aImplInfoArray);
    }


