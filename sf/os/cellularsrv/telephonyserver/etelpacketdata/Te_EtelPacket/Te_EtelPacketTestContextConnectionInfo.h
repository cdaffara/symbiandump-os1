// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for Packet Data Context test Connection Information
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __ETELPACKETTESTCONTEXTCONNECTIONINFORMATION_H__
#define __ETELPACKETTESTCONTEXTCONNECTIONINFORMATION_H__

#include "Te_EtelPacketTestStepBase.h"

_LIT(KContextConnectionInfo,"Context_Connection_Info");

class CEtelPacketTestContextConnectionInfo : public CTestStepEtelPacket
{
public:
	CEtelPacketTestContextConnectionInfo();
	~CEtelPacketTestContextConnectionInfo();

	virtual enum TVerdict doTestStepL( void );
};

//
#endif //__ETELPACKETTESTCONTEXTCONNECTIONINFORMATION_H__
