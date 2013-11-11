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
//desc: test class name identify with base-class-spec
//option:
//date:2009-1-12 11:0:58
//author:bolowyou
//type: CT
template<typename t, class tt>
class NS1  ::  MS2 :: myclass :virtual protected ( Myclass)
{ 
	LCleanedup mem; //check:myclass
};
