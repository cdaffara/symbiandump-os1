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

import java.awt.Component;
import java.util.Vector;

import javax.swing.AbstractCellEditor;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.table.TableCellEditor;

import com.symbian.commdb.data.Database;
import com.symbian.commdb.data.EnumerationType;
import com.symbian.commdb.data.Table;
import com.symbian.commdb.data.TableEntry;
import com.symbian.commdb.data.TableTemplate;

/**
 * This class defines a cell table editor used for allowing the editing
 * of the parameter values of any CommDB table entry column
 */
public class PropertiesCellEditor extends AbstractCellEditor implements TableCellEditor
{
    /**
     * Creates a new instance of PropertiesCellEditor
     */
    public PropertiesCellEditor(Database commdb, TableTemplate tableTemplate)
    {
        m_commdb        = commdb;
        m_tableTemplate = tableTemplate;
        
        // Obtain the parameter enumeration type flags
        
        m_parameterFlags = tableTemplate.GetEnumerationTypeFlags();
    }
    
    // Methods (from interface TableCellEditor)
    
    /**
     * This method is called when a cell value is edited by the user.
     * It sets an initial value for the editor. This will cause the editor to
     * stopEditing and lose any partially edited value if the editor is editing
     * when this method is called. It Returns the component that should be
     * added to the client's Component hierarchy. Once installed in the client's
     * hierarchy this component will then be able to draw and receive user input.
     *
     * @param table The JTable that is asking the editor to edit; can be null
     * @param value The value of the cell to be edited; it is up to the specific
     *              editor to interpret and draw the value. For example, if
     *              value is the string "true", it could be rendered as a string
     *              or it could be rendered as a check box that is checked. null
     *              is a valid value
     * @param isSelected true if the cell is to be rendered with highlighting
     * @param rowIndex the row of the cell being edited
     * @param colIndex the column of the cell being edited
     * @return The component for editing
     */
    public Component getTableCellEditorComponent(JTable table, Object value,
    boolean isSelected,
    int rowIndex, int colIndex)
    {
        Boolean isEnumeration = (Boolean)m_parameterFlags.elementAt(rowIndex);
        String typeName       = m_tableTemplate.GetParameterType(rowIndex);
        
        // For entries which are existing table entry names of several tables
        // allow the user to select the appropriate one from a combo box list
        
        if(typeName.equals("TableEntryNameType"))
        {
            String parameterName = m_tableTemplate.GetParameter(rowIndex) + "Type";
            EnumerationType type = m_tableTemplate.GetEnumerationType(parameterName);
            ui_editComponent     = new javax.swing.JComboBox();
            
            ((JComboBox)ui_editComponent).setEditable(false);
            ((JComboBox)ui_editComponent).setEnabled(true);
            ((JComboBox)ui_editComponent).addItem("");
                        
            for(int k=1; k < type.GetNumberEnumerations(); k++)
            {
                String enumName   = type.GetEnumeratedValue(k);
                Table singleTable = m_commdb.GetTable(enumName + "Table");
   
                ((JComboBox)ui_editComponent).addItem(singleTable.GetTableName() + ".-1");
                
                for(int i=0; i < singleTable.GetNumberOfTableEntries(); i++)
                {
                    TableEntry tableEntry = singleTable.GetTableEntry(i);
                    ((JComboBox)ui_editComponent).addItem(tableEntry.GetQualifiedName());
                }
            }
            
            ((JComboBox)ui_editComponent).setSelectedItem(value);
        }
        
        // For enumerations allow the user to select the appropriate one
        // from a combo box list
        
        else if(isEnumeration.booleanValue())
        {
            EnumerationType type = m_tableTemplate.GetEnumerationType(typeName);
            ui_editComponent = type.CreateJComboBox();
            ((JComboBox)ui_editComponent).setSelectedItem(value);
        }
        
        // For text entries allow the user to write some text
        
        else
        {
            ui_editComponent = new JTextField();
            ((JTextField)ui_editComponent).setText((String)value);
        }
        
        return ui_editComponent;
    }
    
    /**
     * This method is called when editing is completed.
     * It must return the new value to be stored in the cell.
     *
     * @return The value contained in the editor
     */
    public Object getCellEditorValue()
    {
        // Find whether the type of the cell is an enumeration
        // or a simple one
        
        if(ui_editComponent instanceof JComboBox)
        {
            return ((JComboBox)ui_editComponent).getSelectedItem();
        }
        else
            return ((JTextField)ui_editComponent).getText();
    }
    
    /**
     * The CommDB database
     */
    private Database m_commdb;
    
    /**
     * A component used for eiditng purposes
     */
    Component ui_editComponent;
    
    /**
     * The table entry associated with this editor
     */
    TableTemplate m_tableTemplate;
    
    /**
     * The Vector of flags which determine whether the type of a particular
     * table parameter is an enumeration type (true) or a simple one (false)
     */
    Vector m_parameterFlags;
}
