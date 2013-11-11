/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef POWERADVICEHANDLER_H_
#define POWERADVICEHANDLER_H_

#include "LbsInternalInterface.h"

//Interface used for sending power advice to the LbsLocUpdateMonitor
class MPowerAdviceHandler
	{
public:
	virtual void SendPowerAdvice(TLbsPositionUpdateRequestBase::TPowerAdvice, 
									TLbsNetworkEnumInt::TLbsNetProtocolServiceInt) = 0;
	};

#endif /*POWERADVICEHANDLER_H_*/
