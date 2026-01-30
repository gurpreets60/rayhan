CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = editor
SOURCES = vm.c parser.c editor.c
HEADERS = vm_defs.h parser.h

INSTALL_DIR = /usr/local/bin

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o

install: all
	@echo "Installing $(TARGET) to $(INSTALL_DIR)..."
	mkdir -p $(INSTALL_DIR)
	cp $(TARGET) $(INSTALL_DIR)
	@echo "Creating symlink 'rayhan' to $(INSTALL_DIR)/$(TARGET)..."
	ln -sf $(INSTALL_DIR)/$(TARGET) $(INSTALL_DIR)/rayhan
	@echo "Installation complete. You can now run 'rayhan' from anywhere."

uninstall:
	@echo "Uninstalling $(TARGET) from $(INSTALL_DIR)..."
	rm -f $(INSTALL_DIR)/$(TARGET)
	rm -f $(INSTALL_DIR)/rayhan
	@echo "Uninstallation complete."

