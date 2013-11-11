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

import javax.swing.border.BevelBorder;
import javax.swing.border.LineBorder;

import com.symbian.commdb.data.Database;

/**
 * This class defines a table which displays the
 * CommDB information
 */
public class InformationTable extends javax.swing.JFrame
{
    /**
     * Creates a new form InformationTable
     *
     * @param commdb The CommDB object to display the information
     * @param menu The main menu form which launched this form
     */
    public InformationTable(Database commdb, MainMenu menu)
    {
        m_commdb = commdb;
        m_menu   = menu;
        initComponents();
        
        m_menu.DisableInformationDisplayMenuItem();
    }
    
    /**
     * This method is called from within the constructor to
     * initialize the form.
     */
    private void initComponents()
    {
        // Memory allocation for the UI elements
        
        ui_symbianLogoIcon = new javax.swing.JLabel();
        ui_title = new javax.swing.JTextField();
        ui_closeButton = new javax.swing.JButton();
        ui_scrollingPanel = new javax.swing.JScrollPane();
        
        // Initial form settings
        
        getContentPane().setLayout(null);
        getContentPane().setBackground(ui_lightColor);
        
        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setTitle("CommDB Information");
        setBackground(ui_bkgColor);
        setFont(new java.awt.Font("Arial", 0, 12));
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
        ui_scrollingPanel.setFont(new java.awt.Font("Arial", 0, 12));
        getContentPane().add(ui_scrollingPanel, java.awt.BorderLayout.CENTER);
        ui_scrollingPanel.setBounds(10, 20, 600, 430);
        
        // The icon with the Symbian logo
        
        ui_symbianLogoIcon.setFont(new Font("Arial", 0, 12));
        ui_symbianLogoIcon.setIcon(new javax.swing.ImageIcon(getClass()
                .getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
        getContentPane().add(ui_symbianLogoIcon);
        ui_symbianLogoIcon.setBounds(480, 460, 120, 45);
        
        // The title of the frame
        
        ui_title.setBackground(ui_lightColor);
        ui_title.setFont(new Font("Arial", 1, 14));
        ui_title.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_title.setText("CommDB Information");
        ui_title.setBorder(new javax.swing.border.CompoundBorder());
        getContentPane().add(ui_title);
        ui_title.setBounds(450, 510, 160, 30);
        
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
        
        // Generate the information table
        
        CreateInformation();
        
        pack();
    }
    
    /**
     * It creates the information table for this form
     */
    public void CreateInformation()
    {
        ui_informationTable = new javax.swing.JTable();
        ui_informationTable.setModel(new InformationTableModel(m_commdb));
        ui_informationTable.setBackground(new java.awt.Color(250, 250, 250));
        ui_informationTable.setForeground(new java.awt.Color(0, 0, 150));
        ui_informationTable.setGridColor(new java.awt.Color(0, 0, 0));
        ui_informationTable.setFont(new Font("Arial", 1, 12));
        ui_informationTable.getTableHeader().setFont(new Font("Arial", 1, 12));
        ui_informationTable.setShowGrid(true);
        ui_informationTable.setBorder(new LineBorder(new Color(0, 0, 0)));
        ui_informationTable.getColumnModel().getColumn(1).setPreferredWidth(50);
        ui_scrollingPanel.setViewportView(ui_informationTable);
    }
    
    /**
     * Close the form
     */
    private void exitForm(java.awt.event.WindowEvent evt)
    {
        m_menu.EnableInformationDisplayMenuItem();
        dispose();
    }
    
    /**
     * This method is called when the "Close" button is clicked
     *
     * @param evt The generated event
     */
    private void CloseButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        m_menu.EnableInformationDisplayMenuItem();
        dispose();
    }
    
    // Data Members
    
    /**
     * The scrolling plane
     */
    private javax.swing.JScrollPane ui_scrollingPanel;
    
    /**
     * The Information table
     */
    private javax.swing.JTable ui_informationTable;
    
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
     * The CommDB database used for displaying the information
     */
    private Database m_commdb;
    
    /**
     * The CommDB main menu form which launched this form
     */
    private MainMenu m_menu;
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
