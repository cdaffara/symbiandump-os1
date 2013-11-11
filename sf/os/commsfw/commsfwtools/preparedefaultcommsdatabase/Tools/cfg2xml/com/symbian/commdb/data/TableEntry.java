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
 * This class represents a particular entry of a CommDB table and it contains
 * all the values for the parameters of this entry. Note that a table entry
 * depends on the definition of a table template (TableTemplate class) and
 * therefore an instance can only be created when a table template instance
 * has been created and supplied as a parameter to the constructor of this
 * class
 */
public class TableEntry
{
    /**
     * Creates a new instance of TableEntry
     *
     * @param tableTemplate The TableTemplate object which corresponds to this entry
     */
    public TableEntry(TableTemplate tableTemplate)
    {
        m_tableTemplate    = tableTemplate;
        int size           = tableTemplate.GetNumberOfParameters();
        m_tableEntryValues = new Vector(size);
        m_id               = 1;
        
        // Initialise the table entries to an empty string
        
        for(int i=0; i<size; i++)
            m_tableEntryValues.add("");
        
        // Initialise the operation string to "add"
        
        m_operation = new String("add");
    }
    
    // Access Methods
    
    /**
     * Get the qualified name of this table entry.
     * The qualified name has the following format:
     *
     * <TableTemplate>.<TableEntry>
     *
     * where
     *
     * <TableTemplate> is the name of the table template
     * for this table entry
     *
     * <TableEntry> is the value of a parameter "Name" of
     * this table entry. If no such parameter exists then
     * the name is set to "NoName".
     *
     * @return The qualified name of this table entry
     */
    public String GetQualifiedName()
    {
        String templateName = m_tableTemplate.GetTableTemplateName();
        String entryName    = GetParameterValue("Name");
        
        if(entryName.equals("FAILED"))
            entryName = "NoName";
        
        return templateName + "." + entryName;
    }
    
    /**
     * Get the CommDB_ID of this table entry. Note that this
     * makes sense only for entries created when a CommDB file
     * in the pre-XML format was imported.
     *
     * @return The CommDB_ID of this table entry
     */
    public int GetID()
    {
        return m_id;
    }
    
    /**
     * Get the table template of this table entry
     *
     * @return The table template of this table entry
     */
    public TableTemplate GetTableTemplate()
    {
        return m_tableTemplate;
    }
    
    /**
     * Get the number of parameters of this table entry
     *
     * @return The number of table entry parameters
     */
    public int GetNumberOfParameters()
    {
        return m_tableTemplate.GetNumberOfParameters();
    }
    
    /**
     * Get the qualified name of a parameter of this table entry
     * given its index. The qualified name has the following format:
     *
     * <TableName>.<TableEntryName>.<ParameterName>
     *
     * where
     *
     * <TableName> is the name of the table which contains
     * this table entry
     *
     * <TableEntryName> is the name of this table entry
     *
     * <ParameterName> is the name of the parameter
     *
     * @param index The index of the parameter
     * @return The qualified name of the parameter
     */
    public String GetQualifiedParameterName(int index)
    {
        return GetQualifiedName() + "." +
        m_tableTemplate.GetParameter(index);
    }
    /**
     * Get the value of a particular parameter given its index
     *
     * @param index The index of the parameter to find its value
     * @return The value of the parameter
     */
    public String GetParameterValue(int index)
    {
        String element = (String)m_tableEntryValues.elementAt(index);
        return new String(element);
    }
    
    /**
     * Get the value of a particular parameter given the name of the
     * parameter
     *
     * @param parameter The parameter to find its value
     * @return The value of the parameter or "FAILED" if not successful
     */
    public String GetParameterValue(String parameter)
    {
        int index = m_tableTemplate.GetParameterIndex(parameter);
        if(index != -1)
            return new String((String)m_tableEntryValues.elementAt(index));
        else
            return new String("FAILED");
    }
    
    /**
     * Get the name of a particular parameter given the index of the
     * parameter
     *
     * @param index The index of the parameter
     * @return The name of the parameter
     */
    public String GetParameterName(int index)
    {
        String name = m_tableTemplate.GetParameter(index);
        return new String(name);
    }
    
    /**
     * Get the type of a particular parameter given the index of the
     * parameter
     *
     * @param index The index of the parameter
     * @return The type of the parameter
     */
    public String GetParameterType(int index)
    {
        String type = m_tableTemplate.GetParameterType(index);
        return new String(type);
    }
    
    /**
     * Get the operation that CommDB must perform in order to
     * save this entry to the DBMS format
     */
    public String GetOperation()
    {
        return new String(m_operation);
    }
    
    /**
     * It checks whether all parameters which must always have a value
     * are empty. If that is the case it returns "false" else it returns
     * "true"
     *
     * @return True if all mandatory table parameters have values, else false
     */
    public boolean MandatoryParametersValid()
    {
        for(int i=0; i<m_tableEntryValues.size(); i++)
        {
            if(m_tableTemplate.GetParameterMustExist(i))
            {
                String value = (String)m_tableEntryValues.elementAt(i);
                if(value.equals(""))
                    return false;
            }
        }
        return true;
    }
    
    /**
     * Export this table entry in XML format
     *
     * @return The table entry as a String in XML format
     */
    public String toXML()
    {
        String name    = m_tableTemplate.GetTableTemplateName();
        String content = "    <" + name  +
        " operation=\"" +
        m_operation     +
        "\""
        + ">\n";
        
        for(int i=0; i<m_tableEntryValues.size(); i++)
        {
            String param  = m_tableTemplate.GetParameter(i);
            String start  = "        <" + param + ">";
            String value  = (String)m_tableEntryValues.elementAt(i);
            
            // Avoid saving a parameter which has no value
            
            if(value.equals(""))
                continue;
            
            if(value.indexOf('\r') != -1)
                continue;
            
            if(value.indexOf('\n') != -1)
                continue;
            
            // Ensure that the parameter value does not contain any illegal
            // XML characters. Replace them as required
            
            value = value.replaceAll("&", "&amp;");
            value = value.replaceAll("\"", "&quot;");
            value = value.replaceAll(">", "&gt;");
            value = value.replaceAll("<", "&lt;");
            value = value.replaceAll("'", "&apos;");
            
            String finish = "</" + param + ">\n";
            
            content = content + start + value + finish;
        }
        
        return content + "    </" + name + ">\n";
    }
    
    // Set Methods
    
    /**
     * Set the value of a particular parameter given its index
     *
     * @param index The index of the parameter to modify
     * @param value The value for the parameter
     * @return True if operation was successful, false if not
     */
    public boolean SetParameterValue(int index, String value)
    {
        if(index >= m_tableEntryValues.size())
        {
            return false;
        }
        else
        {
            m_tableEntryValues.set(index,new String(value));
            return true;
        }
    }
    
    /**
     * Set the CommDB_ID of this table entry. Note that this
     * makes sense only for entries created when a CommDB file
     * in the pre-XML format was imported.
     *
     * @param id The CommDB_ID for this table entry
     */
    public void SetID(int id)
    {
        m_id = id;
    }
    
    /**
     * Set the operation to use when saving this entry to
     * DBMS format
     *
     * @param operation The operation to use when saving to DBMS
     */
    public void SetOperation(String operation)
    {
        m_operation = new String(operation);
    }
    
    /**
     * Set the table entry link of a particular parameter given its index.
     * This operation is not performed when the parameter value does not
     * represent a table entry link.
     *
     * @param index The index of the parameter to modify
     * @param entry The table entry link for the parameter
     * @return True if operation was successful, false if not
     */
    public boolean SetParameterTableEntryLink(int index, TableEntry entry)
    {
        if(index >= m_tableEntryValues.size() || index < 0)
        {
            return false;
        }
        else if(!m_tableTemplate.GetIsTableEntryLink(index))
        {
            return false;
        }
        else
        {
            if(entry == null)
            {
                m_tableEntryValues.set(index,"");
            }
            else
            {
                m_tableEntryValues.set(index,entry.GetQualifiedName());
            }
            return true;
        }
    }
    
    /**
     * Set the value of a particular parameter given its name
     *
     * @param parameter The name of the parameter to modify
     * @param value The value for the parameter
     * @return True if operation was successful, false if not
     */
    public boolean SetParameterValue(String parameter, String value)
    {
        int index = m_tableTemplate.GetParameterIndex(parameter);
        if(index != -1)
            return SetParameterValue(index,value);
        else
            return false;
    }
    
    /**
     * Set the table entry link of a particular parameter given its name.
     * This operation is not performed when the parameter value does not
     * represent a table entry link
     *
     * @param parameter The name of the parameter to modify
     * @param entry The table entry link for the parameter
     * @return True if operation was successful, false if not
     */
    public boolean SetParameterTableEntryLink(String parameter, TableEntry entry)
    {
        int index = m_tableTemplate.GetParameterIndex(parameter);
        if(index != -1)
            return SetParameterTableEntryLink(index,entry);
        else
            return false;
    }
    
    // Data Members
    
    /**
     * The array of values for the parameters of this table entry
     */
    private Vector m_tableEntryValues;
    
    /**
     * The table template corresponding to this entry
     */
    private TableTemplate m_tableTemplate;
    
    /**
     * The operation that CommDB must perform when saving
     * this entry in DBMS format
     */
    private String m_operation;
    
    /**
     * The CommDB_ID of this entry. Note that this value is only
     * used when importing CommDB files in the old format since
     * new CommDB XML files do not use the concept of table entry
     * IDs.
     */
    private int m_id;
}
