/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Condition listener call-back interface
*
*/


#ifndef SENSRVCHANNELCONDITIONLISTENER_H
#define SENSRVCHANNELCONDITIONLISTENER_H

#include <e32base.h>
#include <sensrvchannelconditionset.h>
#include <sensrvtypes.h>

/**
* Condition listener callback interface to indicate when a ConditionSet has been met.
* 
* This class is for use with the CSensrvChannel::StartConditionListeningL() method. Clients wishing
* to use condition listening must inherit from this class and provide a reference to an instance of
* it in CSensrvChannel::StartConditionListeningL().
* 
* @see CSensrvChannel::StartConditionListeningL()
* @see CSensrvChannel::StopConditionListening()
* @see CSensrvChannel::AddConditionL()
* @see CSensrvChannel::RemoveConditionL()
* @lib sensrvclient.lib
* @since S60 5.0
*/
class MSensrvChannelConditionListener
    {
    public:
    /**
    * Callback implemented by a client so that they can be notified when a channel ConditionSet
    * is met. If a client wants to use same ConditionSet after receiving this notification, the
    * client must add ConditionSet again to the channel object. If the ConditionSet is no longer
    * required the client can destroy it. 
    * 
    * If the ConditionSet contains more than one condition, it is not known which of the individual
    * conditions in the set were met.
    * 
    * Clients providing an implementation for this callback must ensure that the operation does not
    * leave. If a leave does occur then the behaviour is undefined.
    * 
    * @since S60 5.0
    * @param  aChannel Channel associated with the listener
    * @param  aChannelConditionSet Channel ConditionSet that is met
    * @param  aValue Channel data value that met the condition
    * @see CSensrvChannel::GetData() for example on how to retrive data from aValue
    */  
    virtual void ConditionMet( CSensrvChannel& aChannel, 
                               CSensrvChannelConditionSet& aChannelConditionSet, 
                               TDesC8& aValue ) = 0;
    
    /**
    * Callback implemented by a client so that they can be notified when condition listening
    * has failed. If the error is fatal the channel will be closed, the sensor server session
    * has been terminated and the channel object is no longer useable. If the error is minor,
    * some condition matches have potentially been missed, however listening is still active.
    * 
    * Clients providing an implementation for this callback must ensure that the operation does not
    * leave. If a leave does occur then the behaviour is undefined.
    * 
    * @since S60 5.0
    * @param  aChannel Channel associated with the listener
    * @param  aError The error severity
    */
    virtual void ConditionError ( CSensrvChannel& aChannel, 
                                  TSensrvErrorSeverity aError ) = 0;

	  /** 
    * Callback to future proof this API so that additional callbacks can be added in the future 
    * without breaking binary compatibility.
    * 
    * @since S60 5.0
    * @param  aInterfaceUid Identifier for the interface to be retrieved
    * @param  aInterface A reference to a pointer for the specified interface. Implementation sets
    * aInterface to a valid pointer if the M-class identified by aInterfaceUid is supported, otherwise
    * it is set to NULL on exit.
    * @leave  One of the system-wide error codes
    */
	  virtual void GetChannelConditionListenerInterfaceL( TUid aInterfaceUid, TAny*& aInterface ) = 0;

    };



#endif //SENSRVCHANNELCONDITIONLISTENER_H

// End of File
