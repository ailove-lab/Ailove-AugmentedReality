
#ifndef _XML_SERIALIZER_
#error XmlSerializerImpl.cxx should not be compiled directly, but only if included from XmlSerialzier.h
#endif

//-------------------------------------------------------------------------------------

template<class T>
Member<T>::Member(const std::string& name, T& object) 
    : name(name), object(object) 
{
}

//-------------------------------------------------------------------------------------

template<class T>
Member<T>::Member(const char* name, T& object) 
    : name(name), object(object) {
}

//-------------------------------------------------------------------------------------

template<class U, class T>
Member<T> member(U name, T& t) 

{
    return Member<T>(name, t);
}

//-------------------------------------------------------------------------------------

template<class Col>
CollectionHelper<Col>::CollectionHelper(Col& col) 
    : collection(col), size(col.size()) 
{}

//-------------------------------------------------------------------------------------

template<class Col>
void CollectionHelper<Col>::serialize(XmlSerializer& sr) 

{
	sr % member(std::string("size"), size);
    
    if (sr.isLoading()) {
        collection.clear();
        collection.resize(size);
    }
            
    int i = 0;
    for(typename Col::iterator it=collection.begin(); collection.end() != it; ++it) {
        std::stringstream convert;
        convert << "v" << i++;
        sr % member(convert.str(), *it);
    }
}

//-------------------------------------------------------------------------------------
    
template<class T>
void XmlSerializer::save(const std::string& docName, const std::string& rootName, T& t, bool extendedView) 

{
    ticpp::Document doc;
    ticpp::Declaration decl( "1.0", "", "" );
    doc.LinkEndChild(&decl);

    ticpp::Element e(rootName);
    doc.LinkEndChild(&e);
    XmlSerializer srl(&e, false, extendedView);
	t.serialize(srl);

    doc.SaveFile(docName.c_str());
}

//-------------------------------------------------------------------------------------

template<class T>
bool XmlSerializer::load(const std::string& docName, const std::string& rootName, T& t, bool extendedView) 

{
    ticpp::Document doc;
    doc.LoadFile(docName.c_str());

    ticpp::Element* element = doc.FirstChildElement(rootName);
    XmlSerializer srl(element, true, extendedView);
	t.serialize(srl);
    return true;
}

//-------------------------------------------------------------------------------------

struct Store {
    Store(ticpp::Element* &element) : element(element), value(element) {}
    ~Store() {element = value;}
    ticpp::Element* &element;
    ticpp::Element* value;
};

//-------------------------------------------------------------------------------------

template<class T>
XmlSerializer& XmlSerializer::operator %(Member<T>& t) {
    Store temp(element);
        
    if (isLoading()) {
        element = element->FirstChildElement(t.name);
		t.object.serialize(*this);
    } else {
        ticpp::Element e(t.name);
        element->LinkEndChild(&e);
        element = &e;     
		t.object.serialize(*this);
    }

	return *this;
}

//-------------------------------------------------------------------------------------

template<class T>
XmlSerializer& XmlSerializer::operator % (Member<std::list<T> >& atr) 

{
    CollectionHelper<std::list<T> > helper(atr.object);
    *this % member(atr.name, helper);
    return *this;
}

//-------------------------------------------------------------------------------------

template<class Col>
XmlSerializer& XmlSerializer::serializeCollection(Member<Col>& atr) 

{
    std::string name = atr.name;
    for(typename Col::iterator it = atr.object.begin(); atr.object.end() != it; ++it) {
        *this % member(name, *it);
    }
    return *this;
}

//-------------------------------------------------------------------------------------

template<class T>
XmlSerializer& XmlSerializer::serializeAttribute (Member<T>& atr) 

{
    if (isLoading()) {
		getAttribute(atr.name, atr.object);
    } else {
        setAttribute(atr.name, atr.object);
    }
	return *this;
}

//-------------------------------------------------------------------------------------

template<class T>
void XmlSerializer::getAttribute(const std::string& name, T& t) 

{
    if (extendedView) {
        ticpp::Element *e = element->FirstChildElement(name);
        e->GetAttribute(name, &t, false);
    } else {
        element->GetAttribute(name, &t, false);
    }
}

//-------------------------------------------------------------------------------------

template<class T>
void XmlSerializer::setAttribute(const std::string& name, T& t) 

{
    if (extendedView) {
        ticpp::Element e(name);
        element->LinkEndChild(&e);
        e.SetAttribute(std::string("value"), t);
    } else {
        element->SetAttribute(name, t);
    }
}

//-------------------------------------------------------------------------------------

 template<class T>
 XmlSerializer& XmlSerializer::operator % (Member<XmlAttributeHelper<T> >& atr) { 
    std::list<T*>& data = atr.object.data;
    if (isLoading()) {
        std::string value;
        getAttribute(atr.name, value);
        std::istringstream iss(value);
        for(typename std::list<T*>::iterator it = data.begin(); data.end() != it; ++it) {
            iss >> *(*it);
        }
    } else {
        std::ostringstream oss;
        for(typename std::list<T*>::iterator it = data.begin(); data.end() != it; ++it) {
            oss << *(*it)<<' ';
        }
        setAttribute(atr.name, oss.str());
    }
    return *this;
}

 //-------------------------------------------------------------------------------------
