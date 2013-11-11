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
// EtelPacketTest1_1.h
// This is the header file for Packet Data Service test 1.1 (RPacketService functionality testing)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST1_1_H__)
#define __ETELPACKETTEST1_1_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest1_1 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest1_1();
	~CEtelPacketTest1_1();
	
	virtual enum TVerdict doTestStepL( void );

private:
	enum TVerdict ExtendedTest1(RPhone& phone, 	RPacketService& gprs, TRequestStatus& reqStatus);

};

#endif  // (__ETELPACKETTEST1_1_H__)
