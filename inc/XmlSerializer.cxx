#ifndef _XML_SERIALIZER_
#error XmlSerializer.cxx should not be compiled directly, but only if included from XmlSerializer.h
#endif

//-------------------------------------------------------------------------------------

template<class T>
Member::Member(const std::string& name, T& object) 
    : name(name), object(object) 
{
}

//-------------------------------------------------------------------------------------

Member(const char* name, T& object) 
    : name(name), object(object) 
{
}

//-------------------------------------------------------------------------------------

template<class U, class T>
Member<T> member(U name, T& t) {
    return Member<T>(name, t);
}

//-------------------------------------------------------------------------------------

CollectionHelper::CollectionHelper(Col& col) 
    : collection(col), size(col.size()) 
{
}

//-------------------------------------------------------------------------------------

void CollectionHelper::serialize(XMLSerializer& sr) 

{
	sr % member(std::string("size"), size);

    collection.resize(size);
            
    int i = 0;
    for(Col::iterator it = collection.begin(); collection.end() != it; ++it) {
        std::stringstream convert;
        convert << "v" << i++;
        sr % member(convert.str(), *it);
    }
		    
}

//-------------------------------------------------------------------------------------

XMLSerializer::XMLSerializer(ticpp::Element* element, bool loading)
    : element(element), loading(loading)
{
}

//-------------------------------------------------------------------------------------
    
template<class T>
void XMLSerializer::save(ticpp::Document& doc, const std::string& name, T& t)

{
    ticpp::Element e(name);
    doc.LinkEndChild(&e);
    XMLSerializer srl(&e, false);
	t.serialize(srl);
}

//-------------------------------------------------------------------------------------

template<class T>
void XMLSerializer::load(ticpp::Document& doc, const std::string& name, T& t) 

{
    ticpp::Element* element = doc.FirstChildElement(name);
    XMLSerializer srl(element, true);
	t.serialize(srl);
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
XMLSerializer& XMLSerializer::operator %(Member<T>& t) 

{
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
XMLSerializer& XMLSerializer::operator % (Member<std::list<T> >& atr) 

{
    CollectionHelper<std::list<T> > helper(atr.object);
    *this % member(atr.name, helper);
    return *this;
}

//-------------------------------------------------------------------------------------

template<class Col>
XMLSerializer& XMLSerializer::serializeCollection(Member<Col>& atr) 
{
    std::string name = atr.name;
    for(typename Col::iterator it = atr.object.begin(); atr.object.end() != it; ++it) {
        *this % member(name, *it);
    }
    return *this;
}

//-------------------------------------------------------------------------------------

template<class T>
XMLSerializer& XMLSerializer::serializeAttribute (Member<T>& atr) 

{
    if (isLoading()) {
		element->GetAttribute(atr.name, &atr.object);
    } else {
        element->SetAttribute(atr.name, atr.object);
    }
	return *this;
}

//-------------------------------------------------------------------------------------

template<class T>
void XMLSerializer::getAttribute(const std::string& name, T& t) 

{
    element->getAttribute(name, &t);
}

//-------------------------------------------------------------------------------------

template<class T>
void XMLSerializer::setAttribute(const std::string& name, T& t) 

{
    element->setAttribute(name, &t);
}

//-------------------------------------------------------------------------------------