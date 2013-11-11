// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.commdb.convert;

/**
 * This class defines the various termination codes for
 * this utility
 */
public class TerminationCodes
{
    /**
     * Succesful termination
     */
    public static final int SUCCESS                         = 0;
    
    /**
     * Error code: wrong or missing argument
     */
    public static final int ERROR_WRONG_ARGUMENT            = -1;
    
    /**
     * Error code: wrong number of command line arguments
     */
    public static final int ERROR_WRONG_NUMBER_OF_ARGUMENTS = -2;
    
    /**
     * Error code: the CommDB database cannot be created
     */
    public static final int ERROR_DATABASE_CREATION         = -3;
    
    /**
     * Error code: the CommDB database XML Schema files cannot
     * be imported
     */
    public static final int ERROR_DATABASE_IMPORT           = -4;
    
    /**
     * Error code: the conversion from CFG to XML cannot be
     * performed
     */
    public static final int ERROR_CONVERSION                = -5;
    
    /**
     * Error code: an I/O error occured
     */
    public static final int ERROR_IO                        = -6;
}
