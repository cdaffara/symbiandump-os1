/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Implementation of VGU like interface. Basically every VGU API names but prefixed with do_ e.g. do_vguLine
*/

#ifndef VGUINTERNAL_H
#define VGUINTERNAL_H

#include <vgu.h>

/*
 * defines set of internal vgu functions that are 1:1 with the published vgu functions.
 * E.g. vguLine -> do_vguLine
 * 
 * These do_XXX fucntions are called from the libopenvgu.dll (which is a shim) to be processed here
 */

IMPORT_C VGUErrorCode do_vguLine(VGPath path, VGfloat x0, VGfloat y0, VGfloat x1, VGfloat y1);
IMPORT_C VGUErrorCode do_vguPolygon(VGPath path, const VGfloat * points, VGint count, VGboolean closed);
IMPORT_C VGUErrorCode do_vguRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height);
IMPORT_C VGUErrorCode do_vguRoundRect(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat arcWidth, VGfloat arcHeight);
IMPORT_C VGUErrorCode do_vguEllipse(VGPath path, VGfloat cx, VGfloat cy, VGfloat width, VGfloat height);
IMPORT_C VGUErrorCode do_vguArc(VGPath path, VGfloat x, VGfloat y, VGfloat width, VGfloat height, VGfloat startAngle, VGfloat angleExtent, VGUArcType arcType);
IMPORT_C VGUErrorCode do_vguComputeWarpQuadToSquare(VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix);
IMPORT_C VGUErrorCode do_vguComputeWarpSquareToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat * matrix);
IMPORT_C VGUErrorCode do_vguComputeWarpQuadToQuad(VGfloat dx0, VGfloat dy0, VGfloat dx1, VGfloat dy1, VGfloat dx2, VGfloat dy2, VGfloat dx3, VGfloat dy3, VGfloat sx0, VGfloat sy0, VGfloat sx1, VGfloat sy1, VGfloat sx2, VGfloat sy2, VGfloat sx3, VGfloat sy3, VGfloat * matrix);

#endif
