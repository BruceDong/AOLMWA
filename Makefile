CC=g++
CFLAGS=-c -Wall -g -rdynamic
LDFLAGS=
SOURCES=src/DependencyParsing/main.cpp src/DependencyParsing/DependencyPaser.cpp src/DependencyParsing/Environment.cpp src/DependencyParsing/Simulator.cpp src/DependencyParsing/WordAgent.cpp \
		src/DependencyParsing/Model.cpp src/DependencyParsing/Predictor.cpp src/DependencyParsing/Trainer.cpp src/DependencyParsing/FeatureTemplate.cpp src/DependencyParsing/Evaluation.cpp src/DependencyParsing/Test.cpp
OBJECTS=$(patsubst src/DependencyParsing%.cpp, obj/%.o, $(SOURCES))
EXECUTABLE=bin/test

$(shell [ -d bin ] || mkdir -p bin)
$(shell [ -d obj ] || mkdir -p obj)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

obj/%.o: src/DependencyParsing/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
