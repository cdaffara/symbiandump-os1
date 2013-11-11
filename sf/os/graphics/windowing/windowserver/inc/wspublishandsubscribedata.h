// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Standard window server header file
// 
//
#ifndef WSPUBLISHANDSUBSCRIBEDATA_H
#define WSPUBLISHANDSUBSCRIBEDATA_H

#include <e32cmn.h>

// Values for the render orientation P&S value that we publish
// Used when the property is defined, attached to, and deleted
const TUid  KRenderOrientationCategory  = {268450592};  // WServ UID3
const TUint KRenderOrientationKey       = 0x102872E5;

/** The orientation of rendering the application intends to use.

@see RWsSession::IndicateAppOrientation(TOrientation aOrientation)*/
enum TRenderOrientation
    {
    /** Fixed default orientation */
    EDisplayOrientationNormal,
    /** 90° clockwise */
    EDisplayOrientation90CW,
    /** 180° */
    EDisplayOrientation180,
    /** 270° clockwise */
    EDisplayOrientation270CW,
    /** The orientation is from the P&S Key */
    EDisplayOrientationAuto,
    /** use the orientation specified by the next top level window(s) that does not have ignore flag set. 
    E.g. system dialog on top of foreground app. */
    EDisplayOrientationIgnore
    };

#endif // WSPUBLISHANDSUBSCRIBEDATA_H
