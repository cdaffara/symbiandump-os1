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

import java.awt.Color;
import java.awt.Font;

import javax.swing.border.LineBorder;

import com.symbian.commdb.data.Table;
import com.symbian.commdb.data.TableEntry;
import com.symbian.commdb.data.TableTemplate;

/**
 * This form implements an editor for the entries
 * of a CommDB table
 */
public class TableEditor extends javax.swing.JFrame
{
    /**
     * Creates a new form TableEditor
     *
     * @param menu The CommDB main menu object which launches the editor
     * @param table The CommDB table to edit
     */
    public TableEditor(MainMenu menu, Table table)
    {
        initComponents();
        
        m_menu  = menu;
        m_table = table;
        ui_props = new javax.swing.JTable();
        m_entry = null;
        
        m_maxEntriesAllowed = m_table.GetTableTemplate()
                                     .GetMaxNumberOfEntriesAllowed();
        
        s_activeTableEditors++;
        
        String tableName = m_table.GetTableName();
        ui_tableTypeTextField.setText(tableName);
        
        m_menu.DisableTableMenuItem(tableName);
        
        // Disable some buttons if only one entry is allowed
        
        if(m_maxEntriesAllowed == 1)
        {
            // Fixes a defect where buttons are disabled even though there 
            // are zero entries in the table.
            if(m_table.GetNumberOfTableEntries() >= 1)
            {
            ui_previousButton.setEnabled(false);
            ui_nextButton.setEnabled(false);
            ui_addButton.setEnabled(false);
            }            
        }
    }
    
    /**
     * This method is called from within the constructor to
     * initialize the form.
     */
    private void initComponents()
    {
        // Memory allocation for the UI elements
        
        ui_propertiesList = new javax.swing.JScrollPane();
        ui_previousButton = new javax.swing.JButton();
        ui_nextButton = new javax.swing.JButton();
        ui_addButton = new javax.swing.JButton();
        ui_deleteButton = new javax.swing.JButton();
        ui_deleteAllButton = new javax.swing.JButton();
        ui_tableTypeTextField = new javax.swing.JTextField();
        ui_closeButton = new javax.swing.JButton();
        ui_symbianLogoIcon = new javax.swing.JLabel();
        ui_tableEntryCounter = new javax.swing.JTextField();
        ui_title = new javax.swing.JTextField();
        ui_firstButton = new javax.swing.JButton();
        ui_lastButton = new javax.swing.JButton();
        ui_tableEntryOperation = new javax.swing.JComboBox();
        ui_tableEntryOperationTextField = new javax.swing.JTextField();
        
        // Initial form settings
        
        getContentPane().setLayout(null);
        getContentPane().setBackground(ui_lightColor);
        
        setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
        setTitle("Table Editor");
        setBackground(ui_bkgColor);
        setFont(new java.awt.Font("Arial", 0, 12));
        setName("TableEditorFrame");
        setResizable(false);
        
        addWindowListener(
                new java.awt.event.WindowAdapter()
                    {
                        public void windowClosing(java.awt.event.WindowEvent evt)
                        {
                            exitForm(evt);
                        }
                    }
        );
        
        // Set the list of properties
        
        ui_propertiesList.setBackground(ui_bkgColor);
        ui_propertiesList.setFont(new Font("Arial", 0, 12));
        getContentPane().add(ui_propertiesList);
        ui_propertiesList.setBounds(10, 80, 690, 370);
        
        // The "Previous" button
        
        ui_previousButton.setFont(new Font("Arial", 1, 12));
        ui_previousButton.setBackground(ui_lightColor);
        ui_previousButton.setMnemonic('P');
        ui_previousButton.setText("Previous");
        ui_previousButton.setToolTipText("Go to the previous table entry");
        
        ui_previousButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            PreviousButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_previousButton);
        ui_previousButton.setBounds(10, 5, 85, 30);
        
        // The "First" button
        
        ui_firstButton.setFont(new Font("Arial", 1, 12));
        ui_firstButton.setBackground(ui_lightColor);
        ui_firstButton.setMnemonic('F');
        ui_firstButton.setText("First");
        ui_firstButton.setToolTipText("Go to the first table entry");
        
        ui_firstButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            FirstButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_firstButton);
        ui_firstButton.setBounds(10, 40, 85, 30);
        
        // The "Next" button
        
        ui_nextButton.setFont(new Font("Arial", 1, 12));
        ui_nextButton.setBackground(ui_lightColor);
        ui_nextButton.setMnemonic('N');
        ui_nextButton.setText("Next");
        ui_nextButton.setToolTipText("Go to the next table entry");
        
        ui_nextButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            NextButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_nextButton);
        ui_nextButton.setBounds(640, 5, 60, 30);
        
        // The "Add" button
        
        ui_addButton.setFont(new Font("Arial", 1, 12));
        ui_addButton.setBackground(ui_lightColor);
        ui_addButton.setMnemonic('A');
        ui_addButton.setText("Add");
        ui_addButton.setToolTipText("Add a new table entry");
        
        ui_addButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            AddButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_addButton);
        ui_addButton.setBounds(560, 5, 60, 30);
        
        // The "Last" button
        
        ui_lastButton.setFont(new Font("Arial", 1, 12));
        ui_lastButton.setBackground(ui_lightColor);
        ui_lastButton.setMnemonic('L');
        ui_lastButton.setText("Last");
        ui_lastButton.setToolTipText("Go to the last table entry");
        
        ui_lastButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            LastButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_lastButton);
        ui_lastButton.setBounds(640, 40, 60, 30);
        
        // The "Delete" button
        
        ui_deleteButton.setFont(new Font("Arial", 1, 12));
        ui_deleteButton.setBackground(ui_lightColor);
        ui_deleteButton.setMnemonic('D');
        ui_deleteButton.setText("Delete");
        ui_deleteButton.setToolTipText("Delete this entry");
        
        ui_deleteButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            DeleteButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_deleteButton);
        ui_deleteButton.setBounds(100, 5, 70, 30);
        
        // The "Close" button
        
        ui_closeButton.setFont(new Font("Arial", 1, 12));
        ui_closeButton.setBackground(ui_lightColor);
        ui_closeButton.setMnemonic('C');
        ui_closeButton.setText("Close");
        ui_closeButton.setToolTipText("Close and save this table");
        
        ui_closeButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            CloseButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_closeButton);
        ui_closeButton.setBounds(10, 480, 70, 30);
        
        // The "Delete All" button
        
        ui_deleteAllButton.setFont(new Font("Arial", 1, 12));
        ui_deleteAllButton.setBackground(ui_lightColor);
        ui_deleteAllButton.setMnemonic('t');
        ui_deleteAllButton.setText("Delete All");
        ui_deleteAllButton.setToolTipText("Delete all table entries and close this form");
        
        ui_deleteAllButton.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            DeleteAllButtonActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_deleteAllButton);
        ui_deleteAllButton.setBounds(95, 480, 90, 30);
        
        // The Operation selection field
        
        ui_tableEntryOperation.setFont(new Font("Arial", 1, 12));
        ui_tableEntryOperation.setBackground(ui_lightColor);
        ui_tableEntryOperation.setEditable(false);
        ui_tableEntryOperation.setEnabled(true);
        ui_tableEntryOperation.addItem("add");
        ui_tableEntryOperation.addItem("template");
        
        ui_tableEntryOperation.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            TableEntryOperationActionPerformed(evt);
                        }
                    }
        );
        
        getContentPane().add(ui_tableEntryOperation);
        ui_tableEntryOperation.setBounds(300, 485, 90, 24);
        
        // The "DBMS Writing Operation" label
        
        ui_tableEntryOperationTextField.setFont(new Font("Arial", 1, 12));
        ui_tableEntryOperationTextField.setBackground(ui_lightColor);
        ui_tableEntryOperationTextField.setEditable(false);
        ui_tableEntryOperationTextField.setEnabled(true);
        ui_tableEntryOperationTextField.setText("DBMS Writing Operation");
        ui_tableEntryOperationTextField.setToolTipText("The type of DBMS writing operation to use");
        getContentPane().add(ui_tableEntryOperationTextField);
        ui_tableEntryOperationTextField.setBounds(300, 460, 145, 22);
        
        // The text field which displays the name of the edited CommDB table
        
        ui_tableTypeTextField.setBackground(new Color(242, 230, 230));
        ui_tableTypeTextField.setEditable(false);
        ui_tableTypeTextField.setFont(new Font("Arial", 1, 12));
        ui_tableTypeTextField.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_tableTypeTextField.setText("Unknown");
        ui_tableTypeTextField.setToolTipText("The name of this CommDB table");
        ui_tableTypeTextField.setBorder(new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        getContentPane().add(ui_tableTypeTextField);
        ui_tableTypeTextField.setBounds(180, 10, 370, 30);
        
        // The icon with the Symbian logo
        
        ui_symbianLogoIcon.setFont(new Font("Arial", 0, 12));
        ui_symbianLogoIcon.setIcon(new javax.swing.ImageIcon(getClass()
                .getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
        getContentPane().add(ui_symbianLogoIcon);
        ui_symbianLogoIcon.setBounds(580, 460, 120, 45);
        
        // The title of this form
        
        ui_title.setBackground(ui_lightColor);
        ui_title.setFont(new Font("Arial", 1, 14));
        ui_title.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_title.setText("CommDB Table Editor");
        ui_title.setBorder(new javax.swing.border.CompoundBorder());
        getContentPane().add(ui_title);
        ui_title.setBounds(545, 510, 160, 30);
        
        // The text field which displays the index of the table entry currently being edited
        
        ui_tableEntryCounter.setBackground(new Color(242, 230, 230));
        ui_tableEntryCounter.setEditable(false);
        ui_tableEntryCounter.setFont(new Font("Arial", 1, 12));
        ui_tableEntryCounter.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_tableEntryCounter.setText("0 / 0");
        ui_tableEntryCounter.setToolTipText("Current table entry index / Total number of table entries");
        ui_tableEntryCounter.setBorder(new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        getContentPane().add(ui_tableEntryCounter);
        ui_tableEntryCounter.setBounds(280, 50, 180, 19);
        
        pack();
    }
    
    /**
     * This method sets the currently edited table entry
     *
     * @param entry The currently edited table entry
     */
    public void SetEditedTableEntry(TableEntry entry)
    {
        m_entry = entry;
    }
    
    /**
     * This method is class when the "Previous" button is clicked
     *
     * @param evt The generated event
     */
    private void PreviousButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry == null)
            return;
        
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        int index = m_table.GetTableEntryIndex(m_entry);
        m_entry = m_table.GetTableEntry(index-1);
        EditCurrentTableEntry();
    }
    
    /**
     * This method is class when the "First" button is clicked
     *
     * @param evt The generated event
     */
    private void FirstButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry == null)
            return;
        
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        m_entry = m_table.GetTableEntry(0);
        EditCurrentTableEntry();
    }
    
    /**
     * This method is class when the "Last" button is clicked
     *
     * @param evt The generated event
     */
    private void LastButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry == null)
            return;
        
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        m_entry = m_table.GetTableEntry(m_table.GetNumberOfTableEntries()-1);
        EditCurrentTableEntry();
    }
    
    /**
     * This method is called when the "Add" button is clicked
     *
     * @param evt The generated event
     */
    private void AddButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        m_entry = m_table.CreateTableEntry();
        EditCurrentTableEntry();
    }
    
    /**
     * This method is called when the "Next" button is clicked
     *
     * @param evt The generated event
     */
    private void NextButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry == null)
            return;
        
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        int index = m_table.GetTableEntryIndex(m_entry);
        m_entry = m_table.GetTableEntry(index+1);
        EditCurrentTableEntry();
    }
    
    /**
     * This method is called when the "Close" button is clicked
     *
     * @param evt The generated event
     */
    private void CloseButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry != null)
        {    
            // Check that all mandatory parameters from the previous
            // table entry have been resolved
        
            if(!MandatoryParametersValidation()) return;
        
            // Refresh the statistics
        
            m_menu.RefreshInformation();
        }
        
        // Enable some menu items
        
        if(s_activeTableEditors == 1)
            m_menu.EnableMenuItemsAfterEditing();
        
        s_activeTableEditors--;
        
        String tableName = m_table.GetTableName();
        m_menu.EnableTableMenuItem(tableName);
        
        dispose();
    }
    
    /**
     * This method is called when the "Delete All" button is clicked
     *
     * @param evt The generated event
     */
    private void DeleteAllButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        m_table.RemoveAllTableEntries();
        
        m_menu.RefreshInformation();
        if(s_activeTableEditors == 1)
            m_menu.EnableMenuItemsAfterEditing();
        
        s_activeTableEditors--;
        
        String tableName = m_table.GetTableName();
        m_menu.EnableTableMenuItem(tableName);
        
        dispose();
    }
    
    /**
     * This method is called when the "Delete" button is clicked
     *
     * @param evt The generated event
     */
    private void DeleteButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        if(m_entry == null)
            return;
        
        int index = m_table.GetTableEntryIndex(m_entry);
        m_table.RemoveTableEntry(index);
        int numEntries = m_table.GetNumberOfTableEntries();
        
        if(numEntries == 0)
        {
            m_menu.RefreshInformation();
            if(s_activeTableEditors == 1)
                m_menu.EnableMenuItemsAfterEditing();
            
            s_activeTableEditors--;
            
            String tableName = m_table.GetTableName();
            m_menu.EnableTableMenuItem(tableName);
            
            dispose();
        }
        else
        {
            if(index == numEntries)
                index--;
            
            m_entry = m_table.GetTableEntry(index);
            EditCurrentTableEntry();
        }
    }
    
    /**
     * This method is called when the "Operation" box is changed
     *
     * @param evt The generated event
     */
    private void TableEntryOperationActionPerformed(java.awt.event.ActionEvent evt)
    {
        m_entry.SetOperation((String)ui_tableEntryOperation.getSelectedItem());
    }
    
    /**
     * Close the form
     */
    private void exitForm(java.awt.event.WindowEvent evt)
    {
        // Check that all mandatory parameters from the previous
        // table entry have been resolved
        
        if(!MandatoryParametersValidation()) return;
        
        // Refresh the statistics
        
        m_menu.RefreshInformation();
        
        // Enable some menu items
        
        if(s_activeTableEditors == 1)
            m_menu.EnableMenuItemsAfterEditing();
        
        s_activeTableEditors--;
        
        String tableName = m_table.GetTableName();
        m_menu.EnableTableMenuItem(tableName);
        
        dispose();
    }
    
    // Set Methods
    
    /**
     * This method is used in order to edit the currently selected
     * table entry
     */
    public void EditCurrentTableEntry()
    {
        // Check whether the entry to edit is null
        
        if(m_entry == null)
            return;
       
        int numCurrent = m_table.GetTableEntryIndex(m_entry) + 1;
        int numEntries = m_table.GetNumberOfTableEntries();
        
        // Check to see if more entries can be entered after this. If not
        // then disable the "Add" button
        
        if(numEntries == m_maxEntriesAllowed)
            ui_addButton.setEnabled(false);
        else
            ui_addButton.setEnabled(true);
        
        // If this is the last entry then disable the "Next" button
        
        if(numCurrent == numEntries)
            ui_nextButton.setEnabled(false);
        else
            ui_nextButton.setEnabled(true);
        
        // If this is the first entry then disable the "Previous" button
        
        if(numCurrent == 1)
            ui_previousButton.setEnabled(false);
        else
            ui_previousButton.setEnabled(true);
        
        // Create the table
        
        ui_props = new javax.swing.JTable();
        
        // Create and set the table model
        
        PropertiesTableModel model = new PropertiesTableModel(m_entry);
        ui_props.setModel(model);
        
        // Create and set the table cell editor
        
        TableTemplate templ = m_entry.GetTableTemplate();
        PropertiesCellEditor propsCellEditor = new PropertiesCellEditor(m_menu.GetDatabase(),templ);
        ui_props.getColumnModel().getColumn(1).setCellEditor(propsCellEditor);
        
        // Set the cell renderer for the parameters column
        
        javax.swing.table.TableColumn col;
        col = ui_props.getColumnModel().getColumn(0);
        col.setCellRenderer(new PropertiesCellRenderer(templ));
        
        // Set the properties of this table
        
        ui_props.setBackground(new Color(250, 250, 250));
        ui_props.setForeground(new Color(0, 0, 150));
        ui_props.setGridColor(new Color(0, 0, 0));
        ui_props.setFont(new Font("Arial", 1, 12));
        ui_props.getTableHeader().setFont(new Font("Arial", 1, 12));
        ui_props.setBorder(new LineBorder(new Color(0, 0, 0)));
        ui_props.setShowGrid(true);
        
        // Set the initial size of the table columns
        
        ui_props.getColumnModel().getColumn(0).setPreferredWidth(100);
        ui_props.getColumnModel().getColumn(1).setPreferredWidth(250);
        ui_props.getColumnModel().getColumn(2).setPreferredWidth(50);
        
        // Add the created table to the current viewport
        
        ui_propertiesList.setViewportView(ui_props);
        ui_propertiesList.setHorizontalScrollBarPolicy(javax.swing.JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        ui_propertiesList.setVerticalScrollBarPolicy(javax.swing.JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        
        // Display the current number / total number of table entries
        
        ui_tableEntryCounter.setText(numCurrent + " / " + numEntries);
        
        // Display the operation for this table entry
        
        ui_tableEntryOperation.setSelectedItem(m_entry.GetOperation());
    }
    
    /**
     * It checks whether all mandatory table parameters have valid values. If not
     * it produces an error message to the screen and returns false. Else it
     * returns true. This validation is not applicable to the Information table
     *
     * @return True if all mandatory parameters have valid values, else false
     */
    private boolean MandatoryParametersValidation()
    {
        if(m_entry == null)
            return true;
        
        boolean check = m_entry.MandatoryParametersValid();
        
        if(check)
            return true;
        else
        {
            javax.swing.JOptionPane.showMessageDialog(this,
                "Some mandatory parameters have no values\n" +
                "Further editing of this table entry is required",
                "Mandatory Parameters Missing Error",
                javax.swing.JOptionPane.WARNING_MESSAGE);
            return false;
        }
    }
    
    // Data Members
    
    /**
     * The "Add" button
     */
    private javax.swing.JButton ui_addButton;
    
    /**
     * The "Close" button
     */
    private javax.swing.JButton ui_closeButton;
    
    /**
     * The "Delete" button
     */
    private javax.swing.JButton ui_deleteButton;
    
    /**
     * The "Delete All" button
     */
    private javax.swing.JButton ui_deleteAllButton;
    
    /**
     * The "Next" button
     */
    private javax.swing.JButton ui_nextButton;
    
    /**
     * The "Previous" button
     */
    private javax.swing.JButton ui_previousButton;
    
    /**
     * The "First" button
     */
    private javax.swing.JButton ui_firstButton;
    
    /**
     * The "Last" button
     */
    private javax.swing.JButton ui_lastButton;
    
    /**
     * The scrolling pane of the table
     */
    private javax.swing.JScrollPane ui_propertiesList;
    
    /**
     * The icon of the Symbian logo
     */
    private javax.swing.JLabel ui_symbianLogoIcon;
    
    /**
     * The text field which displays the current table entry index
     */
    private javax.swing.JTextField ui_tableEntryCounter;
    
    /**
     * The text field which displays the name of the current table
     */
    private javax.swing.JTextField ui_tableTypeTextField;
    
    /**
     * The title
     */
    private javax.swing.JTextField ui_title;
    
    /**
     * The type of operation to perform for storing
     * a specific table entry in CommDB in DBMS format
     */
    private javax.swing.JComboBox ui_tableEntryOperation;
    
    /**
     * The Java table editor
     */
    private javax.swing.JTable ui_props;
    
    /**
     * The title for the operation of a table entry
     */
    private javax.swing.JTextField ui_tableEntryOperationTextField;
    
    /**
     * The CommDB main menu
     */
    private MainMenu m_menu;
    
    /**
     * The table associated with this editor
     */
    private Table m_table;
    
    /**
     * The current Table entry under editing
     */
    private TableEntry m_entry;
    
    /**
     * The maximum number of table entries allowed
     */
    private int m_maxEntriesAllowed;
    
    /**
     * The number of Table editor forms currently active
     */
    private static int s_activeTableEditors = 0;
    
    // Constants
    
    /**
     * The background color
     */
    private final Color ui_bkgColor = new Color(220, 220, 220);
    
    /**
     * A light gray color for various UI elements
     */
    private final Color ui_lightColor = new Color(230, 230, 230);
}
