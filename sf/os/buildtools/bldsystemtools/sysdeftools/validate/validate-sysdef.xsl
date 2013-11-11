<xsl:stylesheet  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!--Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of "Eclipse Public License v1.0"
	which accompanies this distribution, and is available
	at the URL "http://www.eclipse.org/legal/epl-v10.html".

	Initial Contributors:
	Nokia Corporation - initial contribution.
	Contributors:
	Description:
	Validate a system definition file/files and output results as plain text
-->
	<xsl:output method="text"/>
	<xsl:include href="test-model.xsl"/>
	<xsl:param name="level" select="3"/>

<xsl:template name="Section"><xsl:param name="text"/><xsl:param name="sub"/>
<xsl:text>&#xa;&#xa;</xsl:text>
<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"> (<xsl:value-of select="$sub"/>)</xsl:if>
</xsl:template>


<xsl:template name="Note"><xsl:param name="text"/><xsl:param name="sub"/>
<xsl:if test="$level &gt;= 3">
<xsl:text>&#xa;Note: </xsl:text>
<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"> (<xsl:value-of select="$sub"/>)</xsl:if>
</xsl:if>
</xsl:template>

<xsl:template name="Warning"><xsl:param name="text"/><xsl:param name="sub"/>
<xsl:if test="$level &gt;= 2">
<xsl:text>&#xa;Warning: </xsl:text>
<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"> (<xsl:value-of select="$sub"/>)</xsl:if>
</xsl:if>
</xsl:template>

<xsl:template name="Error"><xsl:param name="text"/><xsl:param name="sub"/>
<xsl:text>&#xa;Error: </xsl:text>
<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"> (<xsl:value-of select="$sub"/>)</xsl:if>
</xsl:template>

</xsl:stylesheet>