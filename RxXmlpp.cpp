#include "RxXmlpp.h"


using namespace  bird2fish;

// 初始化以及清理资源
static Xml2Init xmlInitAll;


Xml2Node::Xml2Node(xmlNodePtr _node)
{
	node = _node;
}
Xml2Node::~Xml2Node()
{

}

Xml2Node::Xml2Node(const Xml2Node& other)
{
	node = other.node;
}

Xml2Node::Xml2Node(const Xml2Node&& other)
{
	node = other.node;
}

string Xml2Node::getName()
{
	return (char *)node->name;
}
string Xml2Node::getValue()
{
	if (node->type == xmlElementType::XML_ELEMENT_NODE)
	{
		return (char *)node->children->content;
	}

	return (char *)node->content;
}

int   Xml2Node::getType()
{
	return (int)node->type;
}

// 0: get all
Xml2NodeArray Xml2Node::getChildren(int typeMask)
{
	Xml2NodeArray vec;

	for (xmlNodePtr cur = node->children; cur != NULL; cur = cur->next)
	{
		if (typeMask == 0)
			vec.push_back(cur);
		else
		{
			if (cur->type == typeMask)
			{
				vec.push_back(cur);
			}
		}
	}
	return vec;
}

std::vector<Xml2Node> Xml2Node::getChildrenElements()
{
	return std::move(getChildren(xmlElementType::XML_ELEMENT_NODE) );
}

std::vector<Xml2Node> Xml2Node::getChildrenByName(const string& eleName, const string& attrName, const string& attrVal)
{
	Xml2NodeArray vec;

	for (xmlNodePtr cur = node->children; cur != NULL; cur = cur->next)
	{
		if (eleName != (const char*)cur->name)
		{
			continue;
		}

		if (attrName == "")
		{
			vec.push_back(cur);
			continue;
		}
		
		Xml2Node curNode(cur);
		auto attr = curNode.hasAttr(attrName, attrVal);
		if (!attr.isNull())
			vec.push_back(cur);
	}
	return vec;
}
Xml2Attr Xml2Node::hasAttr(const string& attrName, const string& attrVal)
{
	Xml2Attr attr = getAttrByName(attrName);
	if (attr.isNull())
		return nullptr;

	if (attrVal == "")
		return attr;

	string val = attr.getValue();
	if (val == attrVal)
	{
		return attr;
	}

	return nullptr;
}

Xml2AttrArray Xml2Node::getAttrsAll()
{
	Xml2AttrArray vec;

	if (node->type == xmlElementType::XML_ELEMENT_NODE)
	{

		// 遍历属性列表
		xmlAttrPtr attr = node->properties;
		while (attr)
		{
			vec.push_back(attr);
			attr = attr->next;
		}
	}
	return vec;
}

Xml2Attr Xml2Node::getAttrByName(const string& attrName)
{
	if (node->type == xmlElementType::XML_ELEMENT_NODE)
	{

		// 遍历属性列表
		xmlAttrPtr attr = node->properties;
		while (attr)
		{
			//string temp = (const char*)attr->name;
			if (attrName == (const char*)attr->name)
			{
				return attr;
			}

// 			if (attrName.compare((const char*)attr->name) == 0)
// 			{			
// 				return attr;
// 			}

			attr = attr->next;
		}
	}
	return nullptr;
}

int deepFind(xmlNodePtr parent, const string& eleName, const string& attrName, const string& attrVal, std::vector<Xml2Node>& vec)
{
	int count = 0;
	for (xmlNodePtr cur = parent->children; cur != NULL; cur = cur->next)
	{
		if (cur->children)
			deepFind(cur, eleName, attrName, attrVal, vec);

		// 先比较节点名字
		if (eleName.compare((const char*)cur->name) != 0)
		{	
			continue;
		}

		// 是否需要检查属性 ""
		if (attrName.length() == 0)
		{
			count++;
			vec.emplace_back(cur);
			continue;
		}

		Xml2Node curNode(cur);
		auto attr = curNode.hasAttr(attrName, attrVal);
		if (!attr.isNull())
		{
			count++;
			vec.push_back(cur);
		}
	}
	return count;
}


// 手动递归搜索
std::vector<Xml2Node> Xml2Node::getDeepElementsByName(const string& eleName, const string& attrName, const string& attrVal)
{
	Xml2NodeArray vec;

	deepFind(node, eleName, attrName, attrVal, vec);

	return vec;
}


xmlXPathObjectPtr Xml2Node::getNodeset(const xmlChar* xpath)
{
	// 从文档中构建XPath上下文
	xmlXPathContextPtr context = xmlXPathNewContext(node->doc);
	if (context == NULL) {
		printf("Failed to create XPath context.\n");
		return nullptr;
	}

	// 设置上下文的节点
	context->node = node;

	xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("xmlXPathEvalExpression return NULL\n");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}

	return result;
}
// xpath搜素
std::vector<Xml2Node> Xml2Node::xpath(const string& npath)
{
	xmlChar* path = BAD_CAST npath.c_str();
	xmlXPathObjectPtr result = getNodeset(path);
	if (nullptr == result)
	{
		return std::vector<Xml2Node>();
	}

	xmlNodeSetPtr nodeset = result->nodesetval;
	xmlNodePtr cur = nullptr;

	Xml2NodeArray vec;
	for (int i = 0; i < nodeset->nodeNr; i++)
	{
		cur = nodeset->nodeTab[i];

		vec.push_back(cur);
	}

	xmlXPathFreeObject(result);

	return std::move(vec);
}

// 
Xml2Node Xml2Node::getParent()
{
	return node->parent;
}

Xml2Node Xml2Node::getPreBrother()
{
	return node->prev;
}

Xml2Node Xml2Node::getNextBrother()
{
	return node->next;
}


Xml2Node Xml2Node::getChildByIndex(const string& name, int index)
{
	std::vector<Xml2Node> vec = getChildrenByName(name, "", "");

	if (index < 0 || index > vec.size())
	{
		return nullptr;
	}

	return vec[index];

}

////////////////////////////////////////////////////
//Xml2Doc::Xml2Doc()
//{
//	doc = nullptr;
//}

Xml2Doc::Xml2Doc(xmlDocPtr _doc, xmlParserCtxtPtr _ctxt)
{
	this->doc = _doc;
	this->ctxt = _ctxt;
}

Xml2Doc::Xml2Doc(Xml2Doc&& _other)
{
	this->doc = _other.doc;
	_other.doc = nullptr;

	this->ctxt = _other.ctxt;
	_other.ctxt = nullptr;
}

Xml2Doc::~Xml2Doc() 
{
	if (doc)
	{
		// 释放资源
		xmlFreeDoc(doc);
	}
	if (ctxt)
	{
		xmlFreeParserCtxt(ctxt);
	}
}



bool Xml2Doc::isNull()
{
	return this->doc == nullptr;
}

string Xml2Doc::getErr()
{
	return errMsg;
}

Xml2Doc Xml2Doc::tryParseFile(const string& filepath, const string & charset, int opt) 
{
	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();

	const char* lpCharSet = nullptr;
	if (charset != "")
	{
		lpCharSet = charset.c_str();
	}
	xmlDocPtr doc  = xmlCtxtReadFile(ctxt, filepath.c_str(), lpCharSet, opt);
	if (doc == NULL) {
		
		auto mDoc = Xml2Doc(nullptr, nullptr);
		mDoc.errMsg = ctxt->lastError.message;
		mDoc.errCode = ctxt->errNo;

		mDoc.errMsg += std::to_string(mDoc.errCode);

		xmlFreeParserCtxt(ctxt);
		return mDoc;
	}

	return Xml2Doc(doc, ctxt);
}



Xml2Doc Xml2Doc::tryParseString(const string& content, const string& charset, int opt)
{

	xmlParserCtxtPtr ctxt = xmlNewParserCtxt();

	const char *lpCharSet = nullptr;
	if (charset != "")
	{
		lpCharSet = charset.c_str();
	}

	xmlDocPtr doc = 
		xmlCtxtReadMemory(ctxt, content.c_str(), content.size(), nullptr, lpCharSet, opt);
	if (doc == NULL) {

		auto mDoc = Xml2Doc(nullptr, nullptr);
		mDoc.errMsg = ctxt->lastError.message;
		mDoc.errCode = ctxt->errNo;

		mDoc.errMsg += std::to_string(mDoc.errCode);

		xmlFreeParserCtxt(ctxt);
		return mDoc;
	}

	return Xml2Doc(doc, ctxt);
}



Xml2Node Xml2Doc::getRoot()
{
	xmlNodePtr root = xmlDocGetRootElement(doc);
	return root;
}


/**
 * @brief xpathxml

 *
 * @param doc xmlָ
 * @param xpath ath

 * @return xmlXPathObjectPtr LLָlXPathFreeObject

 */
xmlXPathObjectPtr Xml2Doc::getNodeset(const xmlChar* xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(doc);

	if (context == NULL)
	{
		printf("context is NULL\n");
		return NULL;
	}

	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("xmlXPathEvalExpression return NULL\n");
		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}

	return result;
}

Xml2NodeArray Xml2Doc::getByXPath(const string& xpath) 
{
	xmlChar* path = BAD_CAST xpath.c_str();
	xmlXPathObjectPtr result = getNodeset(path);
	if (nullptr == result)
	{
		return std::vector<Xml2Node>();
	}

	xmlNodeSetPtr nodeset = result->nodesetval;
	xmlNodePtr cur = nullptr;

	Xml2NodeArray vec;
	for (int i = 0; i < nodeset->nodeNr; i++)
	{
		cur = nodeset->nodeTab[i];

		vec.push_back(cur);
	}

	xmlXPathFreeObject(result);

	return std::move(vec);
}

Xml2NodeArray Xml2Doc::getByName(const string& eleName, const string& attrName, const string& attrVal) 
{
	Xml2Node root = this->getRoot();
	if (!root.isNull())
	{
		return root.getDeepElementsByName(eleName, attrName, attrVal);
	}
	
	return Xml2NodeArray();
}


string Xml2Doc::dump()
{

	xmlChar* xmlbuf;
	int bufsize;
	xmlDocDumpFormatMemory(doc, &xmlbuf, &bufsize, 1);

	// 处理序列化后的 XML 字符串
	printf("%s", xmlbuf);

	xmlFree(xmlbuf);


	remove("example_out.xml");  // 删除现有文件
	xmlSaveFile("example_out.xml", doc);
	return "";
}