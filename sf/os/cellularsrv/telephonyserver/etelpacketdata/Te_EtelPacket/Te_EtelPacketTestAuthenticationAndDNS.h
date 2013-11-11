// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTestAuthenticationAndDNS.h
// This is the header file for Packet Data Context test Authentication and DNS Server names
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTESTAUTHENTICATIONANDDNS_H__)
#define __ETELPACKETTESTAUTHENTICATIONANDDNS_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTestAuthenticationAndDNS : public CTestStepEtelPacket
{
public:
	CEtelPacketTestAuthenticationAndDNS();
	~CEtelPacketTestAuthenticationAndDNS();

	virtual enum TVerdict doTestStepL( void );
};

//
#endif //(__ETELPACKETTESTAUTHENTICATIONANDDNS_1_H__)
