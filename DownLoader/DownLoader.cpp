// DownLoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include "curl/curl.h"

#pragma comment(lib, "libcurl.a")
using namespace std;

size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//cout << "----->reply" << endl;
	string *str = (string*)stream;
	//cout << *str << endl;
	(*str).append((char*)ptr, size*nmemb);
	return size * nmemb;
}


CURLcode curl_get_req(const std::string &url, std::string &response)
{
	// init curl
	CURL *curl = curl_easy_init();
	// res code
	CURLcode res;
	if (curl)
	{
		// set params
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120); // set transport and time out time
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120);
		// start req
		res = curl_easy_perform(curl);
	}
	// release curl
	curl_easy_cleanup(curl);
	return res;
}
char indicator[] = { 0x0D, 0x0A, 0x0D, 0x0A, 0x50, 0x4B };
string getData(string &response)
{
	int pos = response.find(indicator);
	if (pos == -1)
		return {};
	return response.substr(pos + 4);
}

void workingProcess(int index)
{
	for (int i = 0; i < 100; i++)
	{
		string getUrlStr = "http://x5music-40020.sh.gfp.tencent-cloud.com/all_" + to_string(index+i) +".zip";
		string getResponseStr;
		auto res = curl_get_req(getUrlStr, getResponseStr);
		if (res != CURLE_OK)
			continue;
		auto ret = getData(getResponseStr);
		if (ret.size() < 2000)
			continue;
		std::ofstream fout(to_string(index + i) + ".zip", std::ios::binary);	
		fout.write(ret.c_str(), ret.size());
		fout.close();
	}
	
}
int main()
{

	curl_global_init(CURL_GLOBAL_ALL);
	// test get requery
	string getUrlStr = "http://x5music-40020.sh.gfp.tencent-cloud.com/all_100599.zip";
	
	vector<thread> Workers;
	for (int i = 0; i < 30; i++)
	{
		int index = 100000 + i * 100;
		Workers.emplace_back(workingProcess, index);
	}
	for (int i = 0; i < 30; i++)
	{
		Workers[i].join();
	}
	curl_global_cleanup();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
