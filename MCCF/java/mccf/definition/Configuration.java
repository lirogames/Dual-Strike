//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 in JDK 6 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.08.08 at 05:19:36 PM CEST 
//


package mccf.definition;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlSchemaType;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for Configuration complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="Configuration">
 *   &lt;complexContent>
 *     &lt;extension base="{urn:strike-devices:configuration}Configuration">
 *       &lt;redefine>
 *         &lt;complexType name="Configuration">
 *           &lt;complexContent>
 *             &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *               &lt;sequence>
 *                 &lt;group ref="{urn:strike-devices:configuration}Infos"/>
 *                 &lt;sequence>
 *                   &lt;element name="device" type="{urn:strike-devices:configuration}Field"/>
 *                   &lt;element name="version" type="{urn:strike-devices:configuration}Field"/>
 *                   &lt;element name="page" type="{urn:strike-devices:configuration}Page" maxOccurs="unbounded"/>
 *                 &lt;/sequence>
 *               &lt;/sequence>
 *               &lt;attribute name="def-lang-version" use="required" type="{http://www.w3.org/2001/XMLSchema}int" fixed="1" />
 *               &lt;attribute name="lang" use="required" type="{http://www.w3.org/2001/XMLSchema}language" />
 *               &lt;attribute name="prefix" use="required" type="{http://www.w3.org/2001/XMLSchema}ID" />
 *               &lt;attribute name="icon-path" type="{urn:strike-devices:configuration}RelativePath" />
 *             &lt;/restriction>
 *           &lt;/complexContent>
 *         &lt;/complexType>
 *       &lt;/redefine>
 *       &lt;attribute name="byte-width" use="required" type="{http://www.w3.org/2001/XMLSchema}unsignedInt" />
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "Configuration")
public class Configuration
    extends OriginalConfiguration
{

    @XmlAttribute(name = "byte-width", required = true)
    @XmlSchemaType(name = "unsignedInt")
    protected long byteWidth;

    /**
     * Gets the value of the byteWidth property.
     * 
     */
    public long getByteWidth() {
        return byteWidth;
    }

    /**
     * Sets the value of the byteWidth property.
     * 
     */
    public void setByteWidth(long value) {
        this.byteWidth = value;
    }

}
