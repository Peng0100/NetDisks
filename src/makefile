main : main.o factory.o server.o time.o tcp_conn.o channel.o database.o /
	g++ -o main main.o factory.o server.o time.o tcp_conn.o channel.o database.o -I /usr/include/mysql/ -lmysqlclient -lpthread

main.o : main.cc /
	g++ -c main.cc 

factory.o : factory.cc /
	g++ -c factory.cc

server.o : server.cc /
	g++ -c server.cc

time.o : time.cc /
	g++ -c time.cc

tcp_conn.o : tcp_conn.cc /
	g++ -c tcp_conn.cc

channel.o : channel.cc /
	g++ -c channel.cc

database.o : database.cc /
	g++ -c database.cc 

clean : 
	rm main.o factory.o server.o time.o tcp_conn.o channel.o database.o
