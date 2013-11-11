<xsl:stylesheet  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<!--Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
	All rights reserved.
	This component and the accompanying materials are made available
	under the terms of the License "Eclipse Public License v1.0"
	which accompanies this distribution, and is available
	at the URL "http://www.eclipse.org/legal/epl-v10.html".

	Initial Contributors:
	Nokia Corporation - initial contribution.
	Contributors:
	Description:
	Module containing the validation logic for system definition 3.0.0 syntax
-->
	<xsl:key name="named" match="*[ancestor::systemModel]" use="@name"/>
	<xsl:param name="Filename"/>
	<xsl:variable name="info" select="document(/model//info[@type='extra']/@href,/model)//c"/>

	<xsl:variable name="all-ids">
		<xsl:apply-templates select="document(/model/sysdef/@href)| SystemDefinition" mode="ids"/>
	</xsl:variable>

 
<xsl:template match="/model" priority="-1">
	<xsl:apply-templates select="." mode="check"/>
</xsl:template>

<xsl:template match="/model" mode="check">
	<xsl:for-each select="sysdef">
		<xsl:apply-templates select="document (@href,. )/*">
			<xsl:with-param name="filename">
				<xsl:choose>
					<xsl:when test="starts-with(current()/@href,current()/@rootpath)">
						<xsl:value-of select="substring-after(current()/@href,current()/@rootpath)"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="@href"/>
					</xsl:otherwise>
				</xsl:choose>
			 </xsl:with-param>
		</xsl:apply-templates>
	</xsl:for-each>



<xsl:apply-templates mode="x-check" select="document (//info[@type='vp']/@href)/*">
	<xsl:with-param name="sysdef" select="document (sysdef/@href)/*"/>
</xsl:apply-templates>

<xsl:apply-templates mode="x-check" select="document (//info[@type='build']/@href)/*">
	<xsl:with-param name="sysdef" select="document (sysdef/@href)/*"/>
</xsl:apply-templates>
</xsl:template>

<xsl:template match="/SystemDefinition[starts-with(@schema,'3.0.')]" mode="ids">
	<xsl:for-each select="//*[@id and not(@href)]"><xsl:value-of select="concat(' ',@id,' ')"/></xsl:for-each>
	<xsl:apply-templates select="document(//layer/@href | //package/@href | //collection/@href | //component/@href,.)/*" mode="ids"/>
</xsl:template>

<xsl:template match="/SystemDefinition[starts-with(@schema,'3.0.')and systemModel]" priority="2">
	<xsl:param name="filename" select="$Filename"/>
<xsl:call-template name="Section">
	<xsl:with-param name="text">System Definition: <xsl:value-of select="*/@name"/></xsl:with-param>
	<xsl:with-param name="sub"><xsl:value-of select="(string-length($all-ids) - string-length(translate($all-ids,' ',''))) div 2 "/> items</xsl:with-param>
</xsl:call-template>
	<xsl:apply-templates select="*">
		<xsl:with-param name="filename" select="$filename"/>
	</xsl:apply-templates>
</xsl:template>

<xsl:template match="/SystemDefinition[starts-with(@schema,'3.0.')] | systemModel">
	<xsl:param name="filename"  select="$Filename"/>
		
<xsl:if test="//unit">
<xsl:call-template name="Section">
	<xsl:with-param name="text"><xsl:value-of select="translate(substring(name(*),1,1),'clp','CLP')"/><xsl:value-of select="substring(name(*),2)"/> Definition: <xsl:value-of select="*/@name"/></xsl:with-param>
	<xsl:with-param name="sub"><xsl:value-of select="count(//unit)"/> unit<xsl:if test="count(//unit)!=1">s</xsl:if></xsl:with-param>
</xsl:call-template>
</xsl:if>
<xsl:if test="self::systemModel and not(@name)">
	<xsl:call-template name="Error"><xsl:with-param name="text">systemModel element should have a name</xsl:with-param></xsl:call-template>
</xsl:if>
	<xsl:apply-templates select="*">
		<xsl:with-param name="filename" select="$filename"/>
	</xsl:apply-templates>
</xsl:template>


<xsl:template match="@*" mode="valid">
	<xsl:call-template name="Error"><xsl:with-param name="text">Attribute <xsl:value-of select="name()"/>="<xsl:value-of select="."/>" is not valid for <xsl:value-of select="name(..)"/></xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="@before|@id|package/@span|collection/@level|package/@level|package/@levels|layer/@levels" mode="valid"/> <!-- really should check syntax -->

<xsl:template match="@name|@href|@filter" mode="valid"/> 

<xsl:template match="component/@introduced|component/@deprecated" mode="valid"/> 



<xsl:template match="@replace" mode="valid">
	<xsl:if test="/SystemDefinition[@schema='3.0.0']">
		<xsl:call-template name="Error"><xsl:with-param name="text">Attribute <b><xsl:value-of select="name()"/></b>="<xsl:value-of select="."/>" not valid in schema <xsl:value-of select="/SystemDefinition/@schema"/>. Must use schema 3.0.1 or higher</xsl:with-param></xsl:call-template>
	</xsl:if>
</xsl:template>



<xsl:template name="validate-class">
	<ok>plugin</ok>
	<ok>doc</ok>
	<ok>tool</ok>
	<ok>config</ok>
	<ok>api</ok>
	<w d="deprecated">test</w>
</xsl:template>

<xsl:template name="validate-purpose">
	<ok>mandatory</ok>
	<ok>optional</ok>
	<ok>development</ok>
</xsl:template>
<xsl:template name="validate-target">
	<ok>other</ok>
	<ok>desktop</ok>
	<ok>device</ok>
</xsl:template>


<xsl:template name="validate-tech-domain">
	<ok>lo</ok>
	<ok>hb</ok>
	<ok>mm</ok>
	<ok>ma</ok>
	<ok>pr</ok>
	<ok>vc</ok>
	<ok>se</ok>
	<ok>ui</ok>
	<ok>dc</ok>
	<ok>de</ok>
	<ok>dm</ok>
	<ok>rt</ok>
	<ok>to</ok>
	<w d="Non-standard">ocp</w>
</xsl:template>

<xsl:template match="component/@class" mode="valid">
	<xsl:call-template name="checklist">
		<xsl:with-param name="list" select="normalize-space(.)"/>
		<xsl:with-param name="values" select="document('')/*/xsl:template[@name=concat('validate-',name(current()))]/*"/>
	</xsl:call-template>
</xsl:template> 

<xsl:template name="checklist">
	<xsl:param name="list" select="."/><xsl:param name="values"/><xsl:param name="sep" select="' '"/>
	<xsl:variable name="item">
		<xsl:choose>
			<xsl:when test="contains($list,$sep)"><xsl:value-of select="substring-before($list,$sep)"/></xsl:when>
			<xsl:otherwise><xsl:value-of select="$list"/></xsl:otherwise>
		</xsl:choose>
	</xsl:variable>
	<xsl:variable name="v" select="$values[.=$item]"/>
	<xsl:choose>
		<xsl:when test="not($v)">
			<xsl:call-template name="Error"><xsl:with-param name="text">Illegal <xsl:value-of select="name()"/> value <xsl:value-of select="name()"/>="<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
		</xsl:when> 
		<xsl:when test="name($v)='ok'"/> 
		<xsl:when test="name($v)='w'">
			<xsl:call-template name="Warning"><xsl:with-param name="text"><xsl:value-of select="$v/@d"/> value in <xsl:value-of select="name()"/>="<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
		</xsl:when> 
	</xsl:choose>	
	<xsl:if test="contains($list,$sep)">
		<xsl:call-template name="checklist">
			<xsl:with-param name="list" select="substring-after($list,$sep)"/>
			<xsl:with-param name="values" select="$values"/>
			<xsl:with-param name="sep" select="$sep"/>			
		</xsl:call-template>
	</xsl:if>
</xsl:template> 


<xsl:template match="package/@tech-domain|component/@purpose|component/@target" mode="valid">
	<xsl:variable name="v" select="document('')/*/xsl:template[@name=concat('validate-',name(current()))]/*[.=current()]"/>
	<xsl:choose>
		<xsl:when test="not($v)">
			<xsl:call-template name="Error"><xsl:with-param name="text">Illegal <xsl:value-of select="name()"/> value <xsl:value-of select="name()"/>="<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
		</xsl:when> 
		<xsl:when test="name($v)='ok'"/> 
		<xsl:when test="name($v)='w'">
			<xsl:call-template name="Warning"><xsl:with-param name="text"><xsl:value-of select="$v/@d"/> value in <xsl:value-of select="name()"/>="<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
		</xsl:when> 
	</xsl:choose>
</xsl:template> 





<xsl:template match="*" priority="-2">
	<xsl:call-template name="Error"><xsl:with-param name="text">Element "<xsl:value-of select="name()"/>" is not valid in the context of "<xsl:value-of select="name(..)"/>"</xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="component[not(parent::collection)] | collection[not(parent::package)] | package[not(parent::package or parent::layer or (parent::SystemDefinition and count(../*)=1))] | layer[not(parent::systemModel)] " priority="3">
	<xsl:call-template name="Error"><xsl:with-param name="text"><xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" has invalid parent <xsl:value-of select="name(..)"/> "<xsl:value-of select="../@id"/>"</xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="layer | package | collection | component">
	<xsl:param name="filename"/>

<xsl:apply-templates select="@*" mode="valid"/>
<xsl:apply-templates select="@id"/>
<xsl:if test="self::component">
	<xsl:choose>
		<xsl:when test="count(unit[not(@filter | @version)]) = 0 "/>
		<xsl:when test="count(unit[not(@version)]) &gt; 1 and @filter='s60'">
			<xsl:call-template name="Warning"><xsl:with-param name="text">S60 Component "<xsl:value-of select="@id"/>" has <xsl:value-of select="count(unit)"/> units.</xsl:with-param></xsl:call-template>
		</xsl:when>
		<xsl:when test="count(unit[not(@version)]) &gt; 1">
			<xsl:call-template name="Error"><xsl:with-param name="text">Component "<xsl:value-of select="@id"/>" has <xsl:value-of select="count(unit)"/> units.</xsl:with-param></xsl:call-template>
		</xsl:when>
	</xsl:choose>
	<xsl:choose>
		<xsl:when test="unit"/>
		<xsl:when test="contains(comment(),'PLACEHOLDER=')"/>
		<xsl:when test="comment()">
			<xsl:call-template name="Note"><xsl:with-param name="text">Component "<xsl:value-of select="@name"/>" is empty.</xsl:with-param></xsl:call-template>
		</xsl:when>
		<xsl:when test="not(comment())">
			<xsl:call-template name="Warning"><xsl:with-param name="text">Component "<xsl:value-of select="@name"/>" is empty and has no comment</xsl:with-param></xsl:call-template>
		</xsl:when>
	</xsl:choose>
</xsl:if>
<xsl:if test="@href">
	<xsl:variable name="child" select="document(@href,.)/SystemDefinition"/>
	<xsl:if test="@id!=$child/@id">
		<xsl:call-template name="Error"><xsl:with-param name="text"><xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" must match ID in linked file "<xsl:value-of select="@href"/>"</xsl:with-param></xsl:call-template>
	</xsl:if>
	<xsl:if test="$child/@href">
		<xsl:call-template name="Error"><xsl:with-param name="text">linked <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" cannot be a link</xsl:with-param></xsl:call-template>
	</xsl:if>
	<xsl:for-each select="@*[name()!='id']">
		<xsl:if test="$child/@*[name()=name(current())]">
			<xsl:call-template name="Warning"><xsl:with-param name="text">linked <xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" has duplicate attribute to linking document. Duplicate ignored.</xsl:with-param></xsl:call-template>
		</xsl:if>
	</xsl:for-each>
	<xsl:if test="*">
		<xsl:call-template name="Error"><xsl:with-param name="text"><xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" cannot have both link and content. Content ignored.</xsl:with-param></xsl:call-template>
	</xsl:if>
</xsl:if>
<xsl:if test="@href and name()!=name(document(@href,.)/SystemDefinition/*)">
		<xsl:call-template name="Error"><xsl:with-param name="text"><xsl:value-of select="name()"/> "<xsl:value-of select="@id"/>" must match item in linked file "<xsl:value-of select="@href"/>"</xsl:with-param></xsl:call-template>
</xsl:if>
<xsl:if test="not(@href)">
	<xsl:apply-templates select="*">
		<xsl:with-param name="filename" select="$filename"/>
	</xsl:apply-templates>
</xsl:if>
<xsl:if test="@href">
	<xsl:apply-templates select="document(@href,.)/*">
		<xsl:with-param name="filename">
			<xsl:call-template name="normpath">
				<xsl:with-param name="path">
					<xsl:if test="not(starts-with(current()/@href,'/'))">
						<xsl:call-template name="before">
							<xsl:with-param name="text" select="$filename"/>
						</xsl:call-template>
					</xsl:if>
					<xsl:value-of select="current()/@href"/>
				 </xsl:with-param>
			</xsl:call-template>
		 </xsl:with-param>
	</xsl:apply-templates>
</xsl:if>
</xsl:template>



<xsl:template match="meta">	<xsl:param name="filename"/>
	<xsl:apply-templates select="@*"/>
</xsl:template>

<xsl:template match="meta/@rel | meta/@type | meta/@href"/> <!-- anything is valid -->

<xsl:template match="unit">	<xsl:param name="filename"/>
	<xsl:apply-templates select="@mrp|@bldFile">
		<xsl:with-param name="filename" select="$filename"/>
	</xsl:apply-templates>
</xsl:template>


<xsl:template match="@id" mode="path">
	<xsl:choose>
		<xsl:when test="contains(.,':')"><xsl:value-of  select="substring-after(.,':')"/></xsl:when>
		<xsl:otherwise><xsl:value-of  select="."/></xsl:otherwise>
	</xsl:choose>
</xsl:template>


<xsl:template match="@id">
<xsl:if test="contains(concat(' ',substring-after($all-ids,concat(' ',.,' '))),concat(' ',.,' '))">
	<xsl:call-template name="Warning"><xsl:with-param name="text">Duplicate ID: <xsl:value-of select="name(..)"/> "<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
</xsl:if>

<xsl:if test="contains(.,':') and not(ancestor::*/namespace::*[name()=substring-before(current(),':')])">
	<xsl:call-template name="Error"><xsl:with-param name="text">Undefined namespace for ID "<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
</xsl:if>

</xsl:template>


<xsl:template mode="localid" match="*">
	<xsl:choose>
		<xsl:when test="contains(@id,':')">/<xsl:value-of select="substring-after(@id,':')"/></xsl:when>
		<xsl:otherwise><xsl:value-of select="@id"/></xsl:otherwise>
	</xsl:choose>
</xsl:template>
<xsl:template match="@bldFile|@mrp"><xsl:param name="filename"/>
<xsl:if test="substring(.,string-length(.))='/'">
		<xsl:call-template name="Warning"><xsl:with-param name="text"><code><xsl:value-of select="name()"/></code> path "<xsl:value-of select="."/>" should not end in /</xsl:with-param></xsl:call-template>
</xsl:if>
<xsl:if test="contains(.,'\')">
		<xsl:call-template name="Error"><xsl:with-param name="text"><code><xsl:value-of select="name()"/></code> path "<xsl:value-of select="."/>" must use only forward slashes</xsl:with-param></xsl:call-template>
</xsl:if>

<!-- this is a realtive path, so just check that it's the expected number of dirs down -->
	<xsl:variable name="fullpath"><xsl:call-template name="normpath">
				<xsl:with-param name="path">
					<xsl:if test="not(starts-with(.,'/'))">
						<xsl:call-template name="before">
							<xsl:with-param name="text" select="$filename"/>
						</xsl:call-template>
					</xsl:if>
					<xsl:value-of select="."/>
				 </xsl:with-param>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="path">
			<xsl:choose>
				<xsl:when test="not(contains($filename,':'))">/<xsl:for-each select="ancestor::*/@id"><xsl:apply-templates mode="path" select="."/>/</xsl:for-each></xsl:when>
				<xsl:otherwise><xsl:for-each select="../../../@id|../../@id"><xsl:apply-templates mode="path" select="."/>/</xsl:for-each></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="segment"> <!-- the part relative to the fragment directory -->
			<xsl:choose>
				<xsl:when test="ancestor::layer">
					<xsl:apply-templates select="ancestor::package" mode="localid"/>/<xsl:apply-templates select="ancestor::collection" mode="localid"/>
				</xsl:when>
				<xsl:when test="ancestor::package">
				<xsl:apply-templates select="ancestor::collection" mode="localid"/>
				</xsl:when>
				<xsl:when test="ancestor::collection"/>
			</xsl:choose>/<xsl:apply-templates select="ancestor::component" mode="localid"/>/</xsl:variable>
		<xsl:choose>
			<xsl:when test="not(starts-with(concat(.,'/'),$segment))">
				<xsl:call-template name="Note"><xsl:with-param name="text">Unexpected <code><xsl:value-of select="name()"/></code> path for <xsl:apply-templates mode="path" select="../../../@id"/> -&gt; <strong><xsl:apply-templates mode="path" select="../../@id"/></strong>: "<xsl:value-of select="$fullpath"/>"</xsl:with-param></xsl:call-template>
			</xsl:when>
		</xsl:choose>
</xsl:template>


<xsl:template match="@bldFile[starts-with(.,'/') or contains(.,'../') or contains(.,':')] | @mrp[starts-with(.,'/') or contains(.,'../') or contains(.,':')] |@base[starts-with(.,'/') or contains(.,'../') or contains(.,':')]"><xsl:param name="filename"/>
<xsl:if test="substring(.,string-length(.))='/'">
		<xsl:call-template name="Warning"><xsl:with-param name="text"><code><xsl:value-of select="name()"/></code> path "<xsl:value-of select="."/>" should not end in /</xsl:with-param></xsl:call-template>
</xsl:if>
<xsl:if test="contains(.,'\')">
		<xsl:call-template name="Error"><xsl:with-param name="text"><code><xsl:value-of select="name()"/></code> path "<xsl:value-of select="."/>" must use only forward slashes</xsl:with-param></xsl:call-template>
</xsl:if>
	<xsl:variable name="fullpath"><xsl:call-template name="normpath">
				<xsl:with-param name="path">
					<xsl:if test="not(starts-with(.,'/'))">
						<xsl:call-template name="before">
							<xsl:with-param name="text" select="$filename"/>
						</xsl:call-template>
					</xsl:if>
					<xsl:value-of select="."/>
				 </xsl:with-param>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="path">
			<xsl:choose>
				<xsl:when test="not(contains($filename,':'))">/<xsl:for-each select="ancestor::*/@id"><xsl:apply-templates mode="path" select="."/>/</xsl:for-each></xsl:when>
				<xsl:otherwise><xsl:for-each select="../../../@id|../../@id"><xsl:apply-templates mode="path" select="."/>/</xsl:for-each></xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:variable name="segment">
			<xsl:choose>
				<xsl:when test="ancestor::layer">
					<xsl:value-of select="concat($fullpath,'/')"/>
				</xsl:when>
				<xsl:when test="ancestor::package">
					<xsl:value-of select="concat('/',substring-after(substring-after($fullpath,'/'),'/'),'/')"/>
				</xsl:when>
				<xsl:when test="ancestor::collection">
					<xsl:value-of select="concat('/',substring-after(substring-after(substring-after($fullpath,'/'),'/'),'/'),'/')"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat('/',substring-after(substring-after(substring-after(substring-after($fullpath,'/'),'/'),'/'),'/'),'/')"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
<xsl:choose>
	<xsl:when test="contains($filename,':')">
		<xsl:choose>
			<xsl:when test="not(starts-with(.,$path) or concat(.,'/')=$path)">
				<xsl:call-template name="Note"><xsl:with-param name="text">Unexpected <code><xsl:value-of select="name()"/></code> path for <xsl:apply-templates mode="path" select="../../../@id"/> -&gt; <strong><xsl:apply-templates mode="path" select="../../@id"/></strong>: "<xsl:value-of select="."/>"</xsl:with-param></xsl:call-template>
			</xsl:when>
		</xsl:choose>
	</xsl:when>
	<xsl:otherwise>
		<xsl:choose>
			<xsl:when test="substring-before(substring($segment,2),'/') != substring-before(substring($path,2),'/') and (ancestor::SystemDefinition/@id-namespace!='http://www.symbian.org/system-definition' and not(contains(../../@id,':')))">
				<xsl:call-template name="Warning"><xsl:with-param name="text">Unexpected <code><xsl:value-of select="name()"/></code> path for <xsl:apply-templates mode="path" select="../../../@id"/> -&gt; <strong><xsl:apply-templates mode="path" select="../../@id"/></strong>: "<xsl:value-of select="$fullpath"/>"</xsl:with-param></xsl:call-template>
			</xsl:when>
			<xsl:when test="substring-before(substring($segment,2),'/') != substring-before(substring($path,2),'/')">
				<xsl:call-template name="Error"><xsl:with-param name="text">Unexpected <code><xsl:value-of select="name()"/></code> path for <xsl:apply-templates mode="path" select="../../../@id"/> -&gt; <strong><xsl:apply-templates mode="path" select="../../@id"/></strong>: "<xsl:value-of select="$fullpath"/>"</xsl:with-param></xsl:call-template>
			</xsl:when>
			<xsl:when test="not(starts-with($segment,$path))">
				<xsl:call-template name="Note"><xsl:with-param name="text">Unexpected <code><xsl:value-of select="name()"/></code> path for <xsl:apply-templates mode="path" select="../../../@id"/> -&gt; <strong><xsl:apply-templates mode="path" select="../../@id"/></strong>: "<xsl:value-of select="$fullpath"/>"</xsl:with-param></xsl:call-template>
			</xsl:when>
		</xsl:choose>
	</xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="SystemDefinition" mode="check-matches">
	<xsl:param name="which"/>
	<xsl:param name="other"/>
	<xsl:for-each select="//*[@mrp]">
		<xsl:variable name="mrp" select="@mrp"/>
		<xsl:if test="not($other//*[@mrp=$mrp])">
			<xsl:call-template name="Error"><xsl:with-param name="text"><xsl:value-of select="$mrp"/> has no match in <xsl:value-of select="$which"/>.</xsl:with-param></xsl:call-template>
		</xsl:if>
	</xsl:for-each>
</xsl:template>

<xsl:template match="*" mode="value">
	<xsl:call-template name="Error"><xsl:with-param name="text">
		<xsl:value-of select="name()"/>:<xsl:for-each select="@*"><xsl:value-of select="concat(name(),'=',.)"/></xsl:for-each>
	</xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="*" mode="compare"><xsl:param name="base"/>
<xsl:variable name="n" select="@id"/>
<xsl:variable name="tag" select="name()"/>
<xsl:variable name="v"><xsl:apply-templates select="." mode="value"/></xsl:variable>
<xsl:variable name="v2"><xsl:apply-templates mode="value" select="$base//*[name()=$tag and @id=$n]"/></xsl:variable>
<xsl:if  test="$v != $v2">
	<xsl:call-template name="Error"><xsl:with-param name="text">
		<xsl:value-of select="concat(name(),' ',@name)"/> not identical. [<xsl:value-of select="$v"/>|<xsl:value-of select="$v2"/>]</xsl:with-param></xsl:call-template>
</xsl:if>	
</xsl:template>



<xsl:template match="unit" mode="compare" priority="1"><xsl:param name="base"/>
<xsl:variable name="n" select="concat(@version,':',@mrp,'.',@prebuilt)"/>
<xsl:variable name="v"><xsl:apply-templates select="." mode="value"/></xsl:variable>
<xsl:variable name="v2"><xsl:apply-templates mode="value" select="$base//unit[concat(@version,':',@mrp,'.',@prebuilt)=$n]"/></xsl:variable>
<xsl:if  test="$v != $v2">
	<xsl:call-template name="Error"><xsl:with-param name="text">
		<xsl:value-of select="concat('&#xa;',name(),' ',../@name,' v',@version)"/> not identical. [<xsl:value-of select="$v"/>|<xsl:value-of select="$v2"/>]</xsl:with-param></xsl:call-template>
</xsl:if>
</xsl:template>

<xsl:template match="/variations/@module"> (<xsl:value-of select="."/>)</xsl:template>

<xsl:template match="/variations" mode="x-check"><xsl:param name="sysdef"/>
<xsl:call-template name="Section"><xsl:with-param name="text">vp cross-check <xsl:apply-templates select="@module"/></xsl:with-param></xsl:call-template>
<xsl:for-each select="//component">
	<xsl:variable name="found">
		<xsl:apply-templates select="$sysdef" mode="lookfor">
			<xsl:with-param name="id" select="@name"/>
			<xsl:with-param name="version" select="@version"/>
		</xsl:apply-templates>
	</xsl:variable>
	<xsl:if test="$found=''">
		<xsl:call-template name="Error"><xsl:with-param name="text">VP component "<xsl:value-of select="@name"/>"<xsl:if test="@version"> v<xsl:value-of select="@version"/></xsl:if> not in SysDef</xsl:with-param></xsl:call-template>
	</xsl:if>
</xsl:for-each>
</xsl:template>

<xsl:template match="*" mode="lookfor"><xsl:param name="id"/><xsl:param name="version"/>
	<xsl:choose>
		<xsl:when test="string-length($version) and //component[@id=$id and unit[@version=$version]]">Found</xsl:when>
		<xsl:when test="string-length($version)=0 and //*[@id=$id]">Found</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates select="document(//layer/@href | //package/@href | //collection/@href | //component/@href,.)/*/*" mode="lookfor">
				<xsl:with-param name="version" select="$version"/>
				<xsl:with-param name="id" select="$id"/>
			</xsl:apply-templates>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>



<xsl:template match="SystemBuild" mode="x-check"><xsl:param name="sysdef"/>

<xsl:call-template name="Section"><xsl:with-param name="text">System Build cross-check</xsl:with-param></xsl:call-template>
<xsl:for-each select="//ref">
	<xsl:variable name="found">
		<xsl:apply-templates select="$sysdef" mode="lookfor">
		<xsl:with-param name="id" select="current()/@item"/>
	</xsl:apply-templates>
</xsl:variable>
	<xsl:if test="$found=''">
		<xsl:call-template name="Error"><xsl:with-param name="text">Build item "<xsl:value-of select="@item"/>" not in SysDef</xsl:with-param></xsl:call-template>
</xsl:if>
</xsl:for-each>

<xsl:for-each select="//listRef">
	<xsl:if test="not(//list[@name=current()/@list])">
		<xsl:call-template name="Error"><xsl:with-param name="text">Build list "<xsl:value-of select="@name"/>" not defined</xsl:with-param></xsl:call-template>
</xsl:if>	
</xsl:for-each>
</xsl:template>


<xsl:template name="my-release">
	<xsl:variable name="n" select="@name"/>
	<xsl:variable name="new" select="@introduced"/>
	<xsl:variable name="end" select="@deprecated"/>
	<xsl:choose>
		<xsl:when test="$new!='' and $end!=''">(<xsl:value-of select="concat($new,' - ',$end)"/>)</xsl:when>
		<xsl:when test="$new!='' ">(<xsl:value-of select="$new"/>)</xsl:when>
		<xsl:when test="$end!=''">(? - <xsl:value-of select="$end"/>)</xsl:when>
	</xsl:choose>
</xsl:template>

<xsl:template name="normpath"><xsl:param name="path"/>
<!-- normalize out any ".." in the path in $path  -->
<xsl:choose>
	<xsl:when test="contains($path,'/../')">
	<xsl:call-template name="normpath">
		<xsl:with-param name="path">
		<xsl:call-template name="before">
			<xsl:with-param name="text" select="substring-before($path,'/../')"/>
		</xsl:call-template>
		<xsl:value-of select="substring-after($path,'/../')"/>
		</xsl:with-param>
		</xsl:call-template>
	</xsl:when>
	<xsl:otherwise><xsl:value-of select="$path"/></xsl:otherwise>
</xsl:choose>
</xsl:template>

<!-- return all text before the last / -->
<xsl:template name="before"><xsl:param name="text"/>
<xsl:if test="contains($text,'/')">
	<xsl:value-of select="substring-before($text,'/')"/>/<xsl:call-template name="before"><xsl:with-param name="text" select="substring-after($text,'/')"/></xsl:call-template>
	</xsl:if>
</xsl:template>


</xsl:stylesheet>