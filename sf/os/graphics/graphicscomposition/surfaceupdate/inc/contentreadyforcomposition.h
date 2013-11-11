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
// Interface to find out when a particular content update 
// for a given surface has been delivered to the compositor
// and will be used in future compositions involving that 
// surface.
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CONTENTREADYFORCOMPOSITION_H__
#define __CONTENTREADYFORCOMPOSITION_H__


#include <e32std.h>
#include <graphics/surface.h>


/**
MContentReadyForComposition is an interface to find out when a 
particular content update for a given surface has been delivered to
the compositor and will be used in future compositions involving that 
surface.

Each compositor in the system maintains its own content update count 
for each registered surface that determines when content ready 
notifications will be completed. Therefore the content update count is
considered per-surface-per-screen. The content update count is 
internally incremented by the compositor every time it receives a 
valid content update for a registered surface. Frames may be skipped 
if the client submits content updates too fast, meaning that not every
update is displayed on screen. The first valid content update sets the
content update count to one. Updates sent before the surface is 
registered do not affect the content update count. Content ready 
notifications for aContentUpdate = 1 will only be successfully 
completed after at least one content update has been received.
*/
class MContentReadyForComposition
    {
public:
    enum
        {
        KUidContentReadyForComposition = 0x2002E6D5,
        ETypeId = KUidContentReadyForComposition
        };
public:

    /**
    Requests a notification when a particular content update for a 
    given surface has been delivered to the compositor. Further, 
    notification implies that this newest content will be used in 
    future compositions until such time that newer content is 
    available.

    @param  aSurface        Id of the surface for which the
                            notification is required.
    @param  aNotifyReady    Request status that is signaled on the 
                            calling thread.
    @param  aContentUpdate  Content update number for which 
                            notification is required.

    @note   aContentUpdate is one-based. Content update one 
            corresponds to the first content submitted to the 
            compositor for a given surface.

    @note   If aContentUpdate is less than or equal to the current 
            content that the compositor uses for aSurface during 
            composition, aNotifyReady is completed with KErrNone 
            without delay. Else aNotifyReady is completed with 
            KErrNone when the compositor has received content update
            number aContentUpdate for the surface.

    @note   aNotifyReady is set to KRequestPending during the call
            to NotifyContentReady() and remains set to this value 
            until the request is completed with the appropriate 
            return code. The client must wait for this asynchronous 
            completion before attempting to read the return value.

    @note   If the registration count for aSurface becomes zero and 
            the surface is no longer in use in either the committed 
            or uncommitted scene while a notification request is 
            outstanding, the notification is completed with KErrAbort,
            without delay soon after the surface is unregistered.

    @note   Due to the 64-bit precision of aContentUpdate, the 
            probability of wrap around is considered slim and 
            therefore ignored. If wrap around does occur, behaviour of
            the API is undefined.

    @pre    aSurface is registered.
    @pre    aContentUpdate >= 1.
    @pre    There is no outstanding notification request for aSurface. 

    @error  If aSurface is not registered, aNotifyReady is completed
            with KErrArgument without delay.
    @error  If aContentUpdate == 0, aNotifyReady is completed with 
            KErrArgument without delay.
    @error  If there is an outstanding notification request for 
            aSurface, aNotifyReady is completed with KErrInUse without
            delay.

    @post   aNotifyReady will be signaled on the calling thread
            according to the above criteria.
    */
    virtual void NotifyContentReady(const TSurfaceId& aSurface, TUint64 aContentUpdate, TRequestStatus& aNotifyReady) = 0;

    /**
    Cancels a notification request made previously with 
    MContentReadyForComposition::NotifyContentReady() for a specific
    surface.

    @see    MContentReadyForComposition::NotifyContentReady().

    @param  aSurface        Id of the surface for which the 
                            notification request will be cancelled.

    @pre    aSurface is registered. 
    
    @error  If aSurface is not registered, this function is a noop.

    @post   If there is an outstanding notification request for 
            aSurface at the time of the call, it is completed with 
            KErrCancel without delay on the thread that originally 
            called NotifyContentReady() to make the request. If there
            is no outstanding notification request, this function is 
            a noop.

    @note   Completion of an outstanding request with KErrCancel
            must not rely on the calling thread entering the active 
            scheduler after the call to NotifyContentReadyCancel(). 
            It is valid for the client to call User::
            WaitForRequest(aNotifyRequest), where aNotifyRequest is 
            the outstanding request, immediately after calling 
            NotifyContentReadyCancel().
    */
    virtual void NotifyContentReadyCancel(const TSurfaceId& aSurface) = 0;
    };


#endif //__CONTENTREADYFORCOMPOSITION_H__
