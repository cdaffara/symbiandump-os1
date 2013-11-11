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
import java.util.Vector;

import javax.swing.border.BevelBorder;
import javax.swing.border.LineBorder;

/**
 * This class defines a form which displays information
 * regarding the broken table entry links of the CommDB
 * database, if any.
 */
public class BrokenLinks extends javax.swing.JFrame
{
    /**
     * Creates a new form BrokenLinks
     */
    public BrokenLinks(MainMenu menu)
    {
        m_menu          = menu;
        m_brokenParams  = new Vector();
        m_brokenLinks   = new Vector();
        
        m_noBrokenLinks = m_menu.GetDatabase()
                          .FindBrokenTableEntryLinks(m_brokenParams, m_brokenLinks);
        
        m_menu.DisableCheckReferencesMenuItem();
        initComponents();
    }
    
    /**
     * This method is called from within the constructor to
     * initialize the form.
     */
    private void initComponents()
    {
        // Memory allocation for UI elements
        
        ui_symbianLogoIcon = new javax.swing.JLabel();
        ui_title = new javax.swing.JTextField();
        ui_closeButton = new javax.swing.JButton();
        ui_scrollingPanel = new javax.swing.JScrollPane();
        
        // Initial frame settings
        
        getContentPane().setLayout(null);
        getContentPane().setBackground(ui_lightColor);
        
        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setTitle("CommDB Table Entry References Check");
        setFont(new Font("Arial", 0, 12));
        setBackground(ui_bkgColor);
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
        
        // The scrolling panel
        
        ui_scrollingPanel.setBackground(ui_bkgColor);
        ui_scrollingPanel.setViewportBorder(new BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        ui_scrollingPanel.setFont(new Font("Arial", 0, 12));
        getContentPane().add(ui_scrollingPanel, java.awt.BorderLayout.CENTER);
        ui_scrollingPanel.setBounds(10, 20, 600, 430);
        
        // The Symbian logo icon
        
        ui_symbianLogoIcon.setFont(new Font("Arial", 0, 12));
        ui_symbianLogoIcon.setIcon(new javax.swing.ImageIcon(getClass()
                .getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
        getContentPane().add(ui_symbianLogoIcon);
        ui_symbianLogoIcon.setBounds(480, 460, 120, 45);
        
        // The title of this form
        
        ui_title.setBackground(ui_lightColor);
        ui_title.setFont(new Font("Arial", 1, 14));
        ui_title.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_title.setText("References Check");
        ui_title.setBorder(new javax.swing.border.CompoundBorder());
        getContentPane().add(ui_title);
        ui_title.setBounds(460, 510, 160, 30);
        
        // The "Close" button
        
        ui_closeButton.setFont(new Font("Arial", 1, 12));
        ui_closeButton.setBackground(ui_lightColor);
        ui_closeButton.setMnemonic('C');
        ui_closeButton.setText("Close");
        ui_closeButton.setToolTipText("Close this window");
        
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
        
        DisplayBrokenLinks();
        
        pack();
    }
    
    /**
     * It displays the broken links on the screen
     */
    public void DisplayBrokenLinks()
    {
        if(m_noBrokenLinks)
        {
            ui_brokenLinksText = new javax.swing.JEditorPane();
            ui_brokenLinksText.setEditable(false);
            ui_brokenLinksText.setFont(new Font("Arial", 0, 12));
            ui_scrollingPanel.setViewportView(ui_brokenLinksText);
            ui_brokenLinksText.setText("No broken table entry references were found");
        }
        else
        {
            ui_brokenLinksTable = new javax.swing.JTable();
            ui_brokenLinksTable.setModel(new BrokenLinksTableModel(m_brokenParams,m_brokenLinks));
            
            javax.swing.table.TableColumn col;
            col = ui_brokenLinksTable.getColumnModel().getColumn(0);
            col.setCellRenderer(new BrokenLinksCellRenderer());
            
            ui_brokenLinksTable.setBackground(new java.awt.Color(250, 250, 250));
            ui_brokenLinksTable.setForeground(new java.awt.Color(0, 0, 150));
            ui_brokenLinksTable.setGridColor(new java.awt.Color(0, 0, 0));
            ui_brokenLinksTable.setFont(new Font("Arial", 1, 12));
            ui_brokenLinksTable.getTableHeader().setFont(new Font("Arial", 1, 12));
            ui_brokenLinksTable.setShowGrid(true);
            ui_brokenLinksTable.setBorder(new LineBorder(new Color(0, 0, 0)));
            ui_brokenLinksTable.getColumnModel().getColumn(1).setPreferredWidth(50);
            ui_scrollingPanel.setViewportView(ui_brokenLinksTable);
        }
    }
    
    /**
     * Close the form
     */
    private void exitForm(java.awt.event.WindowEvent evt)
    {
        m_menu.EnableCheckReferencesMenuItem();
        dispose();
    }
    
    /**
     * This method is called when the "Close" button is clicked
     *
     * @param evt The generated event
     */
    private void CloseButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        m_menu.EnableCheckReferencesMenuItem();
        dispose();
    }
    
    // Data Members
    
    /**
     * The scrolling panel
     */
    private javax.swing.JScrollPane ui_scrollingPanel;
    
    /**
     * The icon with the Symbian logo
     */
    private javax.swing.JLabel ui_symbianLogoIcon;
    
    /**
     * The title of this form
     */
    private javax.swing.JTextField ui_title;
    
    /**
     * The "Close" button
     */
    private javax.swing.JButton ui_closeButton;
    
    /**
     * The editor pane
     */
    private javax.swing.JEditorPane ui_brokenLinksText;
    
    /**
     * The table which displays the broken links
     */
    private javax.swing.JTable ui_brokenLinksTable;
    
    /**
     * The CommDB main menu which launched this form
     */
    private MainMenu m_menu;
    
    /**
     * A vector with all table entry parameters which contains
     * broken links to table entries which no longer exist
     */
    private Vector m_brokenParams;
    
    /**
     * A vector with the names of the table entries which
     * no longer exist in the CommDB database because of
     * user modifications during editing
     */
    private Vector m_brokenLinks;
    
    /**
     * A flag which indicates whether broken links exist in the
     * database or not
     */
    private boolean m_noBrokenLinks;
    
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
