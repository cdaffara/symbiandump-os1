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
// @file
// @note Logging functionality for the PAN agent
// 
//

#ifndef PANHELPERLOG_H
#define PANHELPERLOG_H

namespace PanHelper
{

#ifdef __FLOG_ACTIVE // define everything we need for logging

const TUint KMaxBtAddrSize = 19;	// 0x11:22:33:44:55:66 - 19 chars
_LIT(KBtAddrSeparator, ":");

#endif // __FLOG_ACTIVE

} // namespace PanHelper
	
#endif // PANHELPERLOG_H
