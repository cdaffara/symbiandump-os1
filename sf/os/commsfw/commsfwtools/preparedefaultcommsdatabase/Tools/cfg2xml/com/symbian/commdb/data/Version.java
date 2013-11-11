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

package com.symbian.commdb.data;


/**
 * This class implements the version of the CommDB Editor
 * engine
 */
public class Version
{
    // Access Methods
    
    /**
     * Get the major version number of the CommDB Editor engine
     *
     * @return The major version number of the CommDB Editor engine
     */
    public static int GetMajorVersionNumber()
    {
        return m_majorVersionNumber;
    }
    
    /**
     * Get the minor version number of the CommDB Editor engine
     *
     * @return The minor version number of the CommDB Editor engine
     */
    public static int GetMinorVersionNumber()
    {
        return m_minorVersionNumber;
    }
    
    /**
     * Get the current status of the CommDB Editor engine
     *
     * @return The current status of the CommDB Editor engine
     */
    public static final String GetStatus()
    {
        return m_status;
    }
    
    /**
     * Get the date of the current build of the CommDB Editor engine
     *
     * @return The date of the current build of the CommDB Editor engine
     */
    public static final String GetBuildDate()
    {
        return m_buildDate;
    }
    
    /**
     * Get the version of the CommDB Editor engine
     *
     * @return The version of the CommDB Editor engine
     */
    public static String GetVersion()
    {
        return m_majorVersionNumber + "." +
               m_minorVersionNumber + " " +
               "[" + m_status + "]" +
               " Build " + m_buildDate;
    }
    
    // Data Members
    
    /**
     * The major version number of the CommDB Editor engine
     */
    private static final int m_majorVersionNumber = 1;
    
    /**
     * The minor version number of the CommDB Editor engine
     */
    private static final int m_minorVersionNumber = 6;
    
    /**
     * The current status of the CommDB Editor engine
     */
    private static final String m_status = "Release";
    
    /**
     * The date of the current build
     */
    private static final String m_buildDate = "20040205";
}

