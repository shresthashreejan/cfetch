cfetch: src/cfetch.c
	$(CC) src/cfetch.c -lcurl -o build/cfetch -Wall -Wextra -pedantic -std=c99