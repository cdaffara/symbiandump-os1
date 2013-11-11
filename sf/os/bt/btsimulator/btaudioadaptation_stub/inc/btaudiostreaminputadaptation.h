/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This header specifies the implementation of BT Audio Adaptation API. 
*
*/


#ifndef __BTAUDIOSTREAMINPUTADAPTATION_H__
#define __BTAUDIOSTREAMINPUTADAPTATION_H__

#include <e32base.h>
#include <e32std.h>
#include "btaudiostreaminputbase.h" 

/**
 *  This class implements the interface specified by CBTAudioStreamInputBase. 
 *
 *  This class contains the stub implementation of the CAudioStreamInputAdaptation, 
 *  that inherits from CBTAudioStreamInputBase. 
 *  
 *  To the user it seems as if he's operating with the base class, because he calls, 
 *  but in practice the class he uses is an instance of this class. 
 *
 *  @lib btaudioadaptation.lib
 *  @since S60 v3.1 
 */
NONSHARABLE_CLASS(CBTAudioStreamInputAdaptation) : public CBTAudioStreamInputBase 
    {
    public:  
    static CBTAudioStreamInputAdaptation* NewL( MBTAudioStreamObserver& aStreamObserver, MBTAudioErrorObserver& aErrorObserver );
    virtual ~CBTAudioStreamInputAdaptation();

    public: 
    /**
     * From CBTAudioStreamInputBase.
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
        IMPORT_C void BufferEmptied( const TDesC8& aBuffer );

    /**
     * From CBTAudioStreamInputBase.
     * This method is for connecting to the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
        IMPORT_C TInt Connect();

    /**
     * From CBTAudioStreamInputBase.
     * This method is for disconnecting from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return void
     */
        IMPORT_C void Disconnect();

    /**
     * From CBTAudioStreamInputBase.
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
        IMPORT_C TAny* EncoderInterface(TUid aInterfaceId);

    /**
     * From CBTAudioStreamInputBase.
     * This method is for choosing the encoder for the audio data. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param aDesiredFourCC specifies the encoder that the client wants to use. 
     * @return a Symbian OS wide error code. 
     */
        IMPORT_C TInt SetFormat(TFourCC aDesiredFourCC);

    /**
     * From CBTAudioStreamInputBase.
     * This method is for starting the recording from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
        IMPORT_C TInt Start();

    /**
     * From CBTAudioStreamInputBase.
     * This method is for stopping the recording from the audio subsystem. 
     *
     * See BT Audio Adaptation API and BT Audio Adaptation Design specifications 
     * for detailed explanation. 
     *
     * @since S60 v3.1
     * @param void. 
     * @return a Symbian OS wide error code. 
     */
        IMPORT_C void Stop();

    private:
        CBTAudioStreamInputAdaptation(MBTAudioStreamObserver& aStreamObserver, MBTAudioErrorObserver& aErrorObserver);
        void ConstructL();
        void RunL(); 
        void DoCancel();
   }; 

#endif 
