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
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __WSCONTENTREADYFORCOMPOSITION_H__
#define __WSCONTENTREADYFORCOMPOSITION_H__


#include <e32std.h>
#include <graphics/surface.h>
#include <graphics/wsgraphicdrawerinterface.h>


/**
MWsContentReadyForComposition is an interface used to wrap the underlying
compositor extension, MContentReadyForComposition, into the window server.
See contentreadyforcomposition.h for deatils of the API.
*/
class MWsContentReadyForComposition : public MWsObjectProvider
    {
public:
    DECLARE_WS_TYPE_ID(KMWsContentReadyForComposition)

public:
    virtual void NotifyContentReady(const TSurfaceId& aSurface, TUint64 aContentUpdate, TRequestStatus& aNotifyReady) = 0;
    virtual void NotifyContentReadyCancel(const TSurfaceId& aSurface) = 0;
    };


#endif //__WSCONTENTREADYFORCOMPOSITION_H__
