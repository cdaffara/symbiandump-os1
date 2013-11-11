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

/**
 @file
 @test
*/

#ifndef __EGLTEST_INI_SETTINGS_H__
#define __EGLTEST_INI_SETTINGS_H__

#include <iniparser.h>
#include <e32base.h>
#include <pixelformats.h>
#include <VG/openvg.h>

// INI file name
_LIT(KConfigFileName,"z:\\egltest\\egltest_settings.ini");

// INI file sections for source formats and surface formats 
_LIT(KSectionImageSourceFormats,"ImageSourceFormats");
_LIT(KSectionPixmapSgSurfaceFormats,"PixmapSgSurfaceFormats");
_LIT(KSectionPixmapFbsSurfaceFormats,"PixmapFbsSurfaceFormats");
_LIT(KSectionPBufferSurfaceFormats,"PBufferSurfaceFormats");
_LIT(KSectionWindowSurfaceFormats,"WindowSurfaceFormats");
_LIT(KKeyNumFormats,"NumFormats");
_LIT(KKeyFormatX,"Format%d");

// INI file sections for EGL/VG/GLES extensions 
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
_LIT(KSectionQueryExtensions,"QueryString_Extensions");
#else
_LIT(KSectionQueryExtensions,"QueryString_Extensions_tb92");
#endif
_LIT(KKeyCountExtensionsEGL,"CountExtensionsEGL");
_LIT(KKeyCountExtensionsVG,"CountExtensionsVG");
_LIT(KKeyCountExtensionsGLES,"CountExtensionsGLES");
_LIT(KKeyExtensionEGLX,"ExtensionEGL%d");
_LIT(KKeyExtensionVGX,"ExtensionVG%d");
_LIT(KKeyExtensionGLESX,"ExtensionGLES%d");


NONSHARABLE_CLASS(CEglTestCommonIniSettings) : public CBase 
	{
public:
	IMPORT_C static CEglTestCommonIniSettings* NewL();
	IMPORT_C static CEglTestCommonIniSettings* NewLC();
	IMPORT_C ~CEglTestCommonIniSettings();	
	
	IMPORT_C VGImageFormat GetVgFormat(const TDesC& aSectioName, const TInt aWhich);	
	IMPORT_C TUidPixelFormat GetPixelFormat(const TDesC& aSectioName, const TInt aWhich);
	IMPORT_C TInt GetNumberOfFormats(const TDesC& aSectioName);
    IMPORT_C TInt GetNumberOfIterations(const TDesC& aSectioName);
    IMPORT_C TSize GetImageSize(const TDesC& aSectioName); 
    IMPORT_C TInt GetThresholdGPUUsedMemory(const TDesC& aSectioName);
    IMPORT_C TInt GetThresholdLastIteration(const TDesC& aSectioName);
    IMPORT_C TSize GetWindowSize(const TDesC& aSectioName); 
    
private:
	void ConstructL();
private:
	CIniData* iIniData;
	};

#endif
