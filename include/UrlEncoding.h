#pragma once
#include <string>
#include <stdint.h>
#include <iostream>
#include <windows.h>

using namespace std;

namespace bird2fish
{
#ifdef _WIN32

	class StringEncoding
	{
	public:
	
		static int multi2uni(const std::string& multi, std::wstring& uni, UINT code)
		{
			auto len = MultiByteToWideChar(code, 0, multi.c_str(), -1, nullptr, 0);
			if (len <= 0)
			{
				std::cerr << __FILE__ << " : " << __LINE__ << " : " << GetLastError() << std::endl;
				return -1;
			}
			WCHAR* buf = new WCHAR[len];
			if (buf == nullptr)
			{
				std::cerr << __FILE__ << " : " << __LINE__ << " : " << "can not new buf, size : " << len << std::endl;
				return -2;
			}
			len = MultiByteToWideChar(code, 0, multi.c_str(), -1, buf, len);
			uni.assign(buf);
			delete[]buf;
			buf = nullptr;
			return len;
		}

		static int uni2multi(const std::wstring& uni, std::string& multi, UINT code)
		{
			auto len = WideCharToMultiByte(code, 0, uni.c_str(), -1, nullptr, 0, nullptr, nullptr);
			if (len <= 0)
			{
				std::cerr << __FILE__ << " : " << __LINE__ << " : " << GetLastError() << std::endl;
				return -1;
			}
			CHAR* buf = new CHAR[len];
			if (buf == nullptr)
			{
				std::cerr << __FILE__ << " : " << __LINE__ << " : " << "can not new buf, size : " << len << std::endl;
				return -2;
			}
			len = WideCharToMultiByte(code, 0, uni.c_str(), -1, buf, len, nullptr, nullptr);
			multi.assign(buf);
			delete[]buf;
			buf = nullptr;
			return len;
		}
	#endif

		// ANSI->Unicode
		static int ansi2uni(const std::string& ansi, std::wstring& uni)
		{
	#ifdef _WIN32
			return multi2uni(ansi, uni, CP_ACP);
	#endif
			return 0;
		}

		// Unicode->ANSI
		static int uni2ansi(const std::wstring& uni, std::string& ansi)
		{
	#ifdef _WIN32
			return uni2multi(uni, ansi, CP_ACP);
	#endif
			return 0;
		}

		// UTF8->Unicode
		static int utf82uni(const std::string& utf8, std::wstring& uni)
		{
	#ifdef _WIN32
			return multi2uni(utf8, uni, CP_UTF8);
	#endif
			return 0;
		}

		// Unicode->UTF8
		static int uni2utf8(const std::wstring& uni, std::string& utf8)
		{
	#ifdef _WIN32
			return uni2multi(uni, utf8, CP_UTF8);
	#endif
			return 0;
		}

		// ANSI->UTF8
		static int ansi2utf8(const std::string& ansi, std::string& utf8)
		{
			std::wstring uni;
			auto len = ansi2uni(ansi, uni);
			if (len <= 0)
			{
				return -3;
			}
			return uni2utf8(uni, utf8);
		}

		static string ansiToUtf8(const string& ansi)
		{
			string utf8;
			StringEncoding::ansi2utf8(ansi, utf8);
			return utf8;
		}

		// UTF8->ANSI
		static int utf82ansi(const std::string& utf8, std::string& ansi)
		{
			std::wstring uni;
			auto len = utf82uni(utf8, uni);
			if (len <= 0)
			{
				return -3;
			}
			return uni2ansi(uni, ansi);
		}
		static string utf8ToAnsi(const string& str)
		{
			string ansi;
			StringEncoding::utf82ansi(str, ansi);
			return ansi;
		}

	};

	

	//原文链接：https ://blog.csdn.net/history123/article/details/118267614
	class UrlEncoding
	{
	public:
		static char dec2hexChar(short int n) {
			if (0 <= n && n <= 9) {
				return char(short('0') + n);
			}
			else if (10 <= n && n <= 15) {
				return char(short('A') + n - 10);
			}
			else {
				return char(0);
			}
		}

		static short int hexChar2dec(char c) {
			if ('0' <= c && c <= '9') {
				return short(c - '0');
			}
			else if ('a' <= c && c <= 'f') {
				return (short(c - 'a') + 10);
			}
			else if ('A' <= c && c <= 'F') {
				return (short(c - 'A') + 10);
			}
			else {
				return -1;
			}
		}

		//特殊字符的含义
		//URL编码
		//
		//	# 用来标志特定的文档位置  		 % 23
		//
		//	% 对特殊字符进行编码        	 % 25
		//
		//	& 分隔不同的变量值对             % 26
		//
		//	+ 在变量值中表示空格     		 % 2B
		//
		//	/ 表示目录路径                   % 2F
		//
		//	\          表示目录路径          % 5C
		//
		//	= 用来连接键和值                 % 3D
		//
		//	? 表示查询字符串的开始           % 3F
		//
		//	空格    空格                     % 20
		//
		//	.句号                            % 2E
		//
		//	: 冒号                           % 3A
		//	————————————————
		//	版权声明：本文为CSDN博主「HAN - Kai」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
		//	原文链接：https ://blog.csdn.net/hankai1024/article/details/8233366

		static string escapeURL(const string& URL)
		{
			string result = "";
			for (unsigned int i = 0; i < URL.size(); i++) {
				char c = URL[i];
				if (
					('0' <= c && c <= '9') ||
					('a' <= c && c <= 'z') ||
					('A' <= c && c <= 'Z') ||
					c == '=' || c == '.' || c == '_' || c == '&'
					) {
					result += c;
				}
				else {
					int j = (short int)c;
					if (j < 0) {
						j += 256;
					}
					int i1, i0;
					i1 = j / 16;
					i0 = j - i1 * 16;
					result += '%';
					result += dec2hexChar(i1);
					result += dec2hexChar(i0);
				}
			}
			return result;
		}

		static string escapeURLAnsi(const string& ansi)
		{
			return UrlEncoding::escapeURL(StringEncoding::ansiToUtf8(ansi));
		}

		static string deescapeURL(const string& URL) {
			string result = "";
			for (unsigned int i = 0; i < URL.size(); i++) {
				char c = URL[i];
				if (c != '%') {
					result += c;
				}
				else {
					char c1 = URL[++i];
					char c0 = URL[++i];
					int num = 0;
					num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
					result += char(num);
				}
			}
			return result;
		}

		static string deescapeURLAnsi(const string& URL)
		{
			return StringEncoding::utf8ToAnsi(UrlEncoding::deescapeURL(URL));
		}

	};// endof 

} // namespace



