
# Nome dell'eseguibile
TARGET = server_http

# Compilatore
CC = gcc

# Flags di compilazione
CFLAGS = -Wall -pthread

# File sorgente
SRC = server_http.c

# Regola principale
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Regola per pulire i file compilati
clean:
	rm -f $(TARGET)
