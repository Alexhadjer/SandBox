FireJail:
		gcc -Wall -Wextra -O2 -Iinclude src/main.c src/util.c src/sandbox.c src/filesystem.c src/sigsys.c src/log.c -lseccomp -o bin/Firejail
		chmod +x ./bin/Firejail
		sudo ./bin/Firejail bin/test.sh
