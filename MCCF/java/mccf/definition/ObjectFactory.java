//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 in JDK 6 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2011.01.19 at 11:06:42 AM CET 
//


package mccf.definition;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the mccf.definition package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _Configuration_QNAME = new QName("urn:strike-devices:configuration", "configuration");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: mccf.definition
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link ChoiceReference }
     * 
     */
    public ChoiceReference createChoiceReference() {
        return new ChoiceReference();
    }

    /**
     * Create an instance of {@link OriginalOption }
     * 
     */
    public OriginalOption createOriginalOption() {
        return new OriginalOption();
    }

    /**
     * Create an instance of {@link Option }
     * 
     */
    public Option createOption() {
        return new Option();
    }

    /**
     * Create an instance of {@link Configuration }
     * 
     */
    public Configuration createConfiguration() {
        return new Configuration();
    }

    /**
     * Create an instance of {@link OriginalIntegerSetting }
     * 
     */
    public OriginalIntegerSetting createOriginalIntegerSetting() {
        return new OriginalIntegerSetting();
    }

    /**
     * Create an instance of {@link OriginalChoiceSetting }
     * 
     */
    public OriginalChoiceSetting createOriginalChoiceSetting() {
        return new OriginalChoiceSetting();
    }

    /**
     * Create an instance of {@link PathInfo }
     * 
     */
    public PathInfo createPathInfo() {
        return new PathInfo();
    }

    /**
     * Create an instance of {@link OriginalConfiguration }
     * 
     */
    public OriginalConfiguration createOriginalConfiguration() {
        return new OriginalConfiguration();
    }

    /**
     * Create an instance of {@link IntegerSetting }
     * 
     */
    public IntegerSetting createIntegerSetting() {
        return new IntegerSetting();
    }

    /**
     * Create an instance of {@link BooleanRequires }
     * 
     */
    public BooleanRequires createBooleanRequires() {
        return new BooleanRequires();
    }

    /**
     * Create an instance of {@link IntegerDomain }
     * 
     */
    public IntegerDomain createIntegerDomain() {
        return new IntegerDomain();
    }

    /**
     * Create an instance of {@link ChoiceSetting }
     * 
     */
    public ChoiceSetting createChoiceSetting() {
        return new ChoiceSetting();
    }

    /**
     * Create an instance of {@link BooleanReference }
     * 
     */
    public BooleanReference createBooleanReference() {
        return new BooleanReference();
    }

    /**
     * Create an instance of {@link IntegerDomainEntry }
     * 
     */
    public IntegerDomainEntry createIntegerDomainEntry() {
        return new IntegerDomainEntry();
    }

    /**
     * Create an instance of {@link IntegerReference }
     * 
     */
    public IntegerReference createIntegerReference() {
        return new IntegerReference();
    }

    /**
     * Create an instance of {@link OptionRequires }
     * 
     */
    public OptionRequires createOptionRequires() {
        return new OptionRequires();
    }

    /**
     * Create an instance of {@link IntegerRequires }
     * 
     */
    public IntegerRequires createIntegerRequires() {
        return new IntegerRequires();
    }

    /**
     * Create an instance of {@link Page }
     * 
     */
    public Page createPage() {
        return new Page();
    }

    /**
     * Create an instance of {@link DescriptionImage }
     * 
     */
    public DescriptionImage createDescriptionImage() {
        return new DescriptionImage();
    }

    /**
     * Create an instance of {@link OriginalBooleanSetting }
     * 
     */
    public OriginalBooleanSetting createOriginalBooleanSetting() {
        return new OriginalBooleanSetting();
    }

    /**
     * Create an instance of {@link BooleanSetting }
     * 
     */
    public BooleanSetting createBooleanSetting() {
        return new BooleanSetting();
    }

    /**
     * Create an instance of {@link Info }
     * 
     */
    public Info createInfo() {
        return new Info();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Configuration }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "urn:strike-devices:configuration", name = "configuration")
    public JAXBElement<Configuration> createConfiguration(Configuration value) {
        return new JAXBElement<Configuration>(_Configuration_QNAME, Configuration.class, null, value);
    }

}
