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
* Description: Observer class that will be implemented by the SUPL Framework.
*
*/


#ifndef EPOS_MPOSSUPLSTATUSINDICATOR_H
#define EPOS_MPOSSUPLSTATUSINDICATOR_H


#include <e32base.h>

// Forward Declaration

/**
 * The observer class that will be implemented by the SUPL Framework.
 * POS Message plug-in only makes use of this class to terminate the SUPL Session and
 * also to retrieve the version of SUPL Framework
 *              
 *  @since Series 60 3.1u
 */
 
 
class MOMASuplObserver
    {
    
public: // New Functions

   /**
     * Reports Version of SUPL Protocol supported by SUPL FW
     *
     * @return @p TReal
     */
   virtual TReal SuplVersion() = 0;
   
    /**
     * Terminates the SUPL Session in the SUPL FW. POS Message
     * plug-in invokes this method to terminate the SUPL Session
     *
     */
   virtual void TerminateSession() = 0;
   

    };


#endif // EPOS_MPOSSUPLSTATUSINDICATOR_H

// End of File
