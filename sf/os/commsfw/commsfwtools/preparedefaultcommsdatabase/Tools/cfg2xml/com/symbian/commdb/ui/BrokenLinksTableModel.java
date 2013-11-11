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

import java.util.Vector;

/**
 * This class defines the table model used for displaying the
 * table entry parameters which refer to table entries (links)
 * which no longer exist (broken)
 *
 * | Qualified Parameter Name  | Qualified Broken Table Entry Name |
 * |                           |                                   |
 *
 */
public class BrokenLinksTableModel extends javax.swing.table.AbstractTableModel
{
    /**
     * Creates a new instance of BrokenLinksTableModel
     *
     * @param brokenParams A vector with the qualified names of the
     * table entry parameters which have broken links
     * @param brokenLinks A vector with the qualified names of the
     * table entries which no longer exist
     */
    public BrokenLinksTableModel(Vector brokenParams, Vector brokenLinks)
    {
        m_brokenParams = brokenParams;
        m_brokenLinks  = brokenLinks;
    }
    
    // Access Methods (from AbstractTableModel)
    
    /**
     * Return the number of columns of the table
     *
     * @return The number of columns which in this case it is always 4
     */
    public int getColumnCount()
    {
        return 2;
    }
    
    /**
     * Return the number of rows of the table
     *
     * @return The number of rows of the table which is the number of
     *         properties of a particular CommDB table
     */
    public int getRowCount()
    {
        return m_brokenParams.size();
    }
    
    /**
     * Returns the value of a particular cell of the table
     *
     * @param rowIndex The index of the table row to access
     * @param columnIndex The index of the table column to access
     * @return The appropriate cell value
     */
    public Object getValueAt(int rowIndex, int columnIndex)
    {
        if(columnIndex == 0)
            return m_brokenParams.elementAt(rowIndex);
        else if(columnIndex == 1)
            return m_brokenLinks.elementAt(rowIndex);
        else
            return null;
    }
    
    /**
     * Returns the name of a particular column
     *
     * @param column The column index
     * @return The name of a particular column or an
     * empty string if this column does not exist
     */
    public String getColumnName(int column)
    {
        if(column == 0)
            return "Table Entry Parameter";
        else if(column == 1)
            return "Invalid Table Entry Link";
        else
            return "";
    }
    
    /**
     * Returns the column with a particular name
     *
     * @param columnName The name of the column to find
     * @return The index of the found column or -1 if not found
     */
    public int findColumn(String columnName)
    {
        if(columnName.equals("Table Entry Parameter"))
            return 0;
        else if(columnName.equals("Invalid Table Entry Link"))
            return 1;
        else
            return -1;
    }
    
    /**
     * Returns true if a particular cell is editable, else it returns false
     *
     * @param rowIndex The row index of the cell to check
     * @param columnIndex The column index of the cell to check
     * @return True if the cell is editable, else false. In this case, only the
     *         cells of columnIndex = 1 are editable
     */
    public boolean isCellEditable(int rowIndex, int columnIndex)
    {
        return false;
    }
    
    // Data Members
    
    /**
     * The vector which contains the qualified names of the table
     * entry parameters which have broken table entry link references
     */
    Vector m_brokenParams;
    
    /**
     * The vector which contains the qualified names of the table
     * entries which no longer exist resulting in broken links
     */
    Vector m_brokenLinks;
}
