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
// Camera specific errors
// 
//

/**
 @internalTechnology
 @released
 @file
*/

#ifndef  ECAMERRORS_DEF_H
#define  ECAMERRORS_DEF_H

/**
This error code is supposed to be used with new event KUidECamEvent2CameraRequestForcedTimedTakeOver 
and KUidECamEvent2CameraRequestTimedTakeOver.
This would mean that another such reserve request is outstanding and has higher priority than this client.

@internalTechnology
*/
static const TInt KErrECamHighPriorityReserveRequesterExists = -12107;

#endif // ECAMERRORS_DEF_H
