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
 * This class defines a generic enumeration type composed of a number of
 * Strings to be used for describing the various enumeration types of the
 * CommDB table parameters
 */
public class EnumerationType
{
    /**
     * Creates a new instance of EnumerationType
     *
     * @param enumerationName The name of this enumeration
     */
    public EnumerationType(String enumerationName)
    {
        m_enumerationName  = enumerationName;
        m_enumeratedValues = new Vector();
        
        // For each enumeration add an empty string to
        // indicate a no-value option
        
        m_enumeratedValues.add("");
    }
    
    // Access Members
    
    /**
     * Get the number of enumerated values
     *
     * @return The number of enumerations
     */
    public int GetNumberEnumerations()
    {
        return m_enumeratedValues.size();
    }
    
    /**
     * Get the name of this enumeration
     *
     * @return The name of this enumeration
     */
    public String GetEnumerationName()
    {
        return new String(m_enumerationName);
    }
    
    /**
     * Get an enumerated value given its index
     *
     * @param index The index of the enumerated value
     * @return The value of the enumeration
     */
    public String GetEnumeratedValue(int index)
    {
        return (String)m_enumeratedValues.elementAt(index);
    }
    
    /**
     * Get the index of an enumerated value given the value itself
     *
     * @param value The value of the enumeration
     * @return The index of the enumeration
     */
    public int GetEnumerationIndex(String value)
    {
        return m_enumeratedValues.indexOf(value);
    }
    
    // Set Methods
    
    /**
     * Add a new enumeration
     *
     * @param value The value of the enumeration to add
     */
    public void AddEnumeration(String value)
    {
        m_enumeratedValues.add(value);
    }
    
    /**
     * Remove an enumeration given its value
     *
     * @param value The enumeration to remove
     */
    public void RemoveEnumeration(String value)
    {
        int index = GetEnumerationIndex(value);
        m_enumeratedValues.remove(index);
    }
    
    /**
     * Remove an enumeration given its index
     *
     * @param index The index of the enumeration to remove
     */
    public void RemoveEnumeration(int index)
    {
        m_enumeratedValues.remove(index);
    }
    
    /**
     * Remove all the enumerations
     */
    public void RemoveAllEnumerations()
    {
        m_enumeratedValues.clear();
    }
    
    /**
     * Create a JComboBox based on this enumeration
     *
     * @return A JComboBox based on this enumeration
     */
    public javax.swing.JComboBox CreateJComboBox()
    {
        javax.swing.JComboBox box = new javax.swing.JComboBox();
        
        box.setEditable(false);
        box.setEnabled(true);
        
        for(int i=0; i<m_enumeratedValues.size(); i++)
            box.addItem((String)m_enumeratedValues.elementAt(i));
        return box;
    }
    
    // Data Members
    
    /**
     * The list of enumerated values
     */
    private Vector m_enumeratedValues;
    
    /**
     * The name of this enumeration
     */
    private String m_enumerationName;
}
