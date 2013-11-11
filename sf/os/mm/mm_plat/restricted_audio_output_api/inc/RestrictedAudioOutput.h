/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class handling restricted audio output
*
*/



#ifndef RESTRICTEDAUDIOOUTPUT_H
#define RESTRICTEDAUDIOOUTPUT_H

#include <e32base.h>

class CRestrictedAudioOutputImpl;

/**
 * CRestrictedAudioOutput class documentation.
 *
 * Use this class to control audio routing to enforce DRM restrictions.
 *
 * Instance of this class should be associated with an audio stream.
 * (Currently supported for DevSound API instance). Once associated, it is
 * valid and should be used only until the DevSound instance exist. Instance of
 * this class should be deleted before deleting the DevSound instance.
 *
 * The output restrictions specified via Commit() will be applied to audio devices
 * when DevSound instance enters playing state.
 *
 * @lib RestrictedAudioOutput.lib
 *
 * @since S60 5.0 
 */
class CRestrictedAudioOutput : public CBase
	{
public:
    enum TAllowedOutputPreference
        {
        EAllowAllAudio,
        EAllowAudioAnalog,
        EAllowAudioFMTransmitter,
        EAllowAudioBtHFPHSP,
        EAllowAudioBtA2DP,
        EAllowAudioUplink,
        EAllowAudioUsb,
        EAllowAudioRecording,
        EAllowVisualization,
        /**
		* RIM CR 417-7642: HDMI with HDCP to Resctricted Audio Output API
		* Adding new ENUMs to CRestrictedAudioOutput::TAllowedOutputPreference for HDMI and HDCP
		* EAllowAudioHDMI and EAllowAudioHdmiHdcpRequired
		*/
        EAllowAudioHDMI,
        EAllowAudioHdmiHdcpRequired
        };
        

    /**
     * Destructor
     *
     */
    IMPORT_C virtual ~CRestrictedAudioOutput();

    /**
     * Appends an allowed output to the existing list.
     *
     * @param aOutput to be appended.
     * @return KErrNone if successful. KErrAlreadyExists if aOutput is already
     *  appended before. KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise any systemwide error code.
     */
    IMPORT_C TInt AppendAllowedOutput( TAllowedOutputPreference aOutput );

    /**
     * Removes an allowed output from the existing list.
     *
     * @param The position within the array from where the object is to be
     * removed. The position is relative to zero, i.e. zero implies that an
     * object at the beginning of the array is to be removed.
     *
     * @return KErrNone if successful. KErrNotFound if aOutput is not in the
     *  the list. KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise any systemwide error code.
     */
    IMPORT_C TInt RemoveAllowedOutput( TAllowedOutputPreference aOutput );

    /**
     * Gets a reference to an allowed output located at a specified position
     * within the list.
     *
     * @param The position within the array from where the object is to be
     * removed. The position is relative to zero, i.e. zero implies that an
     * object at the beginning of the array is to be removed.
     *
     * @param The position within the array from where the object is to be
     * removed. The position is relative to zero, i.e. zero implies that an
     * object at the beginning of the array is to be removed.
     *
     * @param A const reference to the object at position anIndex within the
     * list.
     *
     * @return KErrNone if successful. KErrNotFound if index is not in the
     *  the list. KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise any systemwide error code.
     */
    IMPORT_C TInt GetAllowedOutput( TInt aIndex, TAllowedOutputPreference& aOutput  );

    /**
     * Gets the number of objects in the array.
     *
     * @param On successful return will contain the number of objects in
     * the list.
     *
     * @return KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise a system wide error code.
     */
    IMPORT_C TInt GetAllowedOutputCount( TInt& aSize );

    /**
     * Empties the list, so that it is ready to be reused.
     *
     *
     * @return KErrNone if successful. KErrIncompleteInitialization if ConstructL
     *  is not called by the deriving class.
     */
    IMPORT_C TInt Reset();

    /**
     * Applies settings cached locally and commits.
     *
     * This function can only be called when the DevSound instance is in Idle OR
     * Stopped state.
     *
     * Output preference will take into effect when DevSound instance goes to
     * Playing state.
     *
     * If the client commits an empty list, all other active audio output in 
     * the system will be muted.
     *
     * @return KErrNone if successful. KErrInUse if DevSound instance is not in 
     * idle/stopped state.
     */
    virtual TInt Commit() = 0;

protected:

    /**
     * Constructor
     *
     */
    IMPORT_C CRestrictedAudioOutput();
    
  	/**
     * Second phase constructor. The derived class should call this during
     * construction.
     *
     */
    IMPORT_C void ConstructL();  
  
            
    
private:
    	CRestrictedAudioOutputImpl* iImpl;
  
	};

#endif // RESTRICTEDAUDIOOUTPUT_H

// End of file
