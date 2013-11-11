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

/**
 * This class implements a colour cell renderer for the
 * table which displays all broken table entry references
 */
public class BrokenLinksCellRenderer extends javax.swing.table.DefaultTableCellRenderer
{
    /**
     * Creates a new instance of BrokenLinksCellRenderer
     */
    public BrokenLinksCellRenderer()
    {
    }
    
    /**
     * Overloading of the DefaultTableCellRenderer.setValue() member
     * function to implement proper colouring of the cell text which
     * contains the qualified name of the parameter with the invalid
     * table entry name.
     *
     * @param value The string to print
     */
    public void setValue(Object value)
    {
        String s = (String)value;
        setForeground(new java.awt.Color(150, 0, 0));
        setText(s);
    }
}
