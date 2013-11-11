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
	Validate a system definition file or files and output results in HTML
-->
	<xsl:output method="html"/>
	<xsl:include href="test-model.xsl"/>
<xsl:template match="/model">
	<html><head>
		<title>Cross-Checking System Model</title>
		<style>p {margin: 0px; padding: 0px} 
			p.warn span {color: blue}
			p.note span {color: green}
			p.err span {font-weight: bold}
			p.err {color: red; background: yellow}
			p.note {font-size: 90%}
			<xsl:if test="contains(concat(' ',normalize-space(/model/@supress-errors),' '),' note ')">
				p.note {display: none}
			</xsl:if>
			<xsl:if test="contains(concat(' ',normalize-space(/model/@supress-errors),' '),' err ')">
				p.err {display: none}
			</xsl:if>
			<xsl:if test="contains(concat(' ',normalize-space(/model/@supress-errors),' '),' warn ')">
				p.warn {display: none}
			</xsl:if>
		</style>
	</head><body>
	<xsl:apply-templates select="." mode="check"/>
	</body></html>
</xsl:template>


<xsl:template name="Section"><xsl:param name="text"/><xsl:param name="sub"/>
<h2><xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"><xsl:text> </xsl:text> <small>(<xsl:value-of select="$sub"/>)</small></xsl:if></h2>
</xsl:template>

<xsl:template name="Note"><xsl:param name="text"/><xsl:param name="sub"/>
<p class="note">
	<span>Note:</span>
	<xsl:text> </xsl:text>
	<xsl:copy-of select="$text"/>
<xsl:if test="$sub!=''"><xsl:text> </xsl:text> <small>(<xsl:value-of select="$sub"/>)</small></xsl:if></p>
</xsl:template>

<xsl:template name="Warning"><xsl:param name="text"/><xsl:param name="sub"/>
<p class="warn">
	<span>Warning:</span>
	<xsl:text> </xsl:text>
	<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"><xsl:text> </xsl:text> <small>(<xsl:value-of select="$sub"/>)</small></xsl:if></p>
</xsl:template>

<xsl:template name="Error"><xsl:param name="text"/><xsl:param name="sub"/>
<p class="err">
	<span>Error:</span>
	<xsl:text> </xsl:text>
	<xsl:value-of select="$text"/>
<xsl:if test="$sub!=''"><xsl:text> </xsl:text> <small>(<xsl:value-of select="$sub"/>)</small></xsl:if></p>
</xsl:template>

</xsl:stylesheet>