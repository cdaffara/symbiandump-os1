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

import java.io.IOException;
import java.net.URL;

/**
 * This class defines a form which displays information
 * regarding the usage of this application
 */
public class Usage extends javax.swing.JFrame
{
    /**
     * Creates a new form Usage
     */
    public Usage(MainMenu menu)
    {
        m_menu = menu;
        m_menu.DisableUsageMenuItem();
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
        setTitle("Usage Information");
        setFont(new java.awt.Font("Arial", 0, 12));
        setBackground(ui_bkgColor);
        setResizable(false);
        
        addWindowListener(new java.awt.event.WindowAdapter()
        {
            public void windowClosing(java.awt.event.WindowEvent evt)
            {
                exitForm(evt);
            }
        });
        
        // The scrolling panel
        
        ui_scrollingPanel.setBackground(ui_bkgColor);
        ui_scrollingPanel.setViewportBorder(new javax.swing.border.BevelBorder(javax.swing.border.BevelBorder.LOWERED));
        ui_scrollingPanel.setFont(new java.awt.Font("Arial", 0, 12));
        getContentPane().add(ui_scrollingPanel, java.awt.BorderLayout.CENTER);
        ui_scrollingPanel.setBounds(10, 20, 600, 430);
        
        // The Symbian logo icon
        
        ui_symbianLogoIcon.setFont(new java.awt.Font("Arial", 0, 12));
        ui_symbianLogoIcon.setIcon(new javax.swing.ImageIcon(getClass().getResource("/com/symbian/commdb/various/SymbianLogo.gif")));
        getContentPane().add(ui_symbianLogoIcon);
        ui_symbianLogoIcon.setBounds(480, 460, 120, 45);
        
        // The title of this form
        
        ui_title.setBackground(ui_lightColor);
        ui_title.setFont(new java.awt.Font("Arial", 1, 14));
        ui_title.setHorizontalAlignment(javax.swing.JTextField.CENTER);
        ui_title.setText("Usage Information");
        ui_title.setBorder(new javax.swing.border.CompoundBorder());
        getContentPane().add(ui_title);
        ui_title.setBounds(460, 510, 160, 30);
        
        // The "Close" button
        
        ui_closeButton.setFont(new java.awt.Font("Arial", 1, 12));
        ui_closeButton.setBackground(ui_lightColor);
        ui_closeButton.setMnemonic('C');
        ui_closeButton.setText("Close");
        ui_closeButton.setToolTipText("Close this window");
        
        ui_closeButton.addActionListener(new java.awt.event.ActionListener()
        {
            public void actionPerformed(java.awt.event.ActionEvent evt)
            {
                CloseButtonActionPerformed(evt);
            }
        });
        
        getContentPane().add(ui_closeButton);
        ui_closeButton.setBounds(10, 480, 70, 30);
        
        // Display file "Usage.html" which contains information concerning the
        // use of this application
        
        DisplayUsageFile();
        
        pack();
    }
    
    /**
     * It displays the usage file on the screen
     */
    public void DisplayUsageFile()
    {
        URL urlUsageFile = getClass().getResource("/com/symbian/commdb/various/Usage.html");
        try
        {
            ui_usageText = new javax.swing.JEditorPane(urlUsageFile);
        }
        catch(IOException e)
        {
            return;
        }
        ui_usageText.setEditable(false);
        ui_scrollingPanel.setViewportView(ui_usageText);
    }
    
    /**
     * Close the form
     */
    private void exitForm(java.awt.event.WindowEvent evt)
    {
        m_menu.EnableUsageMenuItem();
        dispose();
    }
    
    /**
     * This method is called when the "Close" button is clicked
     *
     * @param evt The generated event
     */
    private void CloseButtonActionPerformed(java.awt.event.ActionEvent evt)
    {
        m_menu.EnableUsageMenuItem();
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
    private javax.swing.JEditorPane ui_usageText;
    
    /**
     * The CommDB main menu which launched this form
     */
    private MainMenu m_menu;
    
    // Constants
    
    /**
     * The background color
     */
    private final java.awt.Color ui_bkgColor = new java.awt.Color(220, 220, 220);
    
    /**
     * A light gray color for various UI elements
     */
    private final java.awt.Color ui_lightColor = new java.awt.Color(230, 230, 230);
}
