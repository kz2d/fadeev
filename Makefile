compile:
	g++ 1/main.cpp -std=c++2a -o 1.out
	g++ 2/main.cpp -std=c++2a -ljsoncpp -lcurl -o 2.out
	g++ 3/CircularBuffer.h -std=c++2a -o 3.out
	g++ 5/main.cpp -std=c++2a -o 5.out -I
	g++ 6/Polynom.cpp -std=c++2a -o 6.out