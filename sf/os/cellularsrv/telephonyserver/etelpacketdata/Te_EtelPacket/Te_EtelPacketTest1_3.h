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
// EtelPacketTest1_3.h
// This is the header file for Packet Data QoS test 1.3(RPacketQoS functionality)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST1_3_H__)
#define __ETELPACKETTEST1_3_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest1_3 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest1_3();
	~CEtelPacketTest1_3();

	virtual enum TVerdict doTestStepL( void );

};

#endif //(__ETELPACKETTEST1_3_H__)
