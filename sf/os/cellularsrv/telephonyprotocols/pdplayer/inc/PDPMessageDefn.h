// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// It contains all the internal messages used by PDP component.
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDP_MESSAGE_DEFN_H
#define SYMBIAN_PDP_MESSAGE_DEFN_H

class TPDPMessages
    {
public:
    enum { ERealmId = 0x102822EF };

private:
    enum
    /**
    Definition of generic Link Tier message ids
    */
        {
        EPDPFSMMessage = Messages::KNullMessageId + 1,
        EAuthenticateComplete = Messages::KNullMessageId + 2,
        EPDPContentionResultMessage = Messages::KNullMessageId + 3
        };
public:
    typedef Messages::TMessageSigNumberNumber<EPDPFSMMessage, TPDPMessages::ERealmId> TPDPFSMMessage;
    typedef Messages::TMessageSigNumber<EAuthenticateComplete, TPDPMessages::ERealmId> TAuthenticateComplete;
    typedef Messages::TMessageSigNodeIdNumber<EPDPContentionResultMessage, TPDPMessages::ERealmId> TPdpContentionResultMessage;
    };

#endif
