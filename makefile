CXX = g++
CFLAGS = -pthread -std=c++17 -Wall -Wextra -Weffc++

SOURCES = \
	algorithms/naive.cpp \
	algorithms/rajko-aluru.cpp \
	algorithms.cpp\
	utils.cpp \
	main.cpp 

OBJECTS = \
	utils.o \
	main.o 

exec: main.cpp utils.cpp algorithms.cpp algorithms/rajko-aluru.cpp algorithms/naive.cpp
	$(CXX) main.cpp  $(CFLAGS) -o exec 

# exec: $(OBJECTS)
# 	$(CXX) $(CFLAGS) -o exec $(OBJECTS) 

# naive.o: algorithms/naive.cpp
# 	$(CXX) -c $(CFLAGS) -o naive.o algorithms/naive.cpp

# rajko-aluru.o: algorithms/rajko-aluru.cpp
# 	$(CXX) -c $(CFLAGS) -o rajko-aluru.o algorithms/rajko-aluru.cpp

# algorithms.o: algorithms.cpp naive.o rajko-aluru.o
# 	$(CXX) -c $(CFLAGS) -o algorithms.o algorithms.cpp

# utils.o: utils.cpp
# 	$(CXX) -c $(CFLAGS) -o utils.o utils.cpp

# main.o: main.cpp utils.cpp algorithms.cpp algorithms/naive.cpp algorithms/rajko-aluru.cpp
# 	$(CXX) -c $(CFLAGS) -o main.o main.cpp

clean:
	rm -f *.o
	rm -f exec