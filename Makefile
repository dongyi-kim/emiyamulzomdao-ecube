main: main.o edit.o observe.o auth.o curl_common.o buzzer.o dipsw.o
	g++ -o main $^ -lcurl

main.o: main.cpp
	g++ -c main.cpp

edit.o: edit.cpp
	g++ -c edit.cpp

observe.o: observe.cpp
	g++ -c observe.cpp

auth.o: auth.cpp
	g++ -c auth.cpp

curl_common.o: curl_common.cpp
	g++ -c curl_common.cpp

buzzer.o: buzzer.cpp
	g++ -c buzzer.cpp

dipsw.o: dipsw.cpp
	g++ -c dipsw.cpp