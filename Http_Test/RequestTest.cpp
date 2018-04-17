#include "stdafx.h"
#include "../Http/RequestParser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Http_Test {
	TEST_CLASS(RequestTest) {
public:

	TEST_METHOD(TestGetRequest) {
		auto actualCount= 0;
		std::string expectedPath= "/f/15";
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Host", "localhost:6006" },
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0" },
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" },
			{ "Accept-Language", "en-US,en;q=0.5" },
			{ "Accept-Encoding", "gzip, deflate" },
			{ "DNT", "1" },
			{ "Connection", "keep-alive" },
			{ "Upgrade-Insecure-Requests", "1" },
		};
		auto fn= [&actualCount, &expectedPath, &expectedHeaders](Request const& request) {
			++actualCount;
			Assert::AreEqual(request.Uri.Path, expectedPath);
			Assert::AreEqual(expectedHeaders.size(), request.Headers.size());
			for(auto const& header : expectedHeaders) {
				auto const it= request.Headers.find(header.first);
				if(it != request.Headers.cend()) {
					Assert::AreEqual(it->second, header.second);
				} else {
					Assert::IsTrue(it != request.Headers.cend());
				}
			}
		};
		std::string s= "GET " + expectedPath + " HTTP/1.1\r\n";
		for(auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		auto const* p= s.c_str();
		auto const n= s.size();
		RequestParser outerParser(fn);
		outerParser.Add(p, n);
		auto expectedCount= 1;
		for(int i= 1; i < n; i <<= 1) {
			expectedCount += 2;
			RequestParser innerParser(fn);
			for(int j= 0; j < n; j += i) {
				auto k= j + i >= n ? n - j : i;
				outerParser.Add(p + j, k);
				innerParser.Add(p + j, k);
			}
		}
		Assert::AreEqual(actualCount, expectedCount);
	}

	TEST_METHOD(TestPostRequest) {
		auto actualCount= 0;
		std::string expectedPath= "/w/index.php";
		std::vector<std::pair<std::string, std::string>> expectedQuery{
			{ "title", "Special:UserLogin" },
			{ "returnto", "Main Page" },
		};
		std::string expectedRawQuery= "title=Special:UserLogin&returnto=Main+Page";
		std::vector<std::pair<std::string, std::string>> expectedHeaders{
			{ "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" },
			{ "Accept-Encoding", "gzip, deflate, br" },
			{ "Accept-Language", "en-US,en;q=0.5" },
			{ "Connection", "keep-alive" },
			{ "Content-Length", "214" },
			{ "Content-Type", "application/x-www-form-urlencoded" },
			{ "Cookie", "CP=H2; WMF-Last-Access=16-Apr-2018; WMF-Last-Access-Global=16-Apr-2018; GeoIP=US:CA:Oakland:37.81:-122.30:v4;"
			" enwikiSession=nd17fr3jd652l9acc0ch5aavm48kqfmv; forceHTTPS=true" },
			{ "DNT", "1" },
			{ "Host", "en.wikipedia.org" },
			{ "Referer", "https://en.wikipedia.org/w/index.php?title=Special:UserLogin&returnto=Main+Page" },
			{ "Upgrade-Insecure-Requests", "1" },
			{ "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0" },
		};
		std::string expectedData= "wpName=Username&wpPassword=Password&wploginattempt=Log+in&wpEditToken=%2B%5C"
			"&title=Special%3AUserLogin&authAction=login&force=&wpLoginToken=6e47a09fa7efb01ef29268e7585160945ad4071d%2B%5C"
			"&wpForceHttps=1&wpFromhttp=1";
		auto fn= [&actualCount, &expectedPath, &expectedQuery, &expectedRawQuery, &expectedHeaders, &expectedData](Request const& request) {
			++actualCount;
			Assert::AreEqual(request.Uri.Path, expectedPath);
			for(auto const& nameValue : expectedQuery) {
				auto const it= request.Uri.Query.find(nameValue.first);
				if(it != request.Uri.Query.cend()) {
					Assert::AreEqual(it->second, nameValue.second);
				} else {
					Assert::IsTrue(it != request.Uri.Query.cend());
				}
			}
			Assert::AreEqual(request.Uri.RawQuery, expectedRawQuery);
			Assert::AreEqual(expectedHeaders.size(), request.Headers.size());
			for(auto const& header : expectedHeaders) {
				auto const it= request.Headers.find(header.first);
				if(it != request.Headers.cend()) {
					Assert::AreEqual(it->second, header.second);
				} else {
					Assert::IsTrue(it != request.Headers.cend());
				}
			}
			Assert::AreEqual(request.Data, expectedData);
		};
		std::string s= "POST " + expectedPath + '?' + expectedRawQuery + " HTTP/1.1\r\n";
		for(auto const& header : expectedHeaders) {
			s += header.first + ": " + header.second + "\r\n";
		}
		s += "\r\n";
		s += expectedData;
		auto const* p= s.c_str();
		auto const n= s.size();
		RequestParser outerParser(fn);
		outerParser.Add(p, n);
		auto expectedCount= 1;
		for(int i= 1; i < n; i <<= 1) {
			expectedCount += 2;
			RequestParser innerParser(fn);
			for(int j= 0; j < n; j += i) {
				auto k= j + i >= n ? n - j : i;
				outerParser.Add(p + j, k);
				innerParser.Add(p + j, k);
			}
		}
		Assert::AreEqual(actualCount, expectedCount);
	}
	};
}
