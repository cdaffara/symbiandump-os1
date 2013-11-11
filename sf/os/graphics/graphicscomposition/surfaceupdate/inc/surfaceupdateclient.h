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

#ifndef __SURFACEUPDATECLIENT_H__
#define __SURFACEUPDATECLIENT_H__

#include <e32std.h>
#include <graphics/suerror.h>
#ifdef TEST_SURFACE_UPDATE
#include "surfaceupdate.h"
#include "surfaceupdatetest.h"
#endif

class TSurfaceId;

const TInt KDefaultMessageSlot = 6;

typedef TPckgBuf<TUint32> TTimeStamp;

/**
The RSurfaceUpdateSession class is intended to be used by surface update control flow.
The class implements an interface between the Compositor and clients. It allows a client to 
inform the Compositor when a surface has been modified and to receive notification when 
its buffer becomes available and/or displayed. 
*/
class RSurfaceUpdateSession : public RSessionBase
	{
public:
	IMPORT_C RSurfaceUpdateSession();
	/**
	Connect to the server using given number of message slots.

	@param aMessageSlots The number of message slots available to this session.  
	This parameter is defined in RSessionBase specification; it signifies the maximum number 
	of asynchronous outstanding requests to the server.  
	SubmitUpdate commands could contain up to three asynchronous requests to the server, 
	each corresponds to a different type of notification. 

	@see RSessionBase

	@pre The surface update server is started up
	@post Connection to the server has been established. 

	@return KErrNone if successful. 
	@return KErrAlreadyExists if the session has already been established.
	@return KErrArgument if arguments don’t lie within expected range. 
	@return KErrNotFound if the server is not in operation, otherwise one of the 
	    system-wide error codes.
	*/
	IMPORT_C TInt Connect(TInt aMessageSlots = KDefaultMessageSlot);

	/**
	Close the session to the server. 

	@pre Outstanding notification requests (if any) are cancelled.
	@post The session is disconnected.
	*/
	IMPORT_C void Close();	

	/**
	Send update message to the server. All notification requests will also be submitted at 
	this stage. 
	This is referring to when notifications are requested before the update request.
	<p>The GCE may collapse a few requests into one. 
	If a few requests with different buffer numbers specified for the same surface have 
	been submitted at specific composition cycle, only last such request will have effect. 
	<p>Clients can send a global submit update which will be broadcast by the SUS to all 
	backends presented in the system. For a global update, to all screens, 
	the pre-defined constant KAllScreens needs to be passed as the screen parameter 
	to SubmitUpdate. 
	Note that once a Client has begun issuing SubmitUpdate with either KAllScreens or a 
	specific screen, then only that type of screen can be used thereafter for that 
	Surface Update Session, i.e. the client cannot have a mix of SubmitUpdate with 
	KAllScreens and specific screens.

	@see RSurfaceUpdateSession::NotifyWhenAvailable
	@see RSurfaceUpdateSession::NotifyWhenDisplayed 
	@see RSurfaceUpdateSession::NotifyWhenDisplayedXTimes  

	@param aScreen Screen number. KAllScreens is the only allowed parameter. 
	@param aSurfaceId Secure 128-bit surface unique  identifier, which fully specified the 
	    surface. Surface ID must be assigned and registered with the GCE.
	@param aBuffer Current buffer for composition. Varies from 0 to N-1, where N is the 
	    total number of buffers in the surface.
	@param aRegion The region requiring update, if this is NULL or empty, the whole 
	    surface will be used for composition. The function doesn’t take ownership of 
	    this parameter.

	@pre The surface session must be established to the server.
	@post The GCE will trigger composition to use this region at the next composition cycle. 

	@return KErrNone if a message has been transferred to the server successfully. 
	@return KErrArgument if arguments don’t lie within expected range.
	@return KErrBackendNotAvailable if backend for specified screen hasn’t been registered 
	    with the server.
	@return KErrNotSupported if global and per-screen update are mixed within the same session.
	@return KErrNoMemory if the memory is not enough to fulfill the request, 
	    otherwise one of the system-wide error codes.
	Will panic if applied on disconnected session.
	*/
	IMPORT_C TInt SubmitUpdate(TInt aScreen, const TSurfaceId& aSurfaceId, TInt aBuffer, 
						const TRegion* aDirtyRegion = NULL);

	/**
	The message signals the client that the buffer associated with the notifier is available 
	for further updates.
	<p>For performance reasons, the GCE uses the buffer sent by the client for composition 
	directly rather than composing with a copy of the buffer. 
	This means that, potentially, a client could be rendering to a buffer which is currently 
	being used for composition, leading to artefacts and possible tearing.
	Depending on whether the surface is single or multi-buffered, there is some difference 
	in behaviour:  
	<li>In the case of single-buffered surfaces, the GCE will not wait for the next 
	submit update and will complete the request after the composition. </li>
	<li>In case of multi-buffered surfaces, the GCE can postpone with the issuing the notification 
	to the client even after the composition with the specified surface buffer has already happened, 
	and issues the request when the client changes the active buffer, i.e. submits another 
	request for update with different buffer. This will help the GCE to keep the active buffer in a 
	valid state should another composition required.</li> 
	<p>This notification enables a client to, if desired, be able to perform artefact free 
	rendering using this notification in conjunction with a multi-buffered surface.
	<p>Note that since the compositor can delay to issue request complete for multi-buffered 
	surface until the change of the active buffer occurs, the completion of a particular 
	notifier may not relate to the buffer to which it is intuitively associated. 
	<p>Only the last call of this function before SubmitUpdate command takes place will have 
	effect, i.e. the following call will override the previous one.
	This request will only be associated with the following single call of the SubmitUpdate.
	<p>When a SubmitUpdate has been broadcast to all available screen, i.e. KAllScreens was 
	supplied as a screen number, for multi-buffered surfaces, the notification will be completed 
	when the surface buffer has been released by all displays it is being shown on. 
	For single-buffered surfaces, notification will be completed when all displays have read 
	the buffer once.

	@param aStatus Reference to the request status object.

	@see RSurfaceUpdateSession::SubmitUpdate

	@pre The surface session must be established to the server.
	@post This request for the consumption notification event will be transferred to the GCE 
        at next SubmitUpdate command.  

	@return KErrNone if consumption has succeeded, the request completed aStatus will 
	    contain this code.  
	@return KErrOverflow if the GCE drops the frame for composition. That could happen 
	    if the GCE can’t cope with the current composition rate or the new request arrives 
	    while the current is still in a progress.
	    The buffer overflow error can only happen with double buffering:
	    <li>If notification is not being used</li>
	    <li>If notification is not being used correctly as the client is not waiting for 
	    notification to complete before sending a further buffer.</li>

	    If an application such as video is using three (or more) buffers, this error can occur. 
	    For instance, the video may be producing frames at a fixed rate which is faster than 
	    the GCE can cope with, and in this case frames will be “lost”, but the notification 
	    mechanism will still operate correctly.
	    
	@return KErrBackendNotAvailable if backend for specified screen hasn’t been registered 
	    with the server.
	@return KErrArgument if arguments in SubmitUpdate command don’t lie within expected range.
	@return KErrCancel if notification has been cancelled. It could happen, for instance, 
	    if CancelUpdateNotification is called before the request is completed.
	@return KErrServerBusy if the number of outstanding requests has exceeded specified 
	    value (see   RSurfaceUpdateSession::Connect for details).
	@return KErrNotReady if the environment is in invalid state (for instance, the Surface 
	    Update Server has not been launched or the session is disconnected).
	@return KErrSurfaceNotRegistered if the surface is not registered with the specific backend.
	@return KErrNotVisible if the surface specified in SubmitUpdate is not visible. 
	    This could happen if there is no layer associated with the surface or no active 
	    composition scene. 
	    Note, the absence of this error code does not guarantee that the surface 
	    is visible. For example, if the surface is behind another surface that has alpha blending
	    enabled the compositor will not do a pixel level comparison to determine surface visibility.
	@return KErrSurfaceNotRegistered if the surface is not registered with the specific backend.     
	@return KErrNotSupported if global and per-screen update are mixed within the same session.
	@return KErrNoMemory if the memory is not enough to fulfil the request, any other 
	    system error codes.
	Will panic if applied on disconnected session.
	*/
	IMPORT_C void NotifyWhenAvailable(TRequestStatus& aStatus); 

	/**
	Ask the server to notify when the surface buffer which will be specified in the next 
	update request has been displayed for the first time. 
	The notification will also include exact time of the event as it is supplied by the 
	User::FastCounter(). 
	Only the last call of this function before SubmitUpdate command takes place will 
	have effect, i.e. the following call will override the previous one. 

	@param aStatus Reference to the request status object.
	@param aTimeStamp Reference to the timestamp of the display notification event, 
	   packed into the modified buffer descriptor. 
	   Will only be valid if aStatus is equal to KErrNone. 

	@see RSurfaceUpdateSession::SubmitUpdate

	@pre The surface session must be established to the server.
	@post This request for the display notification event will be transferred to the GCE 
	   at next SubmitUpdate command.

	@return KErrNone if the surface has been displayed successfully, when request 
	    completed aStatus will contain this error code.
	@return KErrOverflow if the updated buffer was not displayed because it was 
	    superseded by a further update on the same surface. 
	@return KErrBackendNotAvailable if backend for specified screen hasn’t been 
	    registered with the server.
	@return KErrArgument if arguments in SubmitUpdate command don’t lie within expected range.
	@return KErrCancel if notification has been cancelled. It could happen, for instance, 
	    if CancelUpdateNotification is called before the request is completed.
	@return KErrServerBusy if the number of outstanding requests has exceeded specified 
	    value (@see RSurfaceUpdateSession::Connect for details).
	@return KErrNotReady if the environment is in invalid state (for instance, the 
	    Surface Update Server has not been launched or session is disconnected).
	@return KErrNotVisible if the surface specified in SubmitUpdate is not visible. 
	    This could happen if there is no layer associated with the surface or no active 
	    composition scene.
	    For global update it signifies that the surface is not visible for any screen. 
	    Note, the absence of this error code does not guarantee that the surface is visible. 
	    For example, if the surface is behind another surface that has alpha blending
	    enabled the compositor will not do a pixel level comparison to determine surface visibility.
	@return KErrSurfaceNotRegistered if the surface is not registered with the specific backend.
	@return KErrNotSupported if global and per-screen update are mixed within the same session.
	@return ErrNoMemory if the memory is not enough to fulfill the request, otherwise any 
	    other system error codes. 
	Will panic if applied on disconnected session.
	*/
	IMPORT_C void NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp);

	/**
	Ask the server to notify when the surface buffer which will 
	be specified in the next update request has been displayed for the X times. 
	<p>Only the last call of this function before SubmitUpdate command takes place 
	will have effect, i.e. the following call will override the previous one.
	<p>In case of the global update the notification will always be aligned with the master 
	display. The SUS will send notification to client when the surface has been displayed on a 
	master display for the X times or if there is no master display for that surface. 
	If the master becomes invisible while the request is still in a progress it will be 
	reassigned to the next visible display with highest priority.
	<p>This request will only be associated with the following single call of the SubmitUpdate.

	@param aCount Number of times the surface is displayed  before the request is completed. 
	    Varies from 1 and greater. 
	@param aStatus Reference to the request status object.

	@pre The surface session must be established to the server.
	@post This request for the display notification event will be transferred to the GCE at 
	    next SubmitUpdate command.  

	@return KErrNone if the surface has been displayed successfully for specified numbers 
	    of times, when request completed aStatus will contain this error code.
	@return KErrOverflow if the updated buffer did not remain on-screen for the specified 
	    number of display refreshes because it was superseded by a further update on the 
	    same surface.
	@return KErrArgument if arguments in SubmitUpdate command don’t lie within expected range.
	@return KErrBackendNotAvailable if backend for specified screen hasn’t been registered 
	    with the server.
	@return KErrCancel if notification has been cancelled. It could happen, for instance, 
	    if CancelUpdateNotification is called before the request is completed.
	@return KErrServerBusy if the number of outstanding requests has exceeded 
	    specified value (@see RSurfaceUpdateSession::Connect for details).
	@return KErrNotReady if the environment is in invalid state (for instance, the 
	    Surface Update Server has not been launched or session is disconnected).
	@return KErrNotVisible if the surface specified in SubmitUpdate is not visible. 
	    This could happen if there is no layer associated with the surface or no active 
	    composition scene. 
	    For global update it signifies that the surface is not visible for any screen. 
	    Note, the absence of this error code does not guarantee that the surface is visible. 
	    For example, if the surface is behind another surface that has alpha blending
	    enabled the compositor will not do a pixel level comparison to determine surface visibility.
	@return KErrSurfaceNotRegistered if the surface is not registered with the specific backend.
	@return KErrNotSupported if global and per-screen update are mixed within the same session.
	@return KErrNoMemory if the memory is not enough to fulfill the request, otherwise 
	    any other system error codes. 
	Will panic if applied on disconnected session.
	*/
	IMPORT_C void NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus&  aStatus);
	
	/**
	Cancel all outstanding requests for notification. 
	The function doesn't cancel the submit update requests themselves.

	@pre The surface session must be established to the server.
	@post The server will issue request complete immediately for all outstanding requests 
	    related to particular session.
	Will panic if applied on disconnected session.
	*/
	IMPORT_C void CancelAllUpdateNotifications();

protected:
    /**
    Allow to retrieve the version number.
	
	@return client version number
    */
	TVersion Version() const;

private:	
	void IssueRequestComplete(TInt aErr);

private:
	TRequestStatus *iStatusAvailable;
	TRequestStatus *iStatusDisplayed;
	TRequestStatus *iStatusDisplayedXTimes;
	TTimeStamp* iTimeStamp;
	TInt iCount;
	};

#endif
