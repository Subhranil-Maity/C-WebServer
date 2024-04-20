OUT_FILE=server.out
CARGS=-Wall -Werror -Wextra -pedantic -g
.PHONY: clean $(OUT_FILE)
$(OUT_FILE): main.c
	gcc -o $(OUT_FILE) main.c web_server.c web_log.c $(CARGS)
clean:
	rm $(OUT_FILE)
