// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Core CPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORECPR_H
#define SYMBIAN_CORECPR_H

#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_commsdataobject.h>
#include <comms-infras/provinfoqueryset.h>

class CCoreConnectionProvider : public ESock::CConnectionProviderBase
/**

@internalComponent
@released since v9.4
 */
    {
	friend class CCoreConnectionProviderFactory;

public:

public:
	IMPORT_C virtual ~CCoreConnectionProvider();
	TUint Priority() const;

protected:
    IMPORT_C static CCoreConnectionProvider* NewL(ESock::CConnectionProviderFactoryBase& aFactory);

    IMPORT_C CCoreConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory,
                                     const MeshMachine::TNodeActivityMap& aActivityMap);

    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);
    IMPORT_C void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    };

#endif
//SYMBIAN_CORECPR_H

