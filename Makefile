server:
	g++ -O3 transferStatus.cpp config.cpp server*.cpp -o server -ggdb

client:
	g++ -O3 transferStatus.cpp config.cpp client*.cpp -o client -ggdb

clean:
	rm server || rm client
