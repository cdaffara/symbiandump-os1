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

import com.symbian.commdb.data.TableTemplate;

/**
 * This class implements a colour cell renderer for the
 * CommDB table properties column
 */
public class PropertiesCellRenderer extends javax.swing.table.DefaultTableCellRenderer
{
    /**
     * Creates a new instance of PropertiesCellRenderer
     */
    public PropertiesCellRenderer(TableTemplate tableTemplate)
    {
        m_tableTemplate = tableTemplate;
    }
    
    /**
     * Overloading of the DefaultTableCellRenderer.setValue() member 
     * function to implement proper colouring of the cell text depending
     * on whether the particular CommDB table property is mandatory
     * or not
     *
     * @param value The string to print
     */
    public void setValue(Object value)
    {
        String s = (String)value;
        if(m_tableTemplate.GetParameterMustExist(s))
            setForeground(new java.awt.Color(150, 0, 0));
        else
            setForeground(new java.awt.Color(0, 0, 150));
        
        setText(s);
    }
    
    // Data Members
    
    /**
     * The table template used for editing purposes
     */
    TableTemplate m_tableTemplate;
}
