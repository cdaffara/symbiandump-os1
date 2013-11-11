// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.ant;

import java.io.PrintStream;
import org.apache.tools.ant.DefaultLogger;

public class AbldWhatLogger extends DefaultLogger {
	
	protected void printMessage(String aMessage, PrintStream aPrintStream, int aPrioritary) {  
		if (aMessage.indexOf("[echo]") >= 0) {
			// remove "[echo]"
			aMessage = aMessage.replace("[echo]", "");
		} else {
			aMessage = "";
		}

		// remove drive letter
		aMessage = aMessage.replaceAll("[A-Z]:", "");

		super.printMessage(aMessage, aPrintStream, aPrioritary);
	}

}
