modmul : $(wildcard *.hpp) $(wildcard *.cpp)
	@${CXX} -Wall -Wextra -std=c++0x -O3 -o ${@} $(filter %.cpp, ${^})

.DEFAULT_GOAL = all

all   : modmul

clean :
	@rm -f modmul
