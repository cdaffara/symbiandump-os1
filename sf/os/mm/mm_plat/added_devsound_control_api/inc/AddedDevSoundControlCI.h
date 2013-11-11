/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Project specification for AddedDevSoundControl Custom Interface
*
*/


#ifndef __ADDEDDEVSOUNDCONTROLCI_H__
#define __ADDEDDEVSOUNDCONTROLCI_H__

// INCLUDES
#include <e32base.h>

// CONSTANTS
// Interface UID associated to the Custom Interface class.
const static TUid KUidAddedDevSoundControlInterface = {0x101FAF92};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  Interface class for altering pause and resume behavior.
 *
 *  Clients of DevSound API can use this interface alter the default pause
 *  and resume behavior. By default when CMMFDevSound::Pause() is called data
 *  already sent to it is flushed/discarded and audio resources are released.
 *  Upon next PlayInitL() audio resources are re-allocated.
 *
 *  DevSound clients can obtain a handle to this interface by calling
 *  CMMFDevSound::CustomInterface() function with UID
 *  KUidAddedDevSoundControlInterface. Along with the handle, the ownership of
 *  the object is also transferred to the client.
 *
 *  This interface is valid until the DevSound is re-initialized or deleted.
 *  Client shuold make sure that they delete the interface before DevSound is
 *  re-initialized or deleted.
 *
 *  @lib n/a
 *  @since S60 3.2
 */
class MAddedDevSoundControl
	{
public:

    /**
     *
     * Alters DevSound's behavior for handling Pause and Resume.
     *
     * Can be called after DevSound creation. Must be called prior to calling
     * CMMFDevSound::Pause() to take effect.
     *
     * When value is ETrue, Calling CMMFDevSound::Pause() will halt the decoding
     * and/or rendering temporarily. Data buffers sent to the DevSound instance 
     * are buffered up. Resources associated with the DevSound instance is kept
     * intact. Upon resuming via CMMFDevSound::PlayInitL(), all buffered data
     * will be decoded and/or rendered.
     * When value is EFalse, CMMFDevSound::Pause() will stop decoding and/or
     * rendering immediately. Data buffers sent to the DevSound instance that
     * were not decoded/rendered are flushed. Resources associated with
     * DevSound instance are also released.
     *
     * Not necessarily supported by all platforms.
     *
     * Will return a KErrNotSupported if the DevSound behavior cannot be altered.
     *
     * @since S60 3.2
     * @param TBool - Toggles HW awarness
     * @return TInt - Function call status
     *
     */
     virtual TInt SetHwAwareness(TBool aHwAware) = 0;

     /**
      *
      * Flushes audio data that are not decoded/rendered yet.
      *
      * Not necessarily available in all platforms.
      *
      * DevSound instance must be in the playing PAUSED state.
      *
      * @since S60 3.2
      * @param none
      * @return TInt - Function call status.
      *         KErrNone if audio data is flushed successfully.
      *         KErrInUse if DevSound is not in paused state.
      *         KErrNotSupported if the feature is not available and/or
      *         hardware awareness flag is not set.
      *
      */
     virtual TInt PauseAndFlush() = 0;

    /*
     *
     * Provide virtual destructor to allow DevSound clients to delete
     * instance through the pointer recieved via call to CustomInterface().
     *
     */
    virtual ~MAddedDevSoundControl() {};

	};

#endif // __ADDEDDEVSOUNDCONTROLCI_H__

// End of File
