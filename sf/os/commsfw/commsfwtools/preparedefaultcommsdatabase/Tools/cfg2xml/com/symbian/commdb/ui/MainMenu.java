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
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Vector;

import com.symbian.commdb.data.Database;
import com.symbian.commdb.data.DatabaseException;
import com.symbian.commdb.data.Table;
import com.symbian.commdb.data.TableEntry;
import com.symbian.commdb.data.Version;

/**
 * This class implements the main menu form of the CommDB Editor
 */
public class MainMenu extends javax.swing.JFrame
{
    /**
     * Creates a new form MainMenu
     *
     * @param commdb The CommDB database object that this menu manages
     */
    public MainMenu(Database commdb)
    {
        m_commdb = commdb;
        
        // Initialise the UI components
        
        initComponents();
        
        // Disable all buttons other than the one which allows the
        // import of the XML Schema files. The applications cannot
        // perform any operations without this action
        
        ui_tables.setEnabled(false);
        ui_importXMLMenuItem.setEnabled(false);
        ui_importCFGMenuItem.setEnabled(false);
        ui_exportMenuItem.setEnabled(false);
        ui_clearDatabaseMenuItem.setEnabled(false);
        ui_displayInformationMenuItem.setEnabled(false);
        ui_referencesMenuItem.setEnabled(false);
        m_canEditTables = false;
    }
    
    /**
     * This method is called from within the constructor to
     * initialize the form.
     */
    private void initComponents()
    {
        // Memory allocation for the UI elements
        
        ui_internalPanel = new javax.swing.JPanel();
        ui_symbianLogoIcon = new javax.swing.JLabel();
        ui_title = new javax.swing.JTextField();
        ui_menuBar = new javax.swing.JMenuBar();
        ui_actionsMenu = new javax.swing.JMenu();
        ui_displayInformationMenuItem = new javax.swing.JMenuItem();
        ui_importXMLMenuItem = new javax.swing.JMenuItem();
        ui_importCFGMenuItem = new javax.swing.JMenuItem();
        ui_exportMenuItem = new javax.swing.JMenuItem();
        ui_referencesMenuItem = new javax.swing.JMenuItem();
        ui_separator1MenuItem = new javax.swing.JSeparator();
        ui_clearDatabaseMenuItem = new javax.swing.JMenuItem();
        ui_setCommDBXMLSchemaMenuItem = new javax.swing.JMenuItem();
        ui_separator2MenuItem = new javax.swing.JSeparator();
        ui_quitMenuItem = new javax.swing.JMenuItem();
        ui_tables = new javax.swing.JMenu();
        ui_helpMenu = new javax.swing.JMenu();
        ui_aboutMenuItem = new javax.swing.JMenuItem();
        ui_usageMenuItem = new javax.swing.JMenuItem();
        ui_numberOfTablesLabel = new javax.swing.JLabel();
        ui_numberOfTablesTextField = new javax.swing.JTextField();
        ui_numberOfEntriesLabel = new javax.swing.JLabel();
        ui_numberOfEntriesTextField = new javax.swing.JTextField();
        ui_progressBar = new javax.swing.JProgressBar();
        ui_commdbTypeLabel = new javax.swing.JLabel();
        ui_commdbTypeTextField = new javax.swing.JTextField();
        
        // Initial form settings
        
        setTitle("Symbian CommDB Editor");
        setBackground(ui_lightColor);
        setFont(new Font("Arial", 0, 12));
        setName("Editor");
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
        
        ui_internalPanel.setLayout(null);
        ui_internalPanel.setBackground(ui_lightColor);
        
        // The icon with the Symbian logo
        
        ui_symbianLogoIcon.setFont(new Font("Arial", 0, 12));
        ui_symbianLogoIcon.setIcon(
        new javax.swing.ImageIcon(getClass()
                       .getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
        ui_internalPanel.add(ui_symbianLogoIcon);
        ui_symbianLogoIcon.setBounds(360, 110, 120, 50);
        
        // The title of this form
        
        ui_title.setBackground(ui_lightColor);
        ui_title.setEditable(false);
        ui_title.setFont(new Font("Arial", 1, 14));
        ui_title.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_title.setText("CommDB Editor");
        ui_title.setBorder(new javax.swing.border.EmptyBorder(
                           new java.awt.Insets(1, 1, 1, 1)));
        ui_internalPanel.add(ui_title);
        ui_title.setBounds(365, 160, 120, 30);
        
        // The "CommDB Type" label
        
        ui_commdbTypeLabel.setFont(new Font("Arial Narrow", 1, 14));
        ui_commdbTypeLabel.setText("CommDB Type");
        ui_commdbTypeLabel.setToolTipText("The type of this database");
        ui_internalPanel.add(ui_commdbTypeLabel);
        ui_commdbTypeLabel.setBounds(10, 30, 110, 20);
        
        // The "CommDB Type" text field
        
        ui_commdbTypeTextField.setFont(new Font("Arial Narrow", 1, 14));
        ui_commdbTypeTextField.setBackground(ui_bkgColor);
        ui_commdbTypeTextField.setBorder(
        new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        ui_commdbTypeTextField.setEditable(false);
        ui_commdbTypeTextField.setText("Unspecified");
        ui_commdbTypeTextField.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
        ui_internalPanel.add(ui_commdbTypeTextField);
        ui_commdbTypeTextField.setBounds(130, 30, 140, 20);
        
        // The "Number of Tables" label
        
        ui_numberOfTablesLabel.setFont(new Font("Arial Narrow", 1, 14));
        ui_numberOfTablesLabel.setText("Number of Tables");
        ui_numberOfTablesLabel.setToolTipText("Total number of tables in the database");
        ui_internalPanel.add(ui_numberOfTablesLabel);
        ui_numberOfTablesLabel.setBounds(10, 65, 110, 20);
        
        // The "Number of Table Entries" text field
        
        ui_numberOfTablesTextField.setFont(new Font("Arial Narrow", 1, 14));
        ui_numberOfTablesTextField.setBackground(ui_bkgColor);
        ui_numberOfTablesTextField.setBorder(
        new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        ui_numberOfTablesTextField.setEditable(false);
        ui_numberOfTablesTextField.setText("0");
        ui_numberOfTablesTextField.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
        ui_internalPanel.add(ui_numberOfTablesTextField);
        ui_numberOfTablesTextField.setBounds(130, 65, 140, 20);
        
        // The "Number of Table Entries" label
        
        ui_numberOfEntriesLabel.setFont(new Font("Arial Narrow", 1, 14));
        ui_numberOfEntriesLabel.setText("Number of Entries");
        ui_numberOfEntriesLabel.setToolTipText("Total number of table entries in the database");
        ui_internalPanel.add(ui_numberOfEntriesLabel);
        ui_numberOfEntriesLabel.setBounds(10, 100, 110, 20);
        
        // The "Number of Table Entries" text field
        
        ui_numberOfEntriesTextField.setFont(new Font("Arial Narrow", 1, 14));
        ui_numberOfEntriesTextField.setBackground(ui_bkgColor);
        ui_numberOfEntriesTextField.setBorder(
        new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        ui_numberOfEntriesTextField.setEditable(false);
        ui_numberOfEntriesTextField.setText("0");
        ui_numberOfEntriesTextField.setHorizontalAlignment(javax.swing.JTextField.RIGHT);
        ui_internalPanel.add(ui_numberOfEntriesTextField);
        ui_numberOfEntriesTextField.setBounds(130, 100, 140, 20);
        
        // The progress bar
        
        ui_progressBar.setBounds(10, 200, 480, 10);
        ui_internalPanel.add(ui_progressBar);
        
        getContentPane().add(ui_internalPanel, java.awt.BorderLayout.CENTER);
        
        // The main menu's bar
        
        ui_menuBar.setBorder(new javax.swing.border.BevelBorder(
                             javax.swing.border.BevelBorder.RAISED));
        ui_menuBar.setFont(new Font("Arial Narrow", 1, 14));
        ui_menuBar.setMaximumSize(new java.awt.Dimension(1000, 2));
        ui_menuBar.setMinimumSize(new java.awt.Dimension(1000, 2));
        
        // The "Actions" menu
        
        ui_actionsMenu.setMnemonic('A');
        ui_actionsMenu.setText("Actions");
        ui_actionsMenu.setToolTipText("Perform various actions");
        ui_actionsMenu.setFont(new Font("Arial Narrow", 1, 14));
        
        // The "Import XML File..." menu item
        
        ui_importXMLMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_I,
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_importXMLMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_importXMLMenuItem.setMnemonic('I');
        ui_importXMLMenuItem.setText("Import XML File...");
        ui_importXMLMenuItem.setToolTipText("Import an existing CommDB XML settings file");
        
        ui_importXMLMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            ImportMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_importXMLMenuItem);
        
        // The "Import CFG File..." menu item
        
        ui_importCFGMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_importCFGMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_importCFGMenuItem.setMnemonic('O');
        ui_importCFGMenuItem.setText("Import CFG File...");
        ui_importCFGMenuItem.setToolTipText("Import an existing CommDB settings file in the CFG format");
        
        ui_importCFGMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            ImportOldMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_importCFGMenuItem);
        
        // The "Export XML File..." menu item
        
        ui_exportMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_E, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_exportMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_exportMenuItem.setMnemonic('E');
        ui_exportMenuItem.setText("Export XML File...");
        ui_exportMenuItem.setToolTipText("Export the database to a CommDB XML settings file");
        
        ui_exportMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            ExportMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_exportMenuItem);
        
        // Add a separator
        
        ui_actionsMenu.add(ui_separator1MenuItem);
        
        // The "Display Information" menu item
        
        ui_displayInformationMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_D, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_displayInformationMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_displayInformationMenuItem.setMnemonic('D');
        ui_displayInformationMenuItem.setText("Display Information");
        ui_displayInformationMenuItem.setToolTipText("Displays database information");
        
        ui_displayInformationMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            DisplayInformationMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_displayInformationMenuItem);
        
        // The "Check Table Entry References" menu item
        
        ui_referencesMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_R, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_referencesMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_referencesMenuItem.setMnemonic('R');
        ui_referencesMenuItem.setText("Check Table Entry References");
        ui_referencesMenuItem.setToolTipText("Checks validity of table entry references");
        
        ui_referencesMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            CheckReferencesMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_referencesMenuItem);
        
        // The "Clear Database" menu item
        
        ui_clearDatabaseMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_L, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_clearDatabaseMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_clearDatabaseMenuItem.setMnemonic('l');
        ui_clearDatabaseMenuItem.setText("Clear Database");
        ui_clearDatabaseMenuItem.setToolTipText("Clear all table entries of the database");
        
        ui_clearDatabaseMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            ClearDatabaseMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_clearDatabaseMenuItem);
        
        // The "Set CommDB XML Schema" menu item
        
        ui_setCommDBXMLSchemaMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_setCommDBXMLSchemaMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_setCommDBXMLSchemaMenuItem.setMnemonic('S');
        ui_setCommDBXMLSchemaMenuItem.setText("Set CommDB XML Schema");
        ui_setCommDBXMLSchemaMenuItem.setToolTipText("Specify a specific structure for the CommDB tables by reading an XML Schema set of files");
        
        ui_setCommDBXMLSchemaMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            SetCommDBXMLSchemaMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_setCommDBXMLSchemaMenuItem);
        
        ui_actionsMenu.add(ui_separator2MenuItem);
        
        // The "Quit" menu item
        
        ui_quitMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Q, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_quitMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_quitMenuItem.setMnemonic('Q');
        ui_quitMenuItem.setText("Quit");
        ui_quitMenuItem.setToolTipText("Exit the CommDB Editor");
        
        ui_quitMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            QuitMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_actionsMenu.add(ui_quitMenuItem);
        
        ui_menuBar.add(ui_actionsMenu);
        
        // The "Tables" menu
        
        ui_tables.setMnemonic('T');
        ui_tables.setText("Tables");
        ui_tables.setToolTipText("Edit the CommDB settings tables");
        ui_tables.setFont(new Font("Arial Narrow", 1, 14));
        
        ui_menuBar.add(ui_tables);
        
        // The "Help" menu
        
        ui_helpMenu.setMnemonic('H');
        ui_helpMenu.setText("Help");
        ui_helpMenu.setToolTipText("Editor Help and Information");
        ui_helpMenu.setFont(new Font("Arial Narrow", 1, 14));
        
        // The "About" menu item
        
        ui_aboutMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_B, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_aboutMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_aboutMenuItem.setMnemonic('b');
        ui_aboutMenuItem.setText("About");
        
        ui_aboutMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            AboutMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_helpMenu.add(ui_aboutMenuItem);
        
        // The "Usage" menu item
        
        ui_usageMenuItem.setAccelerator(
        javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_U, 
                                           java.awt.event.InputEvent.CTRL_MASK));
        ui_usageMenuItem.setFont(new Font("Arial Narrow", 1, 14));
        ui_usageMenuItem.setMnemonic('U');
        ui_usageMenuItem.setText("Usage");
        
        ui_usageMenuItem.addActionListener(
                new java.awt.event.ActionListener()
                    {
                        public void actionPerformed(java.awt.event.ActionEvent evt)
                        {
                            UsageMenuItemActionPerformed(evt);
                        }
                    }
        );
        
        ui_helpMenu.add(ui_usageMenuItem);
        
        ui_menuBar.add(ui_helpMenu);

        setJMenuBar(ui_menuBar);
        
        pack();
    }
    
    /**
     * Return the CommDB database element
     */
    public Database GetDatabase()
    {
        return m_commdb;
    }
    
    /**
     * Refresh the CommDB statistics
     */
    public void RefreshInformation()
    {
        if(ui_infoTable != null)
            ui_infoTable.CreateInformation();
        
        ui_numberOfTablesTextField.setText(
            (new Integer(m_commdb.GetNumberOfTables()).toString()));
        ui_numberOfEntriesTextField.setText(
            (new Integer(m_commdb.GetNumberOfTableEntries()).toString()));
    }
    
    /**
     * This method is class when the "Display Statistics" menu item is selected
     *
     * @param evt The generated event
     */
    private void DisplayInformationMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        ui_infoTable = new InformationTable(m_commdb, this);
        ui_infoTable.setBounds(300, 150, 620, 580);
        ui_infoTable.show();
    }
    
    /**
     * This method is class when the "Usage" menu item is selected
     *
     * @param evt The generated event
     */
    private void UsageMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        ui_usage = new Usage(this);
        ui_usage.setBounds(300, 150, 620, 580);
        ui_usage.show();
    }
    
    /**
     * This method is class when the "Check Table Entry References" menu item is selected
     *
     * @param evt The generated event
     */
    private void CheckReferencesMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        ui_brokenLinks = new BrokenLinks(this);
        ui_brokenLinks.setBounds(300, 150, 620, 580);
        ui_brokenLinks.show();
    }
    
    /**
     * This method is class when the "Set CommDB XML Schema" menu item is selected
     *
     * @param evt The generated event
     */
    private void SetCommDBXMLSchemaMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {        
        ui_progressBar.setValue(0);

        // Get the directory which contains the CommDB XML Schema files
        
        javax.swing.JFileChooser fileChooser = new javax.swing.JFileChooser();
        fileChooser.rescanCurrentDirectory();
        fileChooser.setFileSelectionMode(javax.swing.JFileChooser.DIRECTORIES_ONLY);
        fileChooser.setDialogTitle("Set the directory with the CommDB XML files");
        fileChooser.setFont(new Font("Arial Narrow", 1, 14));
        
        int result = fileChooser.showDialog(this,"Select");
        File directory = (result == javax.swing.JFileChooser.APPROVE_OPTION) ?
                          fileChooser.getSelectedFile() : null;
        
        if(directory == null)
            return;
        
        // Create a new CommDB database
        
        String dirName = directory.getAbsolutePath() + "/";
        m_commdb = new Database(dirName);
         
        try
        {
            m_commdb.ReadContents();
        }
        catch(DatabaseException e)
        {
            // Notify the user that the import procedure failed
            
            javax.swing.JOptionPane.showMessageDialog(this,
                e.GetErrorMessage(),
                "Database Error",
                javax.swing.JOptionPane.ERROR_MESSAGE);
            return;
        }
        
        // Generate menu items of the "Tables" menu
        
        ui_tables.removeAll();
        
        for(int i=0; i<m_commdb.GetNumberOfTableTemplates(); i++)
        {
            javax.swing.JMenuItem tableMenuItem = new javax.swing.JMenuItem();
            
            tableMenuItem.setFont(new Font("Arial Narrow", 1, 14));
            final String tableName = m_commdb.GetTableName(i);
            tableMenuItem.setText(tableName);
            
            tableMenuItem.addActionListener(
                    new java.awt.event.ActionListener()
                        {
                            public void actionPerformed(java.awt.event.ActionEvent evt)
                            {
                                OpenTableEditor(tableName);
                            }
                        }
            );
            
            ui_tables.add(tableMenuItem);
        }
        
        // Import the parameters of the CommDB tables from the XML Schema file
        
        try
        {
            m_commdb.BuildTableTemplates(ui_progressBar);
        }
        catch(DatabaseException e)
        {
            // Notify the user that the import procedure failed
            
            javax.swing.JOptionPane.showMessageDialog(this,
                e.GetErrorMessage(),
                "Database Error",
                javax.swing.JOptionPane.ERROR_MESSAGE);
            return;
        }
        
        RefreshInformation();
        
        // Enable other components
        
        ui_tables.setEnabled(true);
        ui_importXMLMenuItem.setEnabled(true);
        ui_importCFGMenuItem.setEnabled(true);
        ui_exportMenuItem.setEnabled(true);
        ui_clearDatabaseMenuItem.setEnabled(true);
        ui_displayInformationMenuItem.setEnabled(true);
        ui_referencesMenuItem.setEnabled(true);
        
        m_canEditTables = true;
        
        // Display the type of the imported CommDB database
        
        ui_commdbTypeTextField.setText(m_commdb.GetDatabaseType());
    }
    
    /**
     * This method is class when the "Clear Database" menu item is selected
     *
     * @param evt The generated event
     */
    private void ClearDatabaseMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        // Confirm this action since it will result in the destruction of
        // all database entries
        
        int reply =
        javax.swing.JOptionPane.showConfirmDialog(this,
            "This action will erase all database entries!\n" +
            "Do you want to continue?",
            "Clear Database Confirmation",
            javax.swing.JOptionPane.YES_NO_OPTION);
        
        if(reply == javax.swing.JOptionPane.YES_OPTION)
        {
            m_commdb.EraseTableEntries();
            RefreshInformation();
        }
    }
    
    /**
     * This method is class when the "Export File..." menu item is selected
     *
     * @param evt The generated event
     */
    private void ExportMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        // Check the validity of the CommDB database before exporting i.e.
        // all table link references are valid
        
        Vector brokenParams = new Vector();
        Vector brokenLinks  = new Vector();
        boolean noBrokenLinks = m_commdb.FindBrokenTableEntryLinks(brokenParams,brokenLinks);
        
        if(!noBrokenLinks)
        {
            // Display an error message
            
            javax.swing.JOptionPane.showMessageDialog(this,
            "Invalid table entry references have been encountered\n",
            "Notification",
            javax.swing.JOptionPane.ERROR_MESSAGE);
            return;
        }
        
        // Create a Java file chooser
        
        javax.swing.JFileChooser fileChooser;
        fileChooser = new javax.swing.JFileChooser();
        fileChooser.setFileSelectionMode(javax.swing.JFileChooser.FILES_ONLY);
        
        ExtensionFilter xmlFilter = new ExtensionFilter(".xml", "XML Files (*.xml)");
        fileChooser.setAcceptAllFileFilterUsed(false);
        fileChooser.addChoosableFileFilter(xmlFilter);
        fileChooser.rescanCurrentDirectory();
        fileChooser.setFileFilter(xmlFilter);
        fileChooser.setDialogTitle("Export CommDB Settings File");
        fileChooser.setFont(new Font("Arial Narrow", 1, 14));
        
        String fileName = "CommDB.xml";
        File exportedFile = new File(fileChooser.getCurrentDirectory(),fileName);
        fileChooser.setSelectedFile(exportedFile);
        int result = fileChooser.showDialog(this,"Select");
        
        // Export the selected file
        
        exportedFile = (result == javax.swing.JFileChooser.APPROVE_OPTION) ? 
                       fileChooser.getSelectedFile() : null;
                       
        if(exportedFile != null)
        {
            try
            {
                m_commdb.ExportDatabase(exportedFile);
            }
            catch(FileNotFoundException e)
            {
                javax.swing.JOptionPane.showMessageDialog(this,
                    "The CommDB XML file was not properly exported\n"
                    + e.getMessage(),
                    "Notification",
                    javax.swing.JOptionPane.ERROR_MESSAGE);
            }
            catch(IOException e)
            {
                javax.swing.JOptionPane.showMessageDialog(this,
                    "The CommDB XML file was not properly exported\n"
                    + e.getMessage(),
                    "Notification",
                    javax.swing.JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    /**
     * This method is class when the "Quit" menu item is selected
     *
     * @param evt The generated event
     */
    private void QuitMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        System.exit(0);
    }
    
    /**
     * This method is class when the "Import XML File..." menu item is selected
     *
     * @param evt The generated event
     */
    private void ImportMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        // Create a Java file chooser
        
        javax.swing.JFileChooser fileChooser;
        fileChooser = new javax.swing.JFileChooser();
        fileChooser.setFileSelectionMode(javax.swing.JFileChooser.FILES_ONLY);
        
        ExtensionFilter xmlFilter = new ExtensionFilter(".xml", "XML Files (*.xml)");
        fileChooser.setAcceptAllFileFilterUsed(false);
        fileChooser.addChoosableFileFilter(xmlFilter);
        fileChooser.rescanCurrentDirectory();
        fileChooser.setFileFilter(xmlFilter);
        fileChooser.setDialogTitle("Import CommDB XML Settings File");
        fileChooser.setFont(new Font("Arial Narrow", 1, 14));
        int result = fileChooser.showDialog(this,"Select");
        
        // Import the selected file
        
        File importedFile = (result == javax.swing.JFileChooser.APPROVE_OPTION) ?
        fileChooser.getSelectedFile() : null;
        
        if(importedFile != null)
        {
            try
            {
                m_commdb.ImportDatabase(importedFile.getAbsolutePath());
                RefreshInformation();
            }
            catch(DatabaseException e)
            {
                javax.swing.JOptionPane.showMessageDialog(this,
                    "The CommDB XML file was not properly imported\n"
                    + e.getMessage(),
                    "Notification",
                    javax.swing.JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    /**
     * This method is class when the "Import CFG File..." menu item is selected
     *
     * @param evt The generated event
     */
    private void ImportOldMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        // Create a Java file chooser
        
        javax.swing.JFileChooser fileChooser;
        fileChooser = new javax.swing.JFileChooser();
        fileChooser.setFileSelectionMode(javax.swing.JFileChooser.FILES_ONLY);
        
        ExtensionFilter cfgFilter = new ExtensionFilter(".cfg", "CFG Files (*.cfg)");
        fileChooser.setAcceptAllFileFilterUsed(false);
        fileChooser.addChoosableFileFilter(cfgFilter);
        fileChooser.rescanCurrentDirectory();
        fileChooser.setFileFilter(cfgFilter);
        fileChooser.setDialogTitle("Import CommDB CFG Settings File");
        fileChooser.setFont(new Font("Arial Narrow", 1, 14));
        int result = fileChooser.showDialog(this,"Select");
        
        // Import the selected file
        
        File importedFile = (result == javax.swing.JFileChooser.APPROVE_OPTION) ?
        fileChooser.getSelectedFile() : null;
        
        if(importedFile != null)
        {
            try
            {
                m_commdb.ImportOldDatabase(importedFile.getAbsolutePath());
                RefreshInformation();
            }
            catch(DatabaseException e)
            {
                javax.swing.JOptionPane.showMessageDialog(this,
                    "The CommDB CFG file was not properly imported\n"
                    + e.GetErrorMessage(),
                    "Notification",
                    javax.swing.JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    /**
     * This method is called when the "About" menu item is selected
     *
     * @param evt The generated event
     */
    private void AboutMenuItemActionPerformed(java.awt.event.ActionEvent evt)
    {
        javax.swing.JOptionPane.showMessageDialog(this,
            "Symbian CommDB Editor\n" +
            "Version " + Version.GetVersion() + "\n" +
            "Copyright © 2003, Symbian Ltd.\n"  +
            "All Rights Reserved.",
            "About Symbian CommDB Editor",
            javax.swing.JOptionPane.INFORMATION_MESSAGE,
            new javax.swing.ImageIcon(
                getClass().getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
    }
    
    /**
     * Exit the main menu and the application
     */
    private void exitForm(java.awt.event.WindowEvent evt)
    {
        System.exit(0);
    }
    
    /**
     * It creates and opens a table editor for a particular CommDB table
     *
     * @param tableName The name of the table to generate
     */
    private void OpenTableEditor(String tableName)
    {
        if(!m_canEditTables)
            return;
        
        // Generate an editor for a table
        
        Table tab = m_commdb.GetTable(tableName);
        TableEditor editor = new TableEditor(this,tab);
        TableEntry entry = null;
        
        if(tab.GetNumberOfTableEntries() > 0)
            entry = tab.GetTableEntry(0);
        
        editor.SetEditedTableEntry(entry);
        editor.EditCurrentTableEntry();
        editor.setBounds(100, 150, 715, 580);
        editor.show();
        DisableMenuItemsDuringEditing();
    }
    
    /**
     * It disables the some menu items during the editing of a table
     */
    public void DisableMenuItemsDuringEditing()
    {
        ui_clearDatabaseMenuItem.setEnabled(false);
        ui_importXMLMenuItem.setEnabled(false);
        ui_importCFGMenuItem.setEnabled(false);
        ui_exportMenuItem.setEnabled(false);
        ui_setCommDBXMLSchemaMenuItem.setEnabled(false);
    }
    
    /**
     * It disables menu item "Usage"
     */
    public void DisableUsageMenuItem()
    {
        ui_usageMenuItem.setEnabled(false);
    }
    
    /**
     * It disables the "Display Information" menu item
     */
    public void DisableInformationDisplayMenuItem()
    {
        ui_displayInformationMenuItem.setEnabled(false);
    }
    
    /**
     * It disables the "Check Table Entry References" menu item
     */
    public void DisableCheckReferencesMenuItem()
    {
        ui_referencesMenuItem.setEnabled(false);
    }
    
    /**
     * It disables the "Check Table Entry References" menu item
     */
    public void DisableCheckReferencesnDisplayMenuItem()
    {
        ui_referencesMenuItem.setEnabled(false);
    }
    
    /**
     * It disables the menu item with the name of a particular table
     *
     * @param tableName The name of the table to disable its menu item
     */
    public void DisableTableMenuItem(String tableName)
    {
        int num = ui_tables.getItemCount();
        for(int i=0; i<num; i++)
        {
            javax.swing.JMenuItem item = ui_tables.getItem(i);
            if(item == null) continue;
            if(item.getText().equals(tableName))
            {
                item.setEnabled(false);
                break;
            }
        }
    }
    
    /**
     * It enables the menu item with the name of a particular table
     *
     * @param tableName The name of the table to enable its menu item
     */
    public void EnableTableMenuItem(String tableName)
    {
        int num = ui_tables.getItemCount();
        for(int i=0; i<num; i++)
        {
            javax.swing.JMenuItem item = ui_tables.getItem(i);
            if(item == null) continue;
            if(item.getText().equals(tableName))
            {
                item.setEnabled(true);
                break;
            }
        }
    }
    
    /**
     * It enables the "Display Information" menu item
     */
    public void EnableInformationDisplayMenuItem()
    {
        ui_displayInformationMenuItem.setEnabled(true);
    }
    
    /**
     * It enables the "Check Table Entry References" menu item
     */
    public void EnableCheckReferencesMenuItem()
    {
        ui_referencesMenuItem.setEnabled(true);
    }
    
    /**
     * It enables menu item "Usage"
     */
    public void EnableUsageMenuItem()
    {
        ui_usageMenuItem.setEnabled(true);
    }
    
    /**
     * It enables some menu items after the editing of a table
     * is completed
     */
    public void EnableMenuItemsAfterEditing()
    {
        ui_clearDatabaseMenuItem.setEnabled(true);
        ui_importXMLMenuItem.setEnabled(true);
        ui_importCFGMenuItem.setEnabled(true);
        ui_exportMenuItem.setEnabled(true);
        ui_setCommDBXMLSchemaMenuItem.setEnabled(true);
    }
    
    // Data Members
    
    /**
     * The "About" menu item
     */
    private javax.swing.JMenuItem ui_aboutMenuItem;
    
    /**
     * The "Actions" menu
     */
    private javax.swing.JMenu ui_actionsMenu;
    
    /**
     * The "Clear Database" menu item
     */
    private javax.swing.JMenuItem ui_clearDatabaseMenuItem;
    
    /**
     * The "Display Information" menu item
     */
    private javax.swing.JMenuItem ui_displayInformationMenuItem;
    
    /**
     * The "Quit" menu item
     */
    private javax.swing.JMenuItem ui_quitMenuItem;
    
    /**
     * The "Export XML File..." menu item
     */
    private javax.swing.JMenuItem ui_exportMenuItem;
    
    /**
     * The "Help" menu
     */
    private javax.swing.JMenu ui_helpMenu;
    
    /**
     * The "Import XML File..." menu item
     */
    private javax.swing.JMenuItem ui_importXMLMenuItem;
    
    /**
     * The "Import CFG File..." menu item
     */
    private javax.swing.JMenuItem ui_importCFGMenuItem;
    
    /**
     * The main menu's internal panel
     */
    private javax.swing.JPanel ui_internalPanel;
    
    /**
     * Menu separator 1
     */
    private javax.swing.JSeparator ui_separator1MenuItem;
    
    /**
     * Menu separator 2
     */
    private javax.swing.JSeparator ui_separator2MenuItem;
    
    /**
     * The "Set the CommDB XML Schema" menu item
     */
    private javax.swing.JMenuItem ui_setCommDBXMLSchemaMenuItem;
    
    /**
     * The "Usage" menu item
     */
    private javax.swing.JMenuItem ui_usageMenuItem;
    
    /**
     * The "Check Table Entry References" menu item
     */
    private javax.swing.JMenuItem ui_referencesMenuItem;
    
    /**
     * The "Tables" menu
     */
    private javax.swing.JMenu ui_tables;
    
    /**
     * The menu bar
     */
    private javax.swing.JMenuBar ui_menuBar;
    
    /**
     * The icon with the Symbian logo
     */
    private javax.swing.JLabel ui_symbianLogoIcon;
    
    /**
     * The title text field
     */
    private javax.swing.JTextField ui_title;
    
    /**
     * The "Number of Tables" label
     */
    private javax.swing.JLabel ui_numberOfTablesLabel;
    
    /**
     * The "Number of Tables" text field
     */
    private javax.swing.JTextField ui_numberOfTablesTextField;
    
    /**
     * The "CommDB Type" label
     */
    private javax.swing.JLabel ui_commdbTypeLabel;
    
    /**
     * The "CommDB Type" text field
     */
    private javax.swing.JTextField ui_commdbTypeTextField;
    
    /**
     * The "Number of Table Entries" label
     */
    private javax.swing.JLabel ui_numberOfEntriesLabel;
    
    /**
     * The "Number of Table Entries" text field
     */
    private javax.swing.JTextField ui_numberOfEntriesTextField;
    
    /**
     * The main menu's progress bar
     */
    private javax.swing.JProgressBar ui_progressBar;
    
    /**
     * The Information table element
     */
    private InformationTable ui_infoTable;
    
    /**
     * The Usage element
     */
    private Usage ui_usage;
    
    /**
     * The References element
     */
    private BrokenLinks ui_brokenLinks;
    
    /**
     * The CommDB database
     */
    private Database m_commdb;
    
    /**
     * If true then the editing of tables is disabled
     */
    private boolean m_canEditTables;
    
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
