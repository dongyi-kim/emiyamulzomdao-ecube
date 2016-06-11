CXX=g++

main: main.o edit.o observe.o auth.o curl_common.o buzzer.o dipsw.o
	$(CXX) -o main $^ -lcurl

main.o: main.cpp
	$(CXX) -c main.cpp

edit.o: edit.cpp
	$(CXX) -c edit.cpp

observe.o: observe.cpp
	$(CXX) -c observe.cpp

auth.o: auth.cpp
	$(CXX) -c auth.cpp

curl_common.o: curl_common.cpp
	$(CXX) -c curl_common.cpp

buzzer.o: buzzer.cpp
	$(CXX) -c buzzer.cpp

dipsw.o: dipsw.cpp
	$(CXX) -c dipsw.cpp

clean:
	rm *.o