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

import com.symbian.commdb.data.TableEntry;

/**
 * This class defines the table model used for editing the
 * properties of any CommDB table. This model is shown
 * graphically below:
 *
 * | Parameter | Value     | Type |
 * |           |           |      |
 *
 */
public class PropertiesTableModel extends javax.swing.table.AbstractTableModel
{
    /**
     * Creates a new instance of PropertiesTableModel
     *
     * @param entry The table entry used for editing purposes
     */
    public PropertiesTableModel(TableEntry entry)
    {
        m_entry = entry;
    }
    
    // Access Methods (from AbstractTableModel)
    
    /**
     * Return the number of columns of the table
     *
     * @return The number of columns which in this case it is always 3
     */
    public int getColumnCount()
    {
        return 3;
    }
    
    /**
     * Return the number of rows of the table
     *
     * @return The number of rows of the table which is the number of
     *         properties of a particular CommDB table
     */
    public int getRowCount()
    {
        return m_entry.GetNumberOfParameters();
    }
    
    /**
     * Returns the value of a particular cell of the table
     *
     * @param rowIndex The index of the table row to access
     * @param columnIndex The index of the table column to access
     * @return If columnIndex = 0 then it returns the name of the CommDB
     *         table property
     *         if columnIndex = 1 it returns the value of the property
     *         if columnIndex = 2 it returns the type of the property
     */
    public Object getValueAt(int rowIndex, int columnIndex)
    {
        if(columnIndex == 0)
            return m_entry.GetParameterName(rowIndex);
        else if(columnIndex == 1)
            return m_entry.GetParameterValue(rowIndex);
        else if(columnIndex == 2)
            return m_entry.GetParameterType(rowIndex);
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
            return "Property";
        else if(column == 1)
            return "Value";
        else if(column == 2)
            return "Type";
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
        if(columnName.equals("Property"))
            return 0;
        else if(columnName.equals("Value"))
            return 1;
        else if(columnName.equals("Type"))
            return 2;
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
        if(columnIndex == 1)
            return true;
        else
            return false;
    }
    
    // Set Methods (from AbstractTable model)
    
    /**
     * Set the value of a particular table cell
     *
     * @param aValue The value to set
     * @param rowIndex The row index of the cell to modify
     * @param columnIndex The column index of the cell to modify
     */
    public void setValueAt(Object aValue, int rowIndex, int columnIndex)
    {
        if(columnIndex == 1)
        {
            // Check the type of the cell
            
            String type = m_entry.GetTableTemplate().GetParameterType(rowIndex);
            
            if(type.equals("TUint32"))
            {
                String value = (String)aValue;
                int valInt;
                
                try
                {
                    valInt = Integer.parseInt(value);
                }
                catch(NumberFormatException e)
                {
                    valInt = 0;
                }
                
                if(valInt < 0)
                    valInt = 0;
                
                String valSet = (new Integer(valInt)).toString();
                m_entry.SetParameterValue(rowIndex,valSet);
            }
            else
                m_entry.SetParameterValue(rowIndex,(String)aValue);
            
            fireTableCellUpdated(rowIndex,columnIndex);
        }
    }
    
    // Data Members
    
    /**
     * The table entry used for editing purposes
     */
    TableEntry m_entry;
}
