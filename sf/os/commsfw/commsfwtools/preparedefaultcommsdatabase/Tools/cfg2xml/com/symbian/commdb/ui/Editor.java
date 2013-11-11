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

import com.symbian.commdb.data.Database;

/**
 * The CommDB Editor main program
 */
public class Editor
{
    /**
     * Main program
     *
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        // Create a CommDB database object
        
        Database commdb = new Database("");
        
        // Open the main menu window
        
        MainMenu menu = new MainMenu(commdb);
        menu.setBounds(50, 50, 500, 270);
        menu.show();
    }
}
