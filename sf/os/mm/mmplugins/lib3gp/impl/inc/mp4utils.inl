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

inline mp4_bool isAvcVideo( mp4_u32 videotype )
    {
    return
        (
        videotype == MP4_TYPE_AVC_PROFILE_BASELINE  ||
        videotype == MP4_TYPE_AVC_PROFILE_MAIN ||
        videotype == MP4_TYPE_AVC_PROFILE_EXTENDED ||
        videotype == MP4_TYPE_AVC_PROFILE_HIGH
        );
    }

inline mp4_bool containsAvcVideo( mp4_u32 type )
    {
    return
        (
        (type & MP4_TYPE_AVC_PROFILE_BASELINE)  ||
        (type & MP4_TYPE_AVC_PROFILE_MAIN) ||
        (type & MP4_TYPE_AVC_PROFILE_EXTENDED) ||
        (type & MP4_TYPE_AVC_PROFILE_HIGH)
        );
    }

