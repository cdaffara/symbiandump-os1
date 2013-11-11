/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef __BTAUDIOSTREAMINPUTBASE_H__
#define __BTAUDIOSTREAMINPUTBASE_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfutilities.h> 

/**
 *  Observer interface for receiving the audio data buffers. 
 *
 *  This class defines an observer interface to be implemented by the 
 *  class that is going to receive the audio data. 
 *
 *  @lib btaudioadaptation.lib 
 *  @since S60 v3.1
 */
class MBTAudioStreamObserver 
{
public: 
    /**
     * Method for receiving audio data and an associated timestamp. 
     *
     * @since S60 v3.1 
     * @param aBuffer the audio data frames from the encoder. 
     * @param aTimestamp the timestamp of the first audio frame in the aBuffer 
     * @return a Symbian OS wide error code. 
     */
	virtual TInt Receive(const TDesC8& aBuffer, TTimeIntervalMicroSeconds aTimestamp) = 0;

    /**
     * Method for receiving audio data. 
     *
     * @since S60 v3.1 
     * @param aBuffer the audio data frames from the encoder. 
     * @return a Symbian OS wide error code. 
     */
	virtual TInt Receive(const TDesC8& aBuffer) = 0;
}; 

/**
 *  Observer interface for receiving the audio adaptation errors. 
 *
 *  This class defines an observer interface to be implemented by the 
 *  class that is going to receive the errors that have occurred in 
 *  BT Audio Adaptation or in the audio subsystem. 
 *
 *  To be implemented by a class who uses this adaptation class. 
 *  @lib btaudioadaptation.lib 
 *  @since S60 v3.1
 */
class MBTAudioErrorObserver 
{
public: 
    /**
 		 *  Method for receiving the errors. 
     *
     * @since S60 v3.1 
     * @param aError a Symbian OS wide error code. 
     * @return a Symbian OS wide error code. 
     */
	virtual TInt Error(const TInt aError) = 0;
}; 

/**
 *  This class specifies the interface for receiving audio data from audio subsystem. 
 *
 *  This class contains the interface definition. It can't be instantiated directly, 
 *  but instead the implementation is done in the class inherited from this class. 
 *  
 *  Also, the factory method for the base class must be implemented by the 
 *  implementator of this interface. 
 *
 *  @lib btaudioadaptation.lib
 *  @since S60 v3.1 
 */
class CBTAudioStreamInputBase : public CBase
{
public: 
    IMPORT_C static CBTAudioStreamInputBase* NewL( MBTAudioStreamObserver &aStreamObserver, MBTAudioErrorObserver &aErrorObserver );

    /**
     * This method is for connecting to the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
    virtual TInt Connect() = 0;

    /**
     * This method is for disconnecting from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return void
     */
    virtual void Disconnect() = 0;

    /**
     * This method is for passing the custom interfaces that are 
     * needed sometimes for configuring the hardware encoder. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param aInterfaceId specifies the id of custom interface that the client 
     *        wants to get. 
     * @return  A pointer to the requested interface. Client must cast it into a correct class. 
     */
    virtual TAny *EncoderInterface(TUid aInterfaceId) = 0;

    /**
     * This method is for choosing the encoder for the audio data. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param aDesiredFourCC specifies the encoder that the client wants to use. 
     * @return a Symbian OS wide error code. 
     */
    virtual TInt SetFormat(TFourCC aDesiredFourCC) = 0; 

    /**
     * This method is for starting the recording from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
    virtual TInt Start() = 0;

    /**
     * This method is for stopping the recording from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
    virtual void Stop() = 0;

    /**
	 * This method is called when the client has processed the buffer it got 
	 * from the Receive method. 
	 * 
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param aBuffer refers to the buffer that was received earlier in the Receive method. 
     * @return  void
     */
    virtual void BufferEmptied( const TDesC8 &aBuffer ) = 0;
}; 

#endif 
