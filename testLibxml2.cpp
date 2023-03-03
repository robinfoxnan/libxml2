// testLibxml2.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include <stdio.h>
#include <string>
#include "RxXmlpp.h"

#include "include/UrlEncoding.h"

using namespace  std;
using namespace bird2fish;



void printAttrs(Xml2Node & node)
{
	auto vec = node.getAttrsAll();
	for (auto& it : vec)
	{
		string val = StringEncoding::utf8ToAnsi(it.getValue());
		std::cout << it.getName() << ": " << val << ", ";

	}

}

void testQQMusic()
{
	string fileName = "D:\\TBuild\\�ĵ�\\QQ����-ǧ���������ֺ������������¸��ȸ����쳩���ĸ�Ʒ������ƽ̨��.html";
	int opt = XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_NOERROR;
	Xml2Doc doc = Xml2Doc::tryParseFile(fileName, "", opt);
	if (doc.isNull())
		return;

	string xpath = "//*[@id='app']//div[@class='songlist__songname']/span/a";

	Xml2NodeArray vec = doc.getByXPath(xpath);
	for (auto& it : vec)
	{
		std::cout << it.getName() << "[ ";
		printAttrs(it);
		std::cout << "] \n ";

	}
}

void testLoadString()
{
	std::string docStr = R"===(<?xml version="1.0" encoding="GBK"?>
<root>
  <person name="Alice" age="25"/>
  <person name="Bob" age="30"/>
  <person name="����" age="35"/>
</root>
)===";
	int opt =  XML_PARSE_NOBLANKS;
	cout << docStr << endl;

	Xml2Doc doc = Xml2Doc::tryParseString(docStr, "GBK", opt);
	if (doc.isNull())
	{
		cout << doc.getErr() << endl;
		return;
	}

	auto root = doc.getRoot();
	Xml2NodeArray vec = root.getChildren(0);
	for (auto &it : vec)
	{
		cout << it.getName() << endl;
		auto attr = it.getAttrByName("name");
		if (!attr.isNull())
		{
			string val = StringEncoding::utf8ToAnsi(attr.getValue());
			cout << attr.getName() <<  "=" << val << endl;
		}
	}
}

void testDeepFind()
{
	string fileName = "D:\\TBuild\\�ĵ�\\��Ч�ز�����-��Ч��ȫ-����.htm";
	int opt = XML_PARSE_RECOVER | XML_PARSE_NOBLANKS | XML_PARSE_NOERROR;
	Xml2Doc doc = Xml2Doc::tryParseFile(fileName, "", opt);
	if (doc.isNull())
		return;

	Xml2NodeArray vec = doc.getByName("div", "class", "SoundContent");
	for (auto& it : vec)
	{
		std::cout << it.getName() << "[ ";

		Xml2NodeArray srcList = it.getDeepElementsByName("source", "", "");
		if (srcList.size() > 0)
		{
			printAttrs(srcList[0]);
		}

		Xml2NodeArray titleList = it.getDeepElementsByName("div", "class", "SoundTitle");
		if (titleList.size() > 0)
		{
			Xml2NodeArray txtList = titleList[0].getChildrenByName("a");
			if (txtList.size() > 0)
			{
				string val = StringEncoding::utf8ToAnsi(txtList[0].getValue());
				cout << val;
			}
				
		}
		

		std::cout << "] \n ";

	}
	

}

int main()
{


	//testQQMusic();

	testDeepFind();

	//testLoadString();

	return 0;
}

