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

#if !defined(__ES_IPC_H__)
#define __ES_IPC_H__

/**
@internalComponent
*/
const TUint KIPCAddrFamily=0x666;
const TUint KIPCStreamProtocol=0x01;
const TUint KIPCDatagramProtocol=0x02;

/**
@internalComponent
*/
const TInt KIPSNumberSockets=0x100;

/**
@internalComponent
*/
const TUint KIPCStreamServiceInfo = KSIReliable | KSIInOrder | KSIStreamBased | KSIUrgentData | KSIGracefulClose;
const TUint KIPCDatagramServiceInfo = KSIConnectionLess | KSIMessageBased ;

/**
@internalComponent
*/
const TUint KIPCStreamDefaultBufferSize=2048;

#endif
