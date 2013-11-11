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

/**
 @file
 @internalComponent
*/


#ifndef __CBNEPPKTDRVFACTORY_H
#define __CBNEPPKTDRVFACTORY_H 

#include <e32base.h>
#include <e32std.h>
#include <es_mbuf.h>
#include <comms-infras/nifif.h>
#include <networking/ethinter.h>

const TUint KBnepMajorVersionNumber=1;
const TUint KBnepMinorVersionNumber=0;
const TUint KBnepBuildVersionNumber=7;
IMPORT_C CPktDrvFactory* NewBnepPktDrvFactoryL();


NONSHARABLE_CLASS(CBnepPktDrvFactory) : public CPktDrvFactory  
    {
public:
    CBnepPktDrvFactory();
    ~CBnepPktDrvFactory();
    void InitL (RLibrary& aLib, CObjectCon& aCon);
    CPktDrvBase* NewDriverL (CLANLinkCommon* aParent);
    TInt Open ();
    TVersion Version () const;
    };

#endif
