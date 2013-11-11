// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @prototype
*/

#ifndef SGCONST_H
#define SGCONST_H

#include <graphics/sgerror.h>


/**
@publishedPartner
@prototype
@deprecated

Identifier of the primary screen, which always exists.
*/
const TInt KSgScreenIdMain = 0;


/**
@publishedPartner
@prototype
@deprecated

Value that specifies, when passed in as screen identifier, that a resource can be
used on all screens.
*/
const TInt KSgScreenIdAny = -1;


/**
@publishedPartner
@prototype
@deprecated

Set of flags that specify how a graphics resource can be used in different rendering
pipelines.
*/
enum TSgUsageFlags
	{
	/** The resource cannot be used in any rendering pipeline. */
	ESgUsageNone = 0,
	/** The resource can be bound as source of DirectGDI rendering. */
	ESgUsageDirectGdiSource = 0x0001,
	/** The resource can be bound as target of DirectGDI rendering. */
	ESgUsageDirectGdiTarget = 0x0002,
	/** The resource can be bound as source of the composition engine. */
	ESgUsageCompositionSource = 0x0004,
	/** The resource can be bound as target of the composition engine. */
	ESgUsageCompositionTarget = 0x0008,
	/** The resource can be used directly as screen contents. */
	ESgUsageScreenSource = 0x0010,
	/** The resource can be bound as OpenGL ES 1.X 2D texture. */
	ESgUsageOpenGlesTexture2D = 0x0020,
	/** The resource can be bound as target of OpenGL ES 1.X rendering. */
	ESgUsageOpenGlesTarget = 0x0040,
	/** The resource can be bound as OpenVG image. */
	ESgUsageOpenVgImage = 0x0080,
	/** The resource can be bound as target of OpenVG rendering. */
	ESgUsageOpenVgTarget = 0x0100,
	/** The resource can be used as destination in calls to eglCopyBuffers(). */
	ESgUsageEglCopyBuffersTarget = 0x0200,
	/** The resource can be bound as OpenGL ES 2.0 2D texture. */
	ESgUsageOpenGles2Texture2D = 0x0400,
	/** The resource can be bound as target of OpenGL ES 2.0 rendering. */
	ESgUsageOpenGles2Target = 0x0800,
	/** The resource can be bound as source of window graphics context rendering. @see CWindowGc */
	ESgUsageWindowGcSource = 0x1000
	};


/**
@publishedPartner
@prototype
@deprecated

Components of TSgCpuAccess. This enumeration is not intended to be used directly, but
provides components for the definition of TSgCpuAccess.
*/
enum TSgCpuAccessComponents
	{
	/** Value of the bit in TSgCpuAccess that indicates ability to read when set. */
	ESgCpuAccessReadable = 1,
	/** Value of the bit in TSgCpuAccess that indicates ability to write when set. */
	ESgCpuAccessWritable = 2
	};


/**
@publishedPartner
@prototype
@deprecated

Enumeration used to specify how a graphics resource can be accessed by the CPU.
*/
enum TSgCpuAccess
	{
	/** The resource cannot be accessed by the CPU. */
	ESgCpuAccessNone = 0,
	/** The resource can only be read by the CPU. */
	ESgCpuAccessReadOnly = ESgCpuAccessReadable,
	/** The resource can only be written by the CPU. */
	ESgCpuAccessWriteOnly = ESgCpuAccessWritable,
	/** The resource can be both read and written by the CPU. */
	ESgCpuAccessReadWrite = ESgCpuAccessReadable | ESgCpuAccessWritable
	};


#endif // SGCONST_H
