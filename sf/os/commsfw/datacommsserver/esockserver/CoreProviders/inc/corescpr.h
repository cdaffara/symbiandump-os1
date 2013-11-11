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
// Core SCPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORESCPR_H
#define SYMBIAN_CORESCPR_H

#include <comms-infras/ss_subconnprov.h>

namespace SCprStates
    {
    class TStartYourFlow;
    class TStoreParams;
    class TRespondWithCurrentParams;
	}

namespace ESock
    {
	class SAccessPointConfig;
	}
class CCoreSubConnectionProvider : public ESock::CSubConnectionProviderBase
/**

@internalComponent
@released since v9.4
 */
    {
	friend class SCprStates::TStartYourFlow;
	friend class SCprStates::TStoreParams;
	friend class SCprStates::TRespondWithCurrentParams;
	friend class CCoreSubConnectionProviderFactory;

public:
	IMPORT_C virtual ~CCoreSubConnectionProvider();

protected:
    IMPORT_C CCoreSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory,
                                        const MeshMachine::TNodeActivityMap& aActivityMap);

    IMPORT_C CCoreSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
    IMPORT_C static CCoreSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);


    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);
    IMPORT_C void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    };

#endif
//SYMBIAN_CORESCPR_H

