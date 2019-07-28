FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp -lpthread -g

.PHONY:all
all:Server

Test:db_test.cpp db.hpp
	g++ -o $@ db_test.cpp $(FLAG)

Server:blog_server.cpp db.hpp httplib.h
	g++ -o $@ blog_server.cpp $(FLAG) 

.PHONY:clean
clean:
	rm Server
