/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains definition for setting wakeup flag for an alarm.
*
*/


#ifndef __WAKEUPALARM_H__
#define __WAKEUPALARM_H__

#pragma message( "Wakeupalarm.h has been deprected. Please use Alarm Server API instead." )

/**
* Index of the wakeup alarm flag, to be used with TBitFlagsT class.
*
* Set this flag on in ClientFlags of a TASShdAlarm to indicate that the alarm
* is a wakeup alarm (it should wake the terminal up even if powered off).
*
* Example:
*   TASShdAlarm alarm;
*   alarm.ClientFlags().Set( KWakeupAlarmFlagIndex );
*/
const TInt KWakeupAlarmFlagIndex = 0;

#endif // __WAKEUPALARM_H__
