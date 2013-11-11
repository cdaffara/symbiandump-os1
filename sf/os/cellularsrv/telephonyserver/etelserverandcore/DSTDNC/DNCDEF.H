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

/**@file
   @internalAll */
#ifndef __DSTDNC_DEF_H
#define __DSTDNC_DEF_H
//
//	Definitions that are Common to Dstd and Tetel
//
/**
@internalComponent
*/
#define DSTDNC_CALL_FULLNAME				_L("DummyNc::PotsNc::HarryNc")
#define DSTDNC_OPEN_CALL_FROM_PHONE_NAME	_L("PotsNc::HarryNc")
#define	DSTDNC_MODULE_NAME					_L("DSTDNC")
#define	DSTDNC_PHONE_NAME					_L("DummyNc")
#define	DSTDNC_LINE_NAME					_L("PotsNc")
#define	DSTDNC_CALL_NAME					_L("HarryNc")
#define	DSTDNC_FAX_NAME						_L("FaxNc")

/**
@internalComponent
*/
#define	DSTDNC_NETWORK_TYPE		RTelServer::ENetworkTypeWiredAnalog
#define	DSTDNC_NUMBER_OF_LINES	(1)
#define DSTDNC_NUMBER_OF_PHONES (1)

/**
@internalComponent
*/
#define DSTDNC_LINE_STATUS	RCall::EStatusIdle
#define DSTDNC_CALL_STATUS	RCall::EStatusIdle
#define DSTDNC_CALL_CAPS	RCall::KCapsFax

#endif
