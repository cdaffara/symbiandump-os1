// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include <ecom/implementationproxy.h>
#include <comms-infras/mobilitymessages.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/mobilitymessages_internal.h>
#endif
#include <comms-infras/mobilitymessagesecom.h>
#include <comms-infras/datamonitormessages.h>
#include <comms-infras/datamonmessagesecom.h>
#include <comms-infras/esockmessages.h>

using namespace ESock;

/**
ECOM Implementation Factories
* @internalTechnology
*/

namespace ESock
{

/**
ESock message factory.

@internalComponent
@released since v9.2
*/
class CESockMsgFactory : public CBase
    {
public:
    static Meta::SMetaDataECom* NewMessageL(TAny* aParams);
    };

Meta::SMetaDataECom* CESockMsgFactory::NewMessageL(TAny* aParams)
    {
    TInt32 type = reinterpret_cast<TInt32>(aParams);
    switch (type)
        {
    default:
        User::Leave(KErrNotFound);
        }
    return NULL;
    }

}	// namespace ESock

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KESockMessagesImplementationUid, CESockMsgFactory::NewMessageL),
    IMPLEMENTATION_PROXY_ENTRY(KMobilityMessagesImplementationUid, CMobilityMsgFactory::NewMessageL),
    IMPLEMENTATION_PROXY_ENTRY(KDataMonitoringMessagesImplementationUid, CDataMonitoringMsgFactory::NewMessageL),
    IMPLEMENTATION_PROXY_ENTRY(KDataMonitoringRequestMessagesImplementationUid, CDataMonitoringMsgPluginInfo::NewL),
    IMPLEMENTATION_PROXY_ENTRY(KMobilityRequestMessagesImplementationUid, CMobilityMsgPluginInfo::NewL),
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }




