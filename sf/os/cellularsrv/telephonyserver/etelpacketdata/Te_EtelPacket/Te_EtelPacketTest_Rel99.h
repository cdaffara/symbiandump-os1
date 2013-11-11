// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest_Rel99.h
// This is the header file for Packet Data Service testing Release 99 (RPacketService functionality testing)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST_REL99_H__)
#define __ETELPACKETTEST1_REL99_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest_Rel99 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest_Rel99();
	~CEtelPacketTest_Rel99();

	virtual enum TVerdict doTestStepL( void );
};

#endif //(__ETELPACKETTEST1_REL99_H__)
