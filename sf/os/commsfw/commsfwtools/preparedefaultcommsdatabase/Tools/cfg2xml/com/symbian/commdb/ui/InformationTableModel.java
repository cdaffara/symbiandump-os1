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

import com.symbian.commdb.data.Database;

/**
 * This class defines the table model used for displaying some
 * information of any CommDB database. This model is shown
 * graphically below:
 *
 * | Table   | Entries | Max. Entries | Schema |
 * |         |         |              |        |
 *
 */
public class InformationTableModel extends javax.swing.table.AbstractTableModel
{
    /**
     * Creates a new instance of InformationTableModel
     *
     * @param commdb The CommDB database to display
     */
    public InformationTableModel(Database commdb)
    {
        m_commdb = commdb;
    }
    
    // Access Methods (from AbstractTableModel)
    
    /**
     * Return the number of columns of the table
     *
     * @return The number of columns which in this case it is always 4
     */
    public int getColumnCount()
    {
        return 4;
    }
    
    /**
     * Return the number of rows of the table
     *
     * @return The number of rows of the table which is the number of
     *         properties of a particular CommDB table
     */
    public int getRowCount()
    {
        return m_commdb.GetNumberOfTableTemplates();
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
            return m_commdb.GetTableTemplateName(rowIndex);
        else if(columnIndex == 1)
            return new Integer(m_commdb.GetTable(rowIndex).GetNumberOfTableEntries());
        else if(columnIndex == 2)
        {
            int num = m_commdb.GetTable(rowIndex).GetTableTemplate().GetMaxNumberOfEntriesAllowed();
            if(num == Integer.MAX_VALUE)
                return new String("unbounded");
            else
                return new Integer(num);
        }
        else if(columnIndex == 3)
            return m_commdb.GetXMLSchemaFilename(rowIndex);
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
            return "Table";
        else if(column == 1)
            return "Entries";
        else if(column == 2)
            return "Max. Entries";
        else if(column == 3)
            return "Schema";
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
        if(columnName.equals("Table"))
            return 0;
        else if(columnName.equals("Entries"))
            return 1;
        else if(columnName.equals("Max. Entries"))
            return 2;
        else if(columnName.equals("Schema"))
            return 3;
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
     * The CommDB database to use for display purposes
     */
    Database m_commdb;
}
