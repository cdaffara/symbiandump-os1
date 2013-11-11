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

package com.symbian.commdb.ui;

import java.io.File;

/**
 * This is a general purpose file filter class to be used
 * by the editor to open files.
 */
public class ExtensionFilter extends javax.swing.filechooser.FileFilter
{
    /**
     * Constructor
     *
     * @param ext The extension of the files to filter
     * @param descr A description of the files to filter
     */
    public ExtensionFilter(String ext, String descr)
    {
        extension = ext.toLowerCase();
        description = descr;
    }
    
    /**
     * Criteria for accepting a particular file
     *
     * @param file The file to accept or not
     * @return True if the file is accepted, else it returns false
     */
    public boolean accept(File file)
    {
        return (file.isDirectory() || file.getName().toLowerCase().endsWith(extension));
    }
    
     /*
      * Get the description of the files filtered
      *
      * @return The description of the filtered files
      */
    public String getDescription()
    {
        return description;
    }
    
    // Data Members
    
    /**
     * The description of the filtered files
     */
    private String description;
    
    /**
     * The extension of the files this filter accepts
     */
    private String extension;
}
