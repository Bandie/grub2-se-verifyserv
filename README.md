# grub2-se-verifyserv


## Purpose
After a kernel upgrade you may forgot to sign your kernel using the [grub2-signing-extension scripts](https://github.com/Bandie/grub2-signing-extension) again. 
This is why I wanted an integration with i3status. I wrote a service in C which touches a file if GRUB2 is signed. If GRUB2 is not signed the file will be deleted.


## Installation
You need gcc or something similar. There is a systemd servicefile included. 
To compile, install and start it you may want to do the following within this project directory:

```
make
sudo make install
sudo systemctl enable grub2-se-verifyserv
sudo systemctl start grub2-se-verifyserv
```


## i3status integration
To let it integrate with i3status, add the following to your i3status config:

```
order += "path_exists GRUB2_signed"
[...]
[...]
path_exists GRUB2_signed {
  path = "/verified"
}
```

`grub2-se-verifyserv --help` will tell you the same btw.


