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

/*
 * Function:
 *
 *   mp4_u16 u16endian(mp4_u16 u)
 *
 * Description:
 *
 *   This function converts the input parameter u between network byte
 *   order (big endian) and current computer byte order.
 *
 *   Note: Intel based Windows system is little endian.
 *         ARM based Symbian OS is little endian.
 *
 * Parameters:
 *
 *   u     Input value
 *
 * Return value:
 *
 *   Converted value
 *
 */
mp4_u16 u16endian(mp4_u16 u)
{
  mp4_u16 result;

  ((mp4_u8 *)&result)[0] = ((mp4_u8 *)&u)[1];
  ((mp4_u8 *)&result)[1] = ((mp4_u8 *)&u)[0];

  return result;
}


/*
 * Function:
 *
 *   mp4_u32 u32endian(mp4_u32 u)
 *
 * Description:
 *
 *   This function converts the input parameter u between network byte
 *   order (big endian) and current computer byte order.
 *
 *   Note: Intel based Windows system is little endian.
 *         ARM based Symbian OS is little endian.
 *
 * Parameters:
 *
 *   u     Input value
 *
 * Return value:
 *
 *   Converted value
 *
 */
mp4_u32 u32endian(mp4_u32 u)
{
  mp4_u32 result;

  ((mp4_u8 *)&result)[0] = ((mp4_u8 *)&u)[3];
  ((mp4_u8 *)&result)[1] = ((mp4_u8 *)&u)[2];
  ((mp4_u8 *)&result)[2] = ((mp4_u8 *)&u)[1];
  ((mp4_u8 *)&result)[3] = ((mp4_u8 *)&u)[0];

  return result;
}


/*
 * Function:
 *
 *   mp4_i32 i32endian(mp4_i32 i)
 *
 * Description:
 *
 *   This function converts the input parameter i between network byte
 *   order (big endian) and current computer byte order.
 *
 *   Note: Intel based Windows system is little endian.
 *         ARM based Symbian OS is little endian.
 *
 * Parameters:
 *
 *   i     Input value
 *
 * Return value:
 *
 *   Converted value
 *
 */
mp4_i32 i32endian(mp4_i32 i)
{
  mp4_i32 result;

  ((mp4_u8 *)&result)[0] = ((mp4_u8 *)&i)[3];
  ((mp4_u8 *)&result)[1] = ((mp4_u8 *)&i)[2];
  ((mp4_u8 *)&result)[2] = ((mp4_u8 *)&i)[1];
  ((mp4_u8 *)&result)[3] = ((mp4_u8 *)&i)[0];

  return result;
}


/*
 * Function:
 *
 *   mp4_u64 u64endian(mp4_u64 u)
 *
 * Description:
 *
 *   This function converts the input parameter u between network byte
 *   order (big endian) and current computer byte order.
 *
 *   Note: Intel based Windows system is little endian.
 *         ARM based Symbian OS is little endian.
 *
 * Parameters:
 *
 *   u     Input value
 *
 * Return value:
 *
 *   Converted value
 *
 */
mp4_u64 u64endian(mp4_u64 u)
{
  mp4_u64 result;

  ((mp4_u8 *)&result)[0] = ((mp4_u8 *)&u)[7];
  ((mp4_u8 *)&result)[1] = ((mp4_u8 *)&u)[6];
  ((mp4_u8 *)&result)[2] = ((mp4_u8 *)&u)[5];
  ((mp4_u8 *)&result)[3] = ((mp4_u8 *)&u)[4];
  ((mp4_u8 *)&result)[4] = ((mp4_u8 *)&u)[3];
  ((mp4_u8 *)&result)[5] = ((mp4_u8 *)&u)[2];
  ((mp4_u8 *)&result)[6] = ((mp4_u8 *)&u)[1];
  ((mp4_u8 *)&result)[7] = ((mp4_u8 *)&u)[0];

  return result;
}

// End of File
