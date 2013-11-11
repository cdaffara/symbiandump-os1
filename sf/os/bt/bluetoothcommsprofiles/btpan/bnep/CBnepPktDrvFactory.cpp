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

#include <bluetooth/logger.h>
#include "CBnepPktDrvFactory.h"
#include "CBnepLocalDevice.h"
#include "bneputils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Create the packet driver factory.
   Uses logging, so will also instantiate TLS in logging builds.
*/
EXPORT_C CPktDrvFactory* NewBnepPktDrvFactoryL()
    {
    CPktDrvFactory* f = new (ELeave) CBnepPktDrvFactory;
    return f;
    }

CBnepPktDrvFactory::CBnepPktDrvFactory()
    {
    CONNECT_LOGGER
    LOG_FUNC
    }

/**
   Delete any TLS allocated.
   @internalComponent
*/
CBnepPktDrvFactory::~CBnepPktDrvFactory()
    {
    CLOSE_LOGGER
    }

/**
   Second stage constructor.
   @param aLib Reference to the packet driver library.  Takes ownership.
   @param aCon The CObject container to hold this object.
   @internalComponent
*/
void CBnepPktDrvFactory::InitL (RLibrary& aLib, CObjectCon& aCon)
    {
    LOG_FUNC
    iLib.SetHandle(aLib.Handle());
    aLib.SetHandle(0); // Handle transferred
    TPtrC name = _L("BNEP");
    SetNameL(&name);
    aCon.AddL(this); 
    }

/**
   Create and return a new packet driver object
   @param aParent The bottom of the ethernet nif.
   @internalComponent
*/
CPktDrvBase* CBnepPktDrvFactory::NewDriverL (CLANLinkCommon* aParent)
    {
    LOG_FUNC
    return CBnepLocalDevice::NewL(*this, aParent);
    }

/**
   Increment the object access count.
   @internalComponent
*/
TInt CBnepPktDrvFactory::Open ()
    {
    LOG_FUNC
    Inc();
    LOG1(_L8("CBnepPktDrvFactory: Open count=%d"), AccessCount());
    return KErrNone; 
    }

/**
   Return version of packet driver
   @internalComponent
*/
TVersion CBnepPktDrvFactory::Version () const
    {
    LOG_FUNC
    return (TVersion(KBnepMajorVersionNumber, KBnepMinorVersionNumber, KBnepBuildVersionNumber)); 
    }

