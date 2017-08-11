# An IRC Bot Framework

This framework is written entirely in C++. I wrote it because I felt like it.
Anyone is free to customize it, add to it, or even contribute in making a better, cleaner (please) framework.

---
# Usage

The code in this repository compiles via `make` into a shared object to be linked into some executable.

---
# Compiling

First, run `./configure.sh`. If you aren't using a Debian-based linux, the dependencies are **libcurl** and **openSSL/libssl** and you must install them manually. If you intend to compile from source, the `*-dev` packages, as well as a C++ compiler, are also needed.

As of commit e6c30387021bfbdbc36e459c37b276e7da32657e, `make` compiles the bot source code into a shared library. An example is in https://github.com/c650/irc-bot-plugins.

---

I have added a few commands as an example, but they needn't stay if you don't want them to.

---
# License

This project is licensed under the [MIT License](/LICENSE).
