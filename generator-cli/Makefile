TARGET = cellar-generator
SOURCES = main.cpp json.cpp markdown.cpp helper.cpp page_rom.cpp page_author.cpp list_roms.cpp list_authors.cpp \
	      include/md4c/md4c.c include/md4c/md4c-html.c include/md4c/entity.c
BUILD_FOLDER = build

CC = g++
OBJECTS = $(SOURCES:%.cpp=$(BUILD_FOLDER)/%.o)

default: $(BUILD_FOLDER) $(BUILD_FOLDER)/$(TARGET)

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(BUILD_FOLDER)/$(TARGET): $(OBJECTS)
	$(CC) -O3 -MMD -MP -o $@ $(OBJECTS) -lm

$(BUILD_FOLDER)/%.o: %.cpp
	$(CC) -O3 -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_FOLDER)

.PHONY: default clean