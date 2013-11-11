/*
 * Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:   Class definition of server plugin wrapper implementing S60 custom interface.
 *
 */

#include "ciextnserverpluginwrapper.h"
#include "citraces.h"
#include <AudioInputMessageTypes.h>
#include <ecom.h>

#define KMuxTempBufferSize 20

#ifdef __WINSCW__
class MyMCustomInterface
    {
public:
    virtual TAny* CustomInterface(TUid aCustomeInterface) = 0;
    virtual void Release() = 0;
    virtual void PassDestructionKey(TUid aUid) = 0;
    };
#endif

CIExtnServerPluginWrapper* CIExtnServerPluginWrapper::NewL(
        MCustomInterface& aInterfaceRef)
    {
    DEB_TRACE0(_L("CIExtnServerPluginWrapper::NewL Begin"));
    CIExtnServerPluginWrapper* self = new (ELeave) CIExtnServerPluginWrapper(
            aInterfaceRef);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    DEB_TRACE0(_L("CIExtnServerPluginWrapper::NewL End"));
    return self;
    }

CIExtnServerPluginWrapper::~CIExtnServerPluginWrapper()
    {
    DEB_TRACE0(_L("CIExtnServerPluginWrappers::~CIExtnServerPluginWrapper Begin"));
    DEB_TRACE0(_L("CIExtnServerPluginWrapper::~CIExtnServerPluginWrapper End"));
    }

CIExtnServerPluginWrapper::CIExtnServerPluginWrapper(
        MCustomInterface& aInterfaceRef) :
    iCustomInterfaceRef(&aInterfaceRef)
    {
    }

void CIExtnServerPluginWrapper::ConstructL()
    {
#ifdef __WINSCW__
    DEB_TRACE0(_L("Try Loading CI Stub Plugin"));
    TUid interfaceUid =
        {
        0x10207C71
        };
    TAny* pluginRef;
    TRAPD(error, pluginRef = REComSession::CreateImplementationL( interfaceUid, iDtor_ID_Key ));
    if (pluginRef)
        {
        DEB_TRACE1(_L("CreateImplementationL Uid [0x%x]"), iDtor_ID_Key.iUid);
        iCustomInterfaceStub = (MyMCustomInterface*) pluginRef;
        iCustomInterfaceStub->PassDestructionKey(iDtor_ID_Key);
        DEB_TRACE0(_L("CI Stub Loaded"));
        }
#endif    

    }

MCustomInterface* CIExtnServerPluginWrapper::GetInterface()
    {
    DEB_TRACE0(_L("CIExtnServerPluginWrappers::GetInterface"));
    return (MCustomInterface*) this;
    }

TAny* CIExtnServerPluginWrapper::CustomInterface(TUid aInterfaceId)
    {
    DEB_TRACE0(_L("*CI* CIExtnServerPluginWrapper::CustomInterface"));
    TAny* pluginRef = NULL;
    pluginRef = iCustomInterfaceRef->CustomInterface(aInterfaceId);
#ifdef __WINSCW__
    if (!pluginRef)
        {
        DEB_TRACE0(_L("*CI* Try Loading CI Stub if present"));
        pluginRef = iCustomInterfaceStub->CustomInterface(aInterfaceId);
        if (pluginRef)
            {
            DEB_TRACE0(_L("*CI* CI Stub Loaded"));
            }
        }
#endif
    return pluginRef;
    }

void CIExtnServerPluginWrapper::Release()
    {
    DEB_TRACE0(_L("CIExtnServerPluginWrappers::Release >> ENTER"));
#ifdef __WINSCW__    
    iCustomInterfaceStub->Release();
#endif    
    DEB_TRACE0(_L("CIExtnServerPluginWrappers::Release >> EXIT"));
    delete this;
    }

/*
 TAny* CIExtnServerPluginWrapper::CustomInterface(TUid aInterfaceId)
 {
 DEB_TRACE0(_L("CIExtnServerPluginWrappers::CustomInterface"));
 TUid interfaceUid =
 {
 0x10207C49
 };
 TEComResolverParams resolverParams;
 DEB_TRACE1(_L("Findig CustomInterface with Uid [0x%x]"),aInterfaceId.iUid);
 TInt uidAsInteger = aInterfaceId.iUid;
 TBuf8<KMuxTempBufferSize> tempBuffer;
 tempBuffer.Num(uidAsInteger, EHex);
 resolverParams.SetDataType(tempBuffer);

 TAny* ptr = NULL;
 TRAPD(err, ptr=REComSession::CreateImplementationL(
 interfaceUid,
 _FOFF(CIExtnServerPluginWrapper, iDtor_ID_Key),resolverParams));
 if (err != KErrNone)
 {
 DEB_TRACE1(_L("No Custom Interface found:  Error [%d]"),err);
 }
 if (ptr)
 {
 DEB_TRACE1(_L("Stub Present for Uid [0x%x]"),interfaceUid);
 return ptr;
 }
 else
 {
 return iCustomInterfaceRef->CustomInterface(aInterfaceId);
 }
 }
 */
