FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp -lpthread

.PHONY:all
all:Test Server

Test:db_test.cpp db.hpp
	g++ -o $@ db_test.cpp $(FLAG)

Server:blog_server.cpp db.hpp 
	g++ -o $@ blog_server.cpp $(FALG)

.PHONY:clean
clean:
	rm Test Server
