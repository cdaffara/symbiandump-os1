/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
//desc: test LCleanedup is used to declare a local variable of a destructor of a common class
//option:
//date:2008-8-20 13:51:31
//author:pingorliu
//type: CT

class temp
{
	private:
               ~temp()
	       {
                static LCleanedup member; //check:LCleanedup,~temp
	       }
};