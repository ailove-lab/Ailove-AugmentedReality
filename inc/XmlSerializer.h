#ifndef _XML_SERIALIZER_
#define _XML_SERIALIZER_

#define TIXML_USE_TICPP
#define TIXML_USE_STL
#define _UNICODE

#include <tinyxml/ticpp.h>
#include <vector>
#include <list>
#include <string>


//-------------------------------------------------------------------------------------

/** Associates variable to be saved with node name */
template<class T>
struct Member {
    const std::string name;
    T& object;

    Member(const std::string& name, T& object);
    Member(const char* name, T& object);
};

template<class U, class T>
Member<T> member(U name, T& t);

#define MEMBER(name) member(#name, name)

//-------------------------------------------------------------------------------------

class XmlSerializer;

/** Helps to serialize collections*/
template<class Col>
struct CollectionHelper {
    int size;
    Col& collection;

    CollectionHelper(Col& col);
    void serialize(XmlSerializer& sr);
};

//-------------------------------------------------------------------------------------

/** Helps to save complex data to attributes */
template<class T>
struct XmlAttributeHelper {
    
    std::list<T*> data;
    XmlAttributeHelper<T>& operator % (T& t) {
        data.push_back(&t);
        return *this;
    }
};

//-------------------------------------------------------------------------------------

/** Class wraps functionality of TinyXML and enables easy loading and saving objects' data */
class XmlSerializer {
public:

    /** Saves object <code>t</code> to a document with the given name.*/
	template<class T>
	static void save(const std::string& docName, const std::string& rootName, T& t, bool extendedView = false);

    /** Loads object <code>t</code> from a document with the given name.*/
    template<class T>
	static bool load(const std::string& docName, const std::string& rootName, T& t, bool extendedView = false);

    /** Serializes common member.*/
   	template<class T>
	XmlSerializer& operator %(Member<T>& t);

    /** Serializes collection of type <code>T</code> member.*/
    template<class T>
    XmlSerializer& operator % (Member<std::list<T> >& atr);
    template<class T>
    XmlSerializer& operator % (Member<std::vector<T> >& atr);

    /** Serializes simple data objects to attributes.*/
    template<class T>
    XmlSerializer& operator % (Member<XmlAttributeHelper<T> >& atr);
    XmlSerializer& operator % (Member<bool>& atr)   { return serializeAttribute(atr); }
    XmlSerializer& operator % (Member<int>& atr)    { return serializeAttribute(atr); }
	XmlSerializer& operator % (Member<float>& atr)  { return serializeAttribute(atr); }
    XmlSerializer& operator % (Member<double>& atr) { return serializeAttribute(atr); }
    XmlSerializer& operator % (Member<std::string>& atr) { return serializeAttribute(atr); }


    bool isLoading()                                { return loading;}

private:
    
    template<class Col>
    XmlSerializer& serializeCollection(Member<Col>& atr);

    template<class T>
	XmlSerializer& serializeAttribute (Member<T>& atr);
    
    template<class T>
    void getAttribute(const std::string& name, T& t);

    template<class T>
    void setAttribute(const std::string& name, T& t);
	
	
    XmlSerializer(ticpp::Element* element, bool loading, bool extendedView = false) {
	    this->element = element;
        this->loading = loading;
        this->extendedView =extendedView;
    }

    XmlSerializer() {}

private:
    bool loading;
    bool extendedView;
	ticpp::Element* element;
};

#include <XmlSerializerImpl.cxx>

//-------------------------------------------------------------------------------------

#endif
