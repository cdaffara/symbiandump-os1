// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef SSMCLAYER_H_ 
#define SSMCLAYER_H_ 

#include <e32std.h> 

/**
 * Constant value used by the compatibility layer as the reason for
 * a state transition when the reason is not needed.
 * 
 * @publishedPartner
 * @released
 */
static const TInt KSsmCLayerNullReason = 0; 

/**
 * Constant value used by the compatibility layer as the reason for
 * a state transition when the state transition indicates a power
 * off of the device is needed.
 * 
 * @publishedPartner
 * @released
 */
static const TInt KSsmCLayerPowerOffReason = 0;

/**
 * Constant value used by the compatibility layer as the reason for
 * a state transition when the device should restart network
 * functionality.
 * 
 * @publishedPartner
 * @released
 */
static const TInt KSsmCLayerRestartNetworkReason = 1; 

#endif /*SSMCLAYER_H_*/ 
