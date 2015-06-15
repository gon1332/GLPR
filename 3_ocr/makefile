CC     = g++
CFLAGS = -Wall -Wextra
CLIBS  = -llept -ltesseract
EXE    = ocr.out

main: main.cpp
	$(CC) $(CFLAGS) $< -o $(EXE) $(CLIBS)

.PHONY:
clean:
	rm -rf *~ $(EXE)
