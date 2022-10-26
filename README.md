# ft_irc
The goal of this project is to recreate an IRC server in C++, following the Internet RFC standards.

## Client
The reference client that was used for this project is **irssi**.

Install it with
```bash
sudo apt install irssi
```

Or with brew
```bash
brew install irssi
```

Then you can connect to the server using:
```bash
irssi -c <ip_adress> -p <port> -w <password> -n <nickname>
```

Try it with
```bash
irssi -c localhost -p 6667 -w pass -n meu
```

## Server
![Compile with macos](https://badgen.net/badge/build/macOS/grey?icon=apple)

Clone the project
```bash
git clone https://github.com/thomasbrq/ft_irc.git
```

Go to project directory
```bash
cd ft_irc
```

Build the executable.
```bash
make
```

Then launch the program
```bash
./ircserv <port> <password>
```

Try it with
```bash
./ircserv 6667 pass
```

## Authors

- [@thomasbrq](https://www.github.com/thomasbrq)
- [@erndael](https://www.github.com/SanGoruden)
