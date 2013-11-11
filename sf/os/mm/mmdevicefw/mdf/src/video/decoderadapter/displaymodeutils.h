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

#ifndef DISPLAYMODEUTILS_H
#define DISPLAYMODEUTILS_H

#include <mmf/devvideo/devvideoconstants.h>
#include <gdi.h>

/*
Collection of utilities for converting display mode parameters
between formats.
*/
class TMMFDisplayModeUtils 
	{
public:	
	static TDisplayMode DisplayMode(TRgbFormat aRgbFormat);
	static TInt BytesPerPixel(TRgbFormat aRgbFormat);	
	};

#endif // DISPLAYMODEUTILS_H
