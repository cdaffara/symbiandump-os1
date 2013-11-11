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

Makefiles provided by Nate Lawson (nate@rootlabs.com)

Import libraries for the cygwin compiler are in the lib directory (*.a)

To build, cd to the examples directory and type "make".  This will build
all the examples except netmeter.  Alternatively, you can type "make" in
each example you want to build.  To remove all objects and executables,
type "make clean".

TODO:
I didn't want to deal with the resource files for netmeter although cygwin
supports these.  Perhaps someone can build a makefile for the netmeter
example.
