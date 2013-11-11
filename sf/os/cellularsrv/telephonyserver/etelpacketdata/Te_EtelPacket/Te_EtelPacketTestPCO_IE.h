// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTestPCO_IE.h
// This is the header file for Packet Data Context Data Structure
// 
//

/**
 @file
 @internalComponent 
*/

#if (!defined __ETEL_PACKET_TEST_PCO_IE_H__)
#define __ETEL_PACKET_TEST_PCO_IE_H__

#include "Te_EtelPacketTestStepBase.h"

class CEtelPacketTestPCO_IE : public CTestStepEtelPacket
{
public:
	CEtelPacketTestPCO_IE();
	~CEtelPacketTestPCO_IE();

	virtual enum TVerdict doTestStepL( void );
};

//	
#endif //(__ETEL_PACKET_TEST_PCO_IE_H__)
