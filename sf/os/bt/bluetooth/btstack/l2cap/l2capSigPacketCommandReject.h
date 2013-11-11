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

#ifndef L2CAPSIGPACKETCOMMANDREJECT_H
#define L2CAPSIGPACKETCOMMANDREJECT_H

#include "l2capCommand.h"

NONSHARABLE_CLASS(HCommandReject) : public HL2CapCommand
	{
public:
	HCommandReject();
	static HCommandReject* New(TL2CAPCommandRejectData aRejectData, TUint8 aId);
	static TInt NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand);
	~HCommandReject();

	TBool ProcessCommand(CL2CapSignalHandler& aSignalHandler);

	TInt RejectData(TL2CAPCommandRejectData& aRejectData) const;
	void SetRejectData(TL2CAPCommandRejectData aRejectData);

private:
	HCommandReject(RMBufChain& aCommand);
	static TBool VerifyStructure(const RMBufChain& aBuffer);

private:
	const static TUint8 KReasonOffset				= HL2CapCommand::KL2CapCommandHeaderLength;
	const static TUint8 KDataOffset					= 6;

	const static TUint8 KMTUExceededLength 			= 2;
	const static TUint8 KMTUExceededOffset			= 6;
	
	const static TUint8 KInvalidRequestedCIDLength	= 4;
	const static TUint8 KLocalEndpointOffset 		= 6;
	const static TUint8 KRemoteEndpointOffset 		= 8;
	
	const static TUint8 KL2CapCommandRejectMinLength = HL2CapCommand::KL2CapCommandHeaderLength + 2;
	const static TUint8 KL2CapCommandRejectMaxLength = KL2CapCommandRejectMinLength + KInvalidRequestedCIDLength;
	};

#endif
