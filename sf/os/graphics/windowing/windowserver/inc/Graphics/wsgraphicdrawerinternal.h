// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef GRAPHICDRAWER_INTERNAL_H
#define GRAPHICDRAWER_INTERNAL_H

class MWsAnimationScheduler: public MWsObjectProvider
/** A system mechanism for customising the animation timer to a particular hardware variant or to a
	particular external compositor e.g. an out-of-scene transition engine.
	The default animation scheduler in WSERV does not understand device-dependant parameters e.g.
	screen refresh rates
	@internalComponent
	@released
*/	{
public:
	/** Called to schedule an animation event for a screen to occur not before some point
	A screen may be scheduled to animate several times without actually receiving an animation event from the scheduler
	If the scheduler cannot honour the request e.g. device constraints, it ignores the request.
	@param aScreen the screen to recieve the event
	@param aWhen the point in time the event need not occur before
	*/
	virtual void ScheduleAnimation(MWsScreen& aScreen,const TTime& aWhen) = 0;
	/** Called if a screen determines that it no longer needs to schedule any animation
	@param aScreen the screen not needing to receive any animation events
	*/
	virtual void UnscheduleAnimation(MWsScreen& aScreen) = 0;
	/** Called to schedule the redrawing of all screens containing this graphic.
	The redrawing of affected screens can be deferred by the scheduler.
	@param aId the ID of the graphic to be redrawn.  The graphic itself need not exist
		(e.g. it has just been deleted, hence the need to redraw all potentially affected screens). */
	virtual void Invalidate(const TGraphicDrawerId& aId) = 0;
	/** Called when the Window Server receives a TRawEvent::EInactive  event, to signal power-saving */
	virtual void OnInactive() = 0;
	/** Called when the Window Server receives a TRawEvent::EActive event, to signal screen back on */
	virtual void OnActive() = 0;

	/** Called to schedule a redraw for a screen which is out of date
	A screen may be scheduled to redraw several times without actually receiving aredraw event from the scheduler
	If this request is ignored then the screen will be draw incorrectly untill a such a request is not ignored.
	@param aScreen the screen to recieve the event
	@param aWhen the point in time the event need not occur before
	*/
	virtual void ScheduleRedraw(MWsScreen& aScreen,const TTime& aWhen) = 0;
	/** Called to force a previously scheduled redraw for a screen to happen immediately.
	Used when we are about to loose our ability to perform the redraw correctly.
	@param aScreen the screen to recieve the event
	*/
	virtual void DoRedrawNow(MWsScreen& aScreen) = 0;
	
	/** Interface to be implemented by callers of the asynchronous overload of 
	DoRedrawNow, which takes a reference to this interface as argument to be able 
	to give a callback once the screen update has been completed.
	*/
	class MScreenUpdateObserver 
		{
	public:
		/** Callback when an asynchronous DoRedrawNow has been completed. */
		virtual void ScreenUpdateComplete(TInt aResult) = 0;
		};
	
	/** Called to force a previously scheduled redraw for a screen to happen immediately. 
	@param aScreen the screen to recieve the event
	@param aObserver reference which will receive a notification when the screen has been updated
	*/
	virtual void DoRedrawNow(MWsScreen& aScreen, MScreenUpdateObserver& aObserver) = 0;
	
	/** Callback to use if aObserver gets deleted before screen update takes place. */
	virtual void ClearScreenUpdateObserver(const MScreenUpdateObserver& aObserver) = 0;

protected:
	IMPORT_C void Animate(MWsScreen& aScreen);
	IMPORT_C void Animate(MWsScreen& aScreen, TRequestStatus* aFinished);
	IMPORT_C void Redraw(MWsScreen& aScreen);
	IMPORT_C TBool RedrawInvalid(MWsScreen& aScreen,const TArray<TGraphicDrawerId>& aInvalid);
	};
	
#endif