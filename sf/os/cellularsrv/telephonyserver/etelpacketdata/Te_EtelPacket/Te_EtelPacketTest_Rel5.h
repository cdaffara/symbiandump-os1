// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest_Rel5.h
// This is the header file for Packet Data Service testing Release 5 (RPacketService functionality testing)
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETELPACKETTEST_REL5_H__)
#define __ETELPACKETTEST1_REL5_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTest_Rel5 : public CTestStepEtelPacket
{
public:
	CEtelPacketTest_Rel5();
	~CEtelPacketTest_Rel5();

	virtual enum TVerdict doTestStepL( void );
};

#endif //(__ETELPACKETTEST1_REL5_H__)

