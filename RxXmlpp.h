#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#pragma comment(lib, "libxml2.lib")
using namespace  std;

namespace bird2fish
{
	class Xml2Init
	{
	public :
		Xml2Init()
		{
			// 初始化解析器
			xmlInitParser();
		}
		~Xml2Init()
		{
			xmlCleanupParser();
		}
	};

	class Xml2Attr
	{
	public:
		Xml2Attr(xmlAttrPtr ptr) { this->attr = ptr; }
		Xml2Attr(const Xml2Attr& other) { this->attr = other.attr; }
		Xml2Attr(const Xml2Attr&& other) { this->attr = other.attr; }

		string getName() {
			return (char *)attr->name;
		}
		string getValue()
		{
			return (char*)attr->children->content;
		}
		bool isNull() { 
			return attr == nullptr;  
		}
	private:
		Xml2Attr() = delete;
		
		xmlAttrPtr attr;
	};

	using Xml2AttrArray = std::vector< Xml2Attr>;

	class Xml2Node
	{
	public:
		Xml2Node(xmlNodePtr _node);
		Xml2Node(const Xml2Node& other);
		Xml2Node(const Xml2Node&& other);
		~Xml2Node();

		string getName();
		string getValue();
		int    getType();
		bool   isNull() { return nullptr == node;  }

		Xml2AttrArray getAttrsAll();
		Xml2Attr getAttrByName(const string& attrName);

		Xml2Attr hasAttr(const string& attrName, const string& attrVal);
		
		// 获取当前所有子节点,可以选择分类
		std::vector<Xml2Node> getChildren(int typeMask);
		std::vector<Xml2Node> getChildrenElements();

		// 获取一级子节点中，匹配名字，带某个属性名字的
		std::vector<Xml2Node> getChildrenByName(const string& eleName, const string& attrName = "", const string& attrVal = "");

		std::vector<Xml2Node> getDeepElementsByName(const string& eleName, const string& attrName = "", const string& attrVal = "");
		std::vector<Xml2Node> xpath(const string& npath);
		Xml2Node getParent();
		Xml2Node getPreBrother();
		Xml2Node getNextBrother();
		Xml2Node getChildByIndex(const string &name, int index);
		

	private:
		Xml2Node() = delete;
		
		xmlXPathObjectPtr getNodeset(const xmlChar* xpath);

		xmlNodePtr node;
		
	};

	using Xml2NodeArray = std::vector<Xml2Node> ;


	class Xml2Doc
	{
	public:
		Xml2Doc(xmlDocPtr _doc, xmlParserCtxtPtr _ctxt);
		Xml2Doc(Xml2Doc&& _other);
		~Xml2Doc();

		static Xml2Doc tryParseFile(const string& filepath, const string& charset, int opt);
		static Xml2Doc tryParseString(const string& content, const string& charset, int opt);

		string dump();
		bool isNull();
		string getErr();
		Xml2Node getRoot();
		Xml2NodeArray getByXPath(const string& xpath);
		Xml2NodeArray getByName(const string& eleName, const string& attrName = "", const string& attrVal = "");
		

	private:
		xmlXPathObjectPtr getNodeset(const xmlChar* xpath);
		Xml2Doc() = delete;
		Xml2Doc(Xml2Doc& _other) = delete;
		
		

		xmlDocPtr doc;
		xmlParserCtxtPtr ctxt;
		string errMsg;
		int    errCode;
	};
	
	

}


