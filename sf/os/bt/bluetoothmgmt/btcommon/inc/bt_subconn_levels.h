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
// Contains all of the definitions for levels within Bluetooth
// that are used by the RSubConnection::Control API.
// The definitions here are for currently intended for internal use only.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef BT_SUBCONN_LEVELS_H
#define BT_SUBCONN_LEVELS_H

//
// Levels for RSubConnection::Control
//
// Sccl == SubConnectionControlLevel
//
const TUint	KScclBtUpperAvctp  =0x3000;		/*!< AVCTP Upper SubConnection Control Level */
const TUint	KScclBtLowerAvctp  =0x3001;		/*!< AVCTP Lower SubConnection Control Level */


#endif // BT_SUBCONN_LEVELS_H
