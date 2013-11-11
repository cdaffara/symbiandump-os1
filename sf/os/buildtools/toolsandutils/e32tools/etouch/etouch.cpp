// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if defined(__MSVCDOTNET__) || defined (__TOOLS2__)
 #include <iostream>
 using namespace std;
#else //!__MSVCDOTNET__
 #include <iostream.h>
#endif //__MSVCDOTNET__

#if defined(__VC32__) || defined(__TOOLS2__)
 #include <sys\utime.h>
#else
 #include <utime.h>
#endif

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#pragma warning( disable : 4710 )	// 'fn': function not inlined
#endif // old MSVC

int main(int argc,char *argv[])
//
// Collect the filename from the command line
// and change its date/time stamp to the current date/time
//
	{

	if (argc!=2)
		{
		cout << "Syntax: etouch filename" << endl;
		return(1);
		}
	return(utime(argv[1],NULL)==(-1) ? 1 : 0);
	}
