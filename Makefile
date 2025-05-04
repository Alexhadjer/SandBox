FireJail:
		gcc -Iinclude src/main.c src/util.c src/sandbox.c src/filesystem.c	-o ./bin/Firejail
		chmod +x ./bin/Firejail
		sudo ./bin/Firejail /bin/bash
