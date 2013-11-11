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
// EtelPacketTest1_2.h
// This is the header file for Packet Context test 1.2 (RPacketContext functionality)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST1_2_H__)
#define __ETELPACKETTEST1_2_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest1_2 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest1_2();
	~CEtelPacketTest1_2();

	virtual enum TVerdict doTestStepL( void );
	
private:
	enum TVerdict ExtendedTest1(TRequestStatus& reqStatus, RPacketContext& gprsContext);
};

#endif //(__ETELPACKETTEST1_2_H__)

