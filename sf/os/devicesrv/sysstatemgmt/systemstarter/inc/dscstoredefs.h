// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DSCSTOREDEFS_H__
#define __DSCSTOREDEFS_H__

/** TDscPosition is an indication used for insert DscItem to DscDatabase
@publishedAll
@released
*/	
enum TDscPosition
		{
		/** Add item to the beginning of the DSC */
		EFirst=0,
		/** Add item to the end of the DSC */
		ELast=1
		};
			
/**This is the default DSC UID that Symbian provided so that licensees can provide 
the same UID for AMA developers independent of UI.
AMAs developer can use the default UID to add item to the database.
@publishedAll
@released
*/
#define KDefaultSymbianDsc 0x2000836D		

#endif // __DSCSTOREDEFS_H__

