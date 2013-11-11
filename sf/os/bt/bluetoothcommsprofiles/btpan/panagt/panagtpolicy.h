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
// Policy on requiring encryption/authorisation for connections
// 
//

/**
 @file
 @note "Policy" type constants for PAN agent
 @internalComponent
*/

#ifndef PANAGTPOLICY_H
#define PANAGTPOLICY_H

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{
	
// for incoming connections
const TBool KPanIncomingEncryptionRequired = ETrue;
const TBool KPanIncomingAuthorisationRequired = ETrue;
const TBool KPanIncomingAuthenticationRequired = ETrue; // note: needs to be considered in light of SSP.

// for outgoing connections
const TBool KPanOutgoingEncryptionRequired = ETrue;
const TBool KPanOutgoingAuthorisationRequired = EFalse;
const TBool KPanOutgoignAuthenticationRequired = ETrue; // note: needs to be considered in light of SSP.

} // PanAgent

#endif // PANAGTPOLICY_H
