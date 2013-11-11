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
* Description: Restricted Audio Output Api Implementation  
*
*/


#ifndef RESTRICTEDAUDIOOUTPUTIMPL_H
#define RESTRICTEDAUDIOOUTPUTIMPL_H


#include "RestrictedAudioOutputImpl.h"
#include <RestrictedAudioOutput.h>


/**
 * CRestrictedAudioOutputImpl class documentation.
 *
 * @lib RestrictedAudioOutput.lib
 *
 * @since S60 5.0 
 */
class CRestrictedAudioOutputImpl : public CBase
	{

public:
        
	// Constructors and destructor

    /**
     * Constructs, and returns a pointer to, a new CBody object.
     * Leaves on failure..
     * @return CBody* A pointer to newly created object.
     */
     static CRestrictedAudioOutputImpl* NewL();


    /**
     * Destructor
     *
     */
    ~CRestrictedAudioOutputImpl();

    /**
     * Appends an allowed output to the existing list.
     *
     * @param aOutput to be appended.
     * @return KErrNone if successful. KErrAlreadyExists if aOutput is already
     *  appended before. KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise any systemwide error code.
     */
    TInt AppendAllowedOutput( CRestrictedAudioOutput::TAllowedOutputPreference aOutput );

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
    TInt RemoveAllowedOutput( CRestrictedAudioOutput::TAllowedOutputPreference aOutput );

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
    TInt GetAllowedOutput( TInt aIndex, CRestrictedAudioOutput::TAllowedOutputPreference& aOutput  );

    /**
     * Gets the number of objects in the array.
     *
     * @param On successful return will contain the number of objects in
     * the list.
     *
     * @return KErrIncompleteInitialization if ConstructL is not called
     *  by the deriving class. Otherwise a system wide error code.
     */
    TInt GetAllowedOutputCount( TInt& aSize );

    /**
     * Empties the list, so that it is ready to be reused.
     *
     *
     * @return KErrNone if successful. KErrIncompleteInitialization if ConstructL
     *  is not called by the deriving class.
     */
    TInt Reset();

    /**
     * Applies settings cached locally and commits.
     *
     * @return KErrNone if successful.
     */
    TInt Commit();
    
    private:

        /**
        * C++ default constructor.
        */
        CRestrictedAudioOutputImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        static void ConstructL();
  
		TBool ExistsInArray(CRestrictedAudioOutput::TAllowedOutputPreference& aOutput);        
        
	// Data:
	

    private:    // Friend classes
           friend class CRestrictedAudioOutput;
      
		RArray<CRestrictedAudioOutput::TAllowedOutputPreference> iAllowedOutputPrefArray; 		

	};

#endif // RESITRCTEDAUDIOOUTPUTIMPL_H

// End of file
