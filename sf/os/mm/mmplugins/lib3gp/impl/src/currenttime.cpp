// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <3gplibrary/mp4config.h>
#include "mp4currenttime.h"
#include <time.h>


/*
 * Function:
 *
 *   mp4_i32 getCurrentTime(mp4_u32 *currentTime)
 *
 * Description:
 *
 *   Calculate time in seconds since Jan 1st, 1970.
 *
 * Parameters:
 *
 *   currentTime   Time is returned here.
 *
 * Return value:
 *
 *   0             Success
 *   -1            Error
 *
 */
mp4_i32 getCurrentTime(mp4_u32 *currentTime)
{
  time_t ltime;

  /* Get time in seconds since Jan 1st, 1970 */
  if (time(&ltime) == -1)
    return -1;

  /* Convert time to start from Jan 1st, 1904 */
  *currentTime = (mp4_u32)ltime + ((mp4_u32)66 * 365 + 17) * 24 * 3600;

  return 0;
}

// End of File
