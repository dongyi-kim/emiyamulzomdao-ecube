CXX=arm-none-linux-gnueabi-g++

main: main.o edit.o observe.o auth.o curl_common.o buzzer.o dipsw.o fnd.o mled.o bled.o oled.o cled.o tlcd.o receiveSensor.o thread_manager.o
	
	$(CXX) -o main $^ -lcurl -lpthread

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

fnd.o: fnd.cpp
	$(CXX) -c fnd.cpp

mled.o: mled.cpp
	$(CXX) -c mled.cpp

bled.o: bled.cpp
	$(CXX) -c bled.cpp

oled.o: oled.cpp
	$(CXX) -c oled.cpp

cled.o: cled.cpp
	$(CXX) -c cled.cpp

tlcd.o: tlcd.cpp
	$(CXX) -c tlcd.cpp

receiveSensor.o: receiveSensor.cpp
	$(CXX) -c receiveSensor.cpp

touch.o: gui/touch.cpp
	$(CXX) -c gui/touch.cpp

thread_manager.o: thread_manager.cpp
	$(CXX) -c thread_manager.cpp


clean:
	rm *.o