/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
@prototype
*/

#ifndef LBS_NET_EXTINTERFACE_VERSIONS_H
#define LBS_NET_EXTINTERFACE_VERSIONS_H

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	/**
	CLbsNetProtocolBase::ExtendedInterface aFunctionNumber parameter.
	
	Special attention must be paid when modifying the enum due to BC issues!
	
	@internalTechnology
	@released					
	*/
	enum TLbsNetExtIntVer
		{
		ELbsNetExtInterfaceCount = 0,
		ELbsNetExtInterface2,
		ELbsNetExtInterfaceMax // Always keep at the end
		};

#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#endif // LBS_NET_EXTINTERFACE_VERSIONS_H
