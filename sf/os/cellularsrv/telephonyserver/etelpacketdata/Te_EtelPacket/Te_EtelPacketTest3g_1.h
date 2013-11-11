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
// EtelPacketTest3G_1.h
// This is the header file for 3G (WCDMA) Packet Data Service test 3G_1 (RPacketService functionality testing)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST3G_1_H__)
#define __ETELPACKETTEST3G_1_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest3G_1 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest3G_1();
	~CEtelPacketTest3G_1();

	virtual enum TVerdict doTestStepL( void );

};

#endif //(__ETELPACKETTEST3G_1_H__)
