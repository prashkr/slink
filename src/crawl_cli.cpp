#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <ctime>
#include <ios>
#include <algorithm>
#include "../include/hiredis/hiredis.h"
#include "../include/rapidjson/document.h"

using namespace rapidjson;
using namespace std;

//function to convert integer to string
string inttostr(int n)
{
	stringstream ss;
	ss << n;
	string str = ss.str();
	return str;
}

//This function talks to the redis server and gets schedule from travelyaari.com
string response(string source, string destination){

	cout<<"Searching bus schedule between "<<source<<" and "<<destination<<endl;

	//converting to lowercase
	transform(source.begin(), source.end(), source.begin(), ::tolower);
	transform(destination.begin(), destination.end(), destination.begin(), ::tolower);

	//Day of the week calculation
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	string t = asctime(timeinfo);
	stringstream ss;
	ss<<t<<endl;
	string day;
	ss>>day;

	//Key for Redis cache
	string key = source+destination+day;

	//Redis cache check
	redisContext *c;
	redisReply *reply1, *reply2;
	const char *hostname = "127.0.0.1";
	int port = 6379;
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	c = redisConnectWithTimeout(hostname, port, timeout);
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	cout<<"Looking for Key: "<<key<<endl;

	reply1 = (redisReply* )redisCommand(c,"GET %s", key.c_str());

	//if key not found then fetch schedule from travelyaari.com
	if(reply1->str==NULL){
		cout<<"Fetching Schedule from Travelyaari"<<endl;

		//wget will save api response in a file called "schedule.json"
		string url = "'http://www.travelyaari.com/api/search/?fromCity=" +source+ "&toCity=" +destination+"'";
		string command = "wget --quiet -O schedule.json "+url;
		system(command.c_str());

		string JSON = "";
		string line = "";

		//copy contents of 'schedule.json' into JSON string
		ifstream myfile ("schedule.json");
		if (myfile.is_open())
		{
			while ( getline (myfile,line) )
			{
				JSON = JSON + line;
			}
			myfile.close();
			//removing temporary schedule file
			command = "rm schedule.json";
			system(command.c_str());
		}
		else
			cout << "Unable to open file"; 

		//dump (key, value) pair in redis cache
		reply2 = (redisReply* )redisCommand(c,"SET %s %s", key.c_str(), JSON.c_str());
		freeReplyObject(reply2);
		return JSON;
	}
	else{
		
		//if key found then fetch from redis cache
		cout<<"Fetching from Redis cache"<<endl;
		string info(reply1->str);
		freeReplyObject(reply1);	
		return info;
	}
}

//parsing raw JSON output into csv file
//This function uses 'rapidjson' library
//These library function are defined under 'rapidjson' namespace
string parseResponse(string response){

	Document document;
	document.Parse(response.c_str());

	const Value& a = document["data"]["routes"];
	
	if(a.Size()==0)
		cout<<"Sorry! No Buses currently available"<<endl;

	//writing to csv
	ofstream log("schedule.csv");
	string line = "CompanyName, DepartureTime, ArrivalTime, Fare, AvailableSeats\n";
	log<<line;
	string data =line;
	
	for (SizeType i = 0; i < a.Size(); i++){ // Uses SizeType instead of size_t
		string company = a[i]["CompanyName"].GetString();
		string departure = a[i]["DepartureTime"].GetString();
		departure[10]=' ';
		string arrival = a[i]["ArrivalTime"].GetString();
		arrival[10] = ' ';
		int fare = a[i]["Fare"].GetInt();
		int availableSeats = a[i]["AvailableSeats"].GetInt();
		line = company+ ", " +departure+ ", " +arrival+ ", " +inttostr(fare)+ ", " +inttostr(availableSeats)+ "\n";	
		data = data + line;
	} 
	log<<data;
	return data;
}

int main(int argc, char *argv[]){

	if(argv[1]==NULL || argv[2]==NULL){
		cout<<"Invalid Syntax!\nCorrect Usage: ./a.out <source> <destination>"<<endl;
		return 0;
	}
	
	string src(argv[1]);
	string dest(argv[2]);

	//Fetching response from travelyaari.com
	string resp = response(src, dest);

	//Parsing response
	resp = parseResponse(resp);
	cout<<resp<<endl;

	return 0;
}