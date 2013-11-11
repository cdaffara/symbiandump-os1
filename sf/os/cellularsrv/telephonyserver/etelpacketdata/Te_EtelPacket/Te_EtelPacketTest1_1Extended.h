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
// EtelPacketTest1_1Extended.h
// This is the header file for the Extended Packet Data Service test 1.1 
// (RPacketService functionality testing). Extra Packet service tests have been
// added to a new file (due to stack overflow in the EtelPacketTest1_1 files).
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST1_1EXTENDED_H__)
#define __ETELPACKETTEST1_1EXTENDED_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest1_1Extended : public CTestStepEtelPacket
{
public:
	CEtelPacketTest1_1Extended();
	~CEtelPacketTest1_1Extended();

	virtual enum TVerdict doTestStepL( void );

};

#endif //(__ETELPACKETTEST1_1EXTENDED_H__)
