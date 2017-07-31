modmul : $(wildcard *.hpp) $(wildcard *.cpp)
	@${CXX} -Wall -Wextra -std=c++11 -O0 -g -o ${@} $(filter %.cpp, ${^})

.DEFAULT_GOAL = all

all   : modmul

clean :
	@rm -f modmul
