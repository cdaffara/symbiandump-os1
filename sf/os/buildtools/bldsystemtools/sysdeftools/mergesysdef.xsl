<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">
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
	Merge two 3.x syntax system definitions
-->

	<xsl:output method="xml" indent="yes"/>
	<xsl:param name="Downstream">mcl/System_Definition_Template.xml</xsl:param>
	<xsl:key name="origin" match="component" use="@origin-model"/>

<!-- only supports 3.x syntax and only operates on stand-alone models -->

<xsl:variable name="downstream" select="document($Downstream,.)/SystemDefinition"/>
<xsl:param name="upname">
	<xsl:choose>
		<xsl:when test="$downstream[starts-with(@schema,'2.') or starts-with(@schema,'1.')]">
			<xsl:message terminate="yes">Syntax <xsl:value-of select="@schema"/> not supported</xsl:message>
		</xsl:when>
		<xsl:when test="name($downstream/*)!=name(/SystemDefinition/*)">
			<xsl:message terminate="yes">Can only merge fragments of the same rank</xsl:message>
		</xsl:when>
<!--		<xsl:when test="$downstream[not(systemModel)]">
			<xsl:message terminate="yes">Needs to be a standalone system definition</xsl:message>
		</xsl:when>-->
		<xsl:when test="/SystemDefinition/systemModel/@name=$downstream/systemModel/@name or not(/SystemDefinition/systemModel/@name)">
			<xsl:apply-templates mode="origin-term" select="/*">
				<xsl:with-param name="root">Upstream</xsl:with-param>
			</xsl:apply-templates>
			</xsl:when>
		<xsl:otherwise><xsl:value-of select="/SystemDefinition/systemModel/@name"/></xsl:otherwise>
	</xsl:choose>
</xsl:param>

<xsl:param name="downname">
	<xsl:choose>
		<xsl:when test="/SystemDefinition/systemModel/@name=$downstream/systemModel/@name or not($downstream/systemModel/@name)">
			<xsl:apply-templates mode="origin-term" select="$downstream">	
				<xsl:with-param name="root">Downstream</xsl:with-param>
			</xsl:apply-templates>
			</xsl:when>
		<xsl:when test="name($downstream/*)!=name(/SystemDefinition/*)">
			<xsl:message terminate="yes">Can only merge fragments of the same rank</xsl:message>
		</xsl:when>
		<xsl:otherwise><xsl:value-of select="$downstream/systemModel/@name"/></xsl:otherwise>
	</xsl:choose>
</xsl:param>

<xsl:template mode="origin-term" match="*">
	<xsl:param name="root"/>
	<xsl:param name="index"/>
	<xsl:choose>
		<xsl:when test="not(key('origin',concat($root,$index)))">
			<xsl:value-of select="concat($root,$index)"/>
		</xsl:when>
		<xsl:when test="$index=''">
			<xsl:apply-templates mode="origin-term" select=".">	
				<xsl:with-param name="root" select="$root"/>
				<xsl:with-param name="index" select="1"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates mode="origin-term" select=".">	
				<xsl:with-param name="root" select="$root"/>
				<xsl:with-param name="index" select="$index + 1"/>
			</xsl:apply-templates>		
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>


<!-- choose the greater of the two versions -->
<xsl:template name="compare-versions"><xsl:param name="v1"/><xsl:param name="v2"/>
			<xsl:choose>
				<xsl:when test="$v1=$v2"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before($v1,'.') &gt; substring-before($v2,'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before($v1,'.') &lt; substring-before($v2,'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:when test="substring-before(substring-after($v1,'.'),'.') &gt; substring-before(substring-after($v2,'.'),'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-before(substring-after($v1,'.'),'.') &lt; substring-before(substring-after($v2,'.'),'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:when test="substring-after(substring-after($v1,'.'),'.') &gt; substring-after(substring-after($v2,'.'),'.')"><xsl:value-of select="$v1"/></xsl:when>
				<xsl:when test="substring-after(substring-after($v1,'.'),'.') &lt; substring-after(substring-after($v2,'.'),'.')"><xsl:value-of select="$v2"/></xsl:when>
				<xsl:otherwise><xsl:value-of select="$v1"/></xsl:otherwise>
			</xsl:choose>
</xsl:template>

<!--  this merge only two files according to the 3.0.x rules. Old syntax not supported. Must be converetd before calling -->



<xsl:template match="/*">
	<xsl:variable name="upmodel">
		<sysdef name="{$upname}"/>
	</xsl:variable>
	<xsl:variable name="downmodel">
		<sysdef name="{$downname}"/>
	</xsl:variable>
	
	<xsl:choose>
		<xsl:when test="function-available('exslt:node-set')">
			<xsl:apply-templates mode="merge-models" select=".">
				<xsl:with-param name="other" select="$downstream"/>
				<xsl:with-param name="up" select="exslt:node-set($upmodel)/sysdef"/>
				<xsl:with-param name="down" select="exslt:node-set($downmodel)/sysdef"/>
			</xsl:apply-templates>
		</xsl:when>
		<xsl:otherwise> <!-- no node set funcion, so don't bother setting the names -->
			<xsl:apply-templates mode="merge-models" select=".">
				<xsl:with-param name="other" select="$downstream"/>
			</xsl:apply-templates> 		
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:include href="mergesysdef-module.xsl"/>
</xsl:stylesheet>
