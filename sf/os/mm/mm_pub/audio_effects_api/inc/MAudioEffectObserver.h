/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the Audio Effect Observer class.
*
*/



#ifndef AUDIOEFFECTOBSERVER_H
#define AUDIOEFFECTOBSERVER_H

// CLASS DECLARATION


/**
*  Interface class to be implemented by objects that are interested in receiving events from
*  audio effect.
*
*  @since 3.0
*/
class MAudioEffectObserver
    {
    public:

		static const TUint KEnabled = 1;				// The effect is enabled.
		static const TUint KDisabled = 2;				// The effect is disabled
		static const TUint KEnforced = 3;				// The effect is enforced
		static const TUint KNotEnforced = 4;			// The effect is not enforced
		static const TUint KLostUpdateRights = 5;		// The effect has lost update rights
		static const TUint KGainedUpdateRights = 6;		// The effect has gained update rights
		static const TUint KSpecificEffectBase = 20;	// Offset to specific effect events

        /**
        * Invoked by the audio effect object when the status of the audio effect has changed.
        * @since 3.0
        * @param aObservedEffect The effect that changed
        * @param aEvent The specific event Id of the effect that changed.
        */
        virtual void EffectChanged( const CAudioEffect* aObservedEffect, TUint8 aEvent ) = 0;

    };

#endif      // AUDIOEFFECTOBSERVER_H

// End of File
