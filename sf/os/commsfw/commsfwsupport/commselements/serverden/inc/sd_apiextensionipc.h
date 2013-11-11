// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_DEN_APIEXTENSIONIPC_H
#define SYMBIAN_DEN_APIEXTENSIONIPC_H


namespace Den
/**
@internalTechnology
*/
{

//Must be common to all technologies using this ApiExt mechanism
//therefore a very big number to minimise the risk of a clash.
enum TMessIPC
	{
//TODO - perhaps we could reuse RMessage2 flags to differenciate session/subsession?
	EIpcSubSessionApiExtIfaceOpen = 2000000000,
	EIpcSubSessionApiExtIfaceSend,
	EIpcSubSessionApiExtIfaceSendReceive,
	EIpcSubSessionApiExtIfaceClose,

	EIpcSessionApiExtIfaceOpen,
	EIpcSessionApiExtIfaceSend,
	EIpcSessionApiExtIfaceSendReceive,
	EIpcSessionApiExtIfaceClose
	};

} //namespace Den

#endif
//SYMBIAN_DEN_APIEXTENSIONIPC_H

