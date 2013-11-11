/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : EPos_MPositionerStatus.h
* Part of     : Mobile Location Framework / Positioning Plugin FW
* Interface   : Positioning Plug-in API
* Interface used by positioners to report positioner status
* Version     : %version: 2 %
*
*/




/**
 @file
 @publishedPartner
 @released
*/

#ifndef MPOSITIONERSTATUS_H
#define MPOSITIONERSTATUS_H

class TPositionModuleStatus;

/**
*  This is an interface through which the PSY can report its current status.
*/
class MPositionerStatus
    {
    public:

        /**
        * Reports that the positioner status has changed.
        * A PSY should not report EDeviceDisabled, so that status will be
        * ignored.
        *
        * @param [in] aStatus The new status.
        */
        virtual void ReportStatus( const TPositionModuleStatus& aStatus ) = 0;

    };  

#endif  // MPOSITIONERSTATUS_H
