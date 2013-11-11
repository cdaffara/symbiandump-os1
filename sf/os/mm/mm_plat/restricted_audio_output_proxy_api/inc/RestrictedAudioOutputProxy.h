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
* Description:  Handles client side implementation.
*
*/


#ifndef CRESTRICTEDAUDIOOUTPUTPROXY_H
#define CRESTRICTEDAUDIOOUTPUTPROXY_H

#include <RestrictedAudioOutput.h>
#include <RestrictedAudioOutputMessageTypes.h>
#include <mmfcontrollerframework.h>
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include <sounddevice.h>



//class CRestrictedAudioOutput;
class CMMFDevSound;

/**
 * CRestrictedAudioOutputProxy class documentation.
 *
 * @lib RestrictedAudioOutputProxy.lib
 *
 * @since S60 5.0 
 */
class CRestrictedAudioOutputProxy: public CRestrictedAudioOutput
	{
	friend class CRestrictedAudioOutput;
    public:  
    

  	/**
     * Constructs, and returns a pointer to, a new 
     * CRestrictedAudioOutputProxy object.
     * Leaves on failure..
     * @param 
     * @return CRestrictedAudioOutputProxy* A pointer to newly created object.
     */    
    IMPORT_C static CRestrictedAudioOutputProxy* NewL( TMMFMessageDestinationPckg aMessageHandler,
                    						MCustomCommand& aCustomCommand,
                     						CCustomInterfaceUtility* aCustomInterfaceUtility );
                     						

    /** Constructs, and returns a pointer to, a new CRestrictedAudioOutput
     * object.
     * Leaves on failure..
     * @param 
     * @return CRestrictedAudioOutput* A pointer to newly created object.
     */
    IMPORT_C static CRestrictedAudioOutputProxy* NewL(CMMFDevSound& aDevSound);
    				
		
	/**
	 * Destructor.
	 */
	virtual ~CRestrictedAudioOutputProxy();

    /**
     * Returns class identifier to caller.
     * 
     *
     * @since S60 5.0
     * @param 
     * @return const TUid identifier
     */ 	
	IMPORT_C static const TUid Uid();
		

    /**
     * Commits chosen output values.
     * 
     *
     * @since S60 5.0
     * @param 
     * @return TInt error code.
     */ 	
	IMPORT_C TInt Commit();


	private:

		/**
		* Private C++ constructor for this class.
        * @since	5.0
        * @param aMessageHandler reference to message handler
        * @param aCustomCommand reference to custom command utility
        * @return	-
        */
		CRestrictedAudioOutputProxy(TMMFMessageDestinationPckg aMessageHandler, MCustomCommand& aCustomCommand,
					 CCustomInterfaceUtility* aCustomInterfaceUtility);


    private:    // Data
		MCustomCommand*                	   			iCustomCommand;
		TMMFMessageDestinationPckg 					iMessageHandler;
    	CCustomInterfaceUtility* 					iCustomInterfaceUtility;
		TAllowedOutputPreference          			iOutput;
		CRestrictedAudioOutput*						iAudioOutput;
		CArrayFixFlat<TAllowedOutputPreference>  	iArray;		
			

	};

#endif      // CRESTRICTEDAUDIOINPUTPROXY_H
            
// End of File
