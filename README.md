# Slink
A web crawler in C++ which takes as input two cities and shows bus schedule between them.<br/>
It sends request to travelyaari.com and stores the response in Redis cache.

It can be used in the following two modes:
* A command line interface and, <br/>
* A webapp using codeIgniter MVC framework

### Libraries
---
* [hiredis](https://github.com/redis/hiredis) - C wrapper for Redis <br/>
* [rapidjson](https://github.com/miloyip/rapidjson) - for parsing raw JSON repsonse 

### C++ Files:
---
* crawl_cli.cpp - for command line interface.<br/>
* crawl_server.cpp - server for codeIgnitor MVC framework

### Installation:
---
* First install Redis server using the following command:<br/>
  `sudo apt-get install redis-server`

* Clone this repository:<br/>
   `git clone https://github.com/prashkr/slink.git`

* Run `make install`

* Run `make compile`

### Running:
---
* Start redis server by typing `redis-server` on terminal.

* For command line interface<br/>
	   `./crawl_cli <source_city> <destination_city>`<br/>
	   `e.g. ./crawl_cli Delhi Mumbai`
	
* For Webapp
	* Copy codeIgnitor into your server folder so you can access the webapp on `http://localhost` <br/>
		    `If you don't have a server running then you can use php to create a server` <br/>
		    `cd CodeIgniter-3.0.0`<br/>
		    `Run "php -S 127.0.0.1:80"`<br/>
		    `Now you can see the webapp on http://localhost`<br/>
	* Run `./crawl_server` so that server starts listening for request from Webapp.<br/>
