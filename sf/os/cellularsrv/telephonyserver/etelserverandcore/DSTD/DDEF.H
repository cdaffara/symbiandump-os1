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
#ifndef __DSTD_DEF_H
#define __DSTD_DEF_H

/**
DSTD Phone
@internalComponent
*/
#define	DSTD_MODULE_NAME				_L("DSTD")
#define	DSTD_MODULE_NAME2				_L("DSTD.TSY")
#define DSTD_OPEN_CALL_FROM_PHONE_NAME	_L("Pots::Harry")
#define DSTD_PHONE_NAME					_L("Dummy")
#define	DSTD_LINE_NAME					_L("Pots")
#define DSTD_CALL_NAME					_L("Harry")
#define DSTD_FAX_NAME					_L("Fax")
#define DSTD_SLOW_PHONE_NAME			_L("Slow_Phone")

#define DSTD_CALL_FULLNAME				_L("Dummy::Pots::")
#define DSTD_LINE_FULLNAME				_L("Dummy::Pots")
#define DSTDPM_LINE_FULLNAME			_L("DummyPm::PotsPm")

#define DSTD_NETWORK_TYPE	(RTelServer::ENetworkTypeWiredAnalog)
#define DSTDFC_NETWORK_TYPE (RTelServer::ENetworkTypeWiredAnalog)
#define DSTDPM_NETWORK_TYPE (RTelServer::ENetworkTypeMobileDigital)
#define	DSTD_NUMBER_OF_LINES	(1)
#define	DSTDFC_NUMBER_OF_LINES	(1)
#define	DSTDPM_NUMBER_OF_LINES	(1)

#define DSTDFC_OPEN_CALL_FROM_PHONE_NAME	_L("PotsFC::HarryFC")
#define DSTDFC_PHONE_NAME					_L("DummyFC")
#define	DSTDFC_LINE_NAME					_L("PotsFC")
#define DSTDFC_CALL_NAME					_L("HarryFC")
#define DSTDFC_FAX_NAME						_L("FaxFC")
#define DSTDFC_CALL_FULLNAME				_L("DummyFC::PotsFC::HarryFC")

#define DSTDPM_OPEN_CALL_FROM_PHONE_NAME	_L("PotsPm::HarryPm")
#define DSTDPM_PHONE_NAME					_L("DummyPm")
#define	DSTDPM_LINE_NAME					_L("PotsPm")
#define DSTDPM_CALL_NAME					_L("HarryPm")
#define DSTDPM_FAX_NAME						_L("FaxPm")
#define DSTDPM_CALL_FULLNAME				_L("DummyPm::PotsPm::HarryPm")

/**
here change to narrow-unicode
@internalComponent
*/
#define DSTD_NARROW								_L8("Hello")
#define DSTD_UNICODE							_L16("Hello")

#define DSTDFC_CALL_STATUS	RCall::EStatusConnected
#define DSTDPM_CALL_STATUS	RCall::EStatusConnecting

#define DSTDFC_LINE_STATUS	RCall::EStatusConnected
#define DSTDPM_LINE_STATUS	RCall::EStatusUnknown

#define	DSTD_CALL_INDEX		(0)
#define	DSTDFC_CALL_INDEX	(0)
#define	DSTDPM_CALL_INDEX	(0)

#define DSTD_NUMBER_OF_PHONES (4)
#define DSTDPM_CALL_FULL_NONAME _L("DummyPm::PotsPm::")
#define DSTD_CALL_STATUS	RCall::EStatusIdle
#define DSTD_LINE_STATUS	RCall::EStatusIdle
#define DSTD_PHONE_STATUS	RPhone::EPhoneStatusPowerOn

#define	DSTDFC_OPEN_PHONE_TIMEOUT 100000

#define DSTATICCALL_PHONE_NAME				_L("DummySC")
#define	DSTATICCALL_LINE_NAME				_L("VoiceLine")

#endif
