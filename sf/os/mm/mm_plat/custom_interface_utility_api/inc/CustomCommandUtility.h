/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of custom command utility classes
*
*/


#ifndef __CUSTOMCOMMANDUTILITY_H
#define __CUSTOMCOMMANDUTILITY_H

#include <e32base.h>
#include <MCustomCommand.h>
//#include "CustomCommandTypes.h"


// FORWARD DECLARATION
class CMdaAudioConvertUtility;
class CMdaAudioPlayerUtility;
class CMdaAudioRecorderUtility;
class CMidiClientUtility;
class CDrmPlayerUtility;
class CVideoPlayerUtility;
class CVideoRecorderUtility;

// CLASS DECLARATION
class CCustomCommandUtility : public CBase,
                              public MCustomCommand
	{
public:
	
	IMPORT_C static CCustomCommandUtility* NewL(CMdaAudioConvertUtility&  aUtility);
	
	IMPORT_C static CCustomCommandUtility* NewL(CMdaAudioPlayerUtility&   aUtility);
	
	IMPORT_C static CCustomCommandUtility* NewL(CMdaAudioRecorderUtility& aUtility,
	                                            TBool aRecordStream);

	IMPORT_C static CCustomCommandUtility* NewL(CMidiClientUtility& aUtility);

	IMPORT_C static CCustomCommandUtility* NewL(CDrmPlayerUtility& aUtility);

	IMPORT_C static CCustomCommandUtility* NewL(CVideoPlayerUtility&   aUtility);

	IMPORT_C static CCustomCommandUtility* NewL(CVideoRecorderUtility&   aUtility);
	};

#endif
