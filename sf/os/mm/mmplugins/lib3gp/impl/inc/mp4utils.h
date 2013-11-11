// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors: Nokia Corporation
//
// Description: Internal utility functions used by the 3GP library. 
//

#ifndef MP4UTILS_H
#define MP4UTILS_H

#include <3gplibrary/mp4lib.h>

/**
 * Returns true if the given video type is equal to any one of the
 * supported AVC profiles.
 * The supported AVC profiles are:
 *  MP4_TYPE_AVC_PROFILE_BASELINE
 *  MP4_TYPE_AVC_PROFILE_MAIN
 *  MP4_TYPE_AVC_PROFILE_EXTENDED
 *  MP4_TYPE_AVC_PROFILE_HIGH
 * 
 * @param videotype The video type to be checked
 *
 * @return  true if videotype is a supported AVC profile, false otherwise.
 */
inline mp4_bool isAvcVideo( mp4_u32 videotype );

/**
 * Returns true if the given audio/video type has flags corresponding
 * to any of the supported AVC profiles set.
 * The supported AVC profiles are:
 *  MP4_TYPE_AVC_PROFILE_BASELINE
 *  MP4_TYPE_AVC_PROFILE_MAIN
 *  MP4_TYPE_AVC_PROFILE_EXTENDED
 *  MP4_TYPE_AVC_PROFILE_HIGH
 * 
 * @param type  The audio/video type to be checked
 *
 * @return  true if type has bits corresponding to supported AVC profiles set,
 *          false otherwise.
 */
inline mp4_bool containsAvcVideo( mp4_u32 type );


#include "mp4utils.inl"
#endif // MP4UTILS_H
