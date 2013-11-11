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

import java.util.Vector;

/**
 * This class specifies a table in CommDB which can contain any number of
 * table entries (TableEntry class), all based on a particular table
 * template (TableTemplate class)
 */
public class Table
{
    
    /**
     * Creates a new instance of Table
     */
    public Table(TableTemplate tableTemplate)
    {
        m_tableTemplate = tableTemplate;
        m_tableName     = new String("");
        m_tableEntries  = new Vector();
    }
    
    // Access Methods
    
    /**
     * Get the name of this table
     *
     * @return The name of this table
     */
    public String GetTableName()
    {
        return new String(m_tableName);
    }
    
    /**
     * Get the table template employed by this table
     *
     * @return The table template of this table
     */
    public TableTemplate GetTableTemplate()
    {
        return m_tableTemplate;
    }
    
    /**
     * Get the number of table entries in this table
     *
     * @return The number of table entris in this table
     */
    public int GetNumberOfTableEntries()
    {
        return m_tableEntries.size();
    }
    
    /**
     * Get a table entry given its index
     *
     * @param index The index for the table entry to access
     * @return The table entry accessed
     */
    public TableEntry GetTableEntry(int index)
    {
        TableEntry entry;
        try
        {
            entry = (TableEntry)m_tableEntries.elementAt(index);
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
            return null;
        }
        return entry;
    }
    
    /**
     * Get a table entry index given the entry
     *
     * @param entry The table entry
     * @return The index of the table entry, -1 if it does not exist
     */
    public int GetTableEntryIndex(TableEntry entry)
    {
        for(int i=0; i<m_tableEntries.size(); i++)
        {
            TableEntry testEntry = (TableEntry)m_tableEntries.elementAt(i);
            if(testEntry == entry)
                return i;
        }
        return -1;
    }
    
    /**
     * Get a table entry given the value of one of its parameters
     * If multiple table entries have the same value for the specified
     * parameter, the first entry encountered is returned.
     *
     * @param param The name of the parameter
     * @param value The value of the parameter
     */
    public TableEntry GetTableEntry(String param, String value)
    {
        for(int i=0; i<m_tableEntries.size(); i++)
        {
            TableEntry testEntry = (TableEntry)m_tableEntries.elementAt(i);
            String testValue = testEntry.GetParameterValue(param);
            if(value.equals(testValue))
                return testEntry;
        }
        return null;
    }
    
    /**
     * Export this table in XML format
     *
     * @return A string with this table in XML format
     */
    public String toXML()
    {
        String content = "<" + m_tableName + ">\n";
        
        for(int i=0; i<m_tableEntries.size(); i++)
        {
            TableEntry entry = (TableEntry)m_tableEntries.elementAt(i);
            content += entry.toXML();
        }
        
        return content + "</" + m_tableName + ">\n\n";
    }
    
    // Set Methods
    
    /**
     * Set the name of this table
     *
     * @param tableName The name of this table
     */
    public void SetTableName(String tableName)
    {
        m_tableName = new String(tableName);
    }
    
    /**
     * Create a new table entry and return it
     *
     * @return The created table entry
     */
    public TableEntry CreateTableEntry()
    {
        TableEntry entry = new TableEntry(m_tableTemplate);
        m_tableEntries.add(entry);
        return entry;
    }
    
    /**
     * Remove a table entry given its index
     *
     * @param index The index of the table entry to remove
     * @return True if the removal was successful, false if not
     */
    public boolean RemoveTableEntry(int index)
    {
        try
        {
            m_tableEntries.removeElementAt(index);
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
            return false;
        }
        return true;
    }
    
    /**
     * Remove all tables entries for this table
     */
    public void RemoveAllTableEntries()
    {
        m_tableEntries.clear();
    }
    
    // Data Members
    
    /**
     * The name of this table
     */
    private String m_tableName;
    
    /**
     * The array of table entries forming this table
     */
    private Vector m_tableEntries;
    
    /**
     * The table template used for this table
     */
    private TableTemplate m_tableTemplate;
}
