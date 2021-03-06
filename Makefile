# Makefile for grub2-se-verifyserv
# Author: Bandie Canis 
# Licence: GNU-GPLv3

all: 
	mkdir -p bin
	cc --std=c99 src/grub2-se-verifyserv.c -o bin/grub2-se-verifyserv

install:
	mkdir -p /usr/local/sbin
	cp ./bin/grub2-se-verifyserv /usr/local/sbin/
	cp ./systemd/grub2-se-verifyserv.service /etc/systemd/system
	chown root:root /usr/local/sbin/grub2-se-verifyserv
	chmod 740 /usr/local/sbin/grub2-se-verifyserv
	@printf "Done.\n"
	@printf "You may want to enable and start the service: \"systemctl enable grub2-se-verifyserv ; systemctl start grub2-se-verifyserv\"\n"	
	@printf "The file will be called /verified\n"

uninstall:
	rm /usr/sbin/local/grub2-se-verifyserv
	rm /etc/systemd/system/grub2-se-verifyserv.service
	@printf "Done.\n"

clean:
	rm bin/grub2-se-verifyserv
	rmdir bin

test:
	@printf "What to test?"
