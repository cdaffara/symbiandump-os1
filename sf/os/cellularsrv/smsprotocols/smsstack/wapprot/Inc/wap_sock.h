// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IOCTL values
// 
//

const TUint KSolWapProv = 0x0010;				//< Ioctl level for wap provider

/**
 *  Ioctl name for getting the length of datagram
 *  
 *  @capability None
 */
const TUint KSOGetLength			=	0x100;

/**
 *  Ioctl name for getting the length of the message parameters
 *  
 *  @capability None
 */
const TUint KSOGetMessageParametersLength	=	0x101;

/**
 *  Ioctl name for getting the message parameters of wap message
 *  
 *  @capability ReadDeviceData
 */
const TUint KSOGetMessageParameters	=	0x102;

//
// Option Settings
//
const TInt KWapSmsOptionNewStyleClient	= 0x04;	//< option for new style clients
const TInt KWapSmsOptionOKToDeleteMessage = 0x05;	//< option for confirming the receipt of message for new style client

