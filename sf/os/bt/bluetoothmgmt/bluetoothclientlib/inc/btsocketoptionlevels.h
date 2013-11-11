// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// BT socket interface types
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef BTSOCKETOPTIONLEVELS_H
#define BTSOCKETOPTIONLEVELS_H

#include <e32def.h>

// Socket Options
const TUint	KSolBtBlog  =0x1000;		/*!< Logging socket option */
const TUint KSolBtHCI   =0x1010;		/*!< HCI socket option */
const TUint KSolBtLM    =0x1011;		/*!< Link Manager socket option */
const TUint KSolBtL2CAP =0x1012;		/*!< L2CAP socket option */
const TUint KSolBtRFCOMM=0x1013;		/*!< RFCOMM socket option */
const TUint KSolBtAVCTP	=0x1014;		/*!< AVCTP socket option */
const TUint KSolBtACL	=0x1015;		/*!< ACL socket option */
/**
Decimal Value: 4118.
*/
const TUint	KSolBtAVDTPSignalling	=0x1016;
/**
Decimal Value: 4119.
*/
const TUint	KSolBtAVDTPMedia		=0x1017;
/**
Decimal Value: 4120.
*/
const TUint	KSolBtAVDTPReporting	=0x1018;
/**
Decimal Value: 4121.
*/
const TUint	KSolBtAVDTPRecovery		=0x1019;
/**
Decimal Value: 4128.
*/
const TUint KSolBtAVDTPInternal		=0x1020;
const TUint KSolBtLMProxy = 0x2011;		/*!< Link Manager Proxy socket option */
const TUint KSolBtSAPBase  = 0x2020;	/*!< CBluetoothSAP handles SetOpt first */

#endif // BTSOCKETOPTIONLEVELS_H