# An IRC Bot Framework

This framework is written entirely in C++. I wrote it because I felt like it.
Anyone is free to customize it, add to it, or even contribute in making a better, cleaner (please) framework.

All of this code is licensed under the MIT License.

---
# Design...

I aim to provide a simple way to interface with other entities on IRC. Using the `on_privmsg` function, a user of this framework can add some extra functionality. The bot will listen for a trigger like ("\@something"). The trigger has to be the beginning of another user's IRC message.

	b.on_privmsg("\@sayhi", [](const IRC::Packet& packet){
		packet.reply("Hello!");
	}, "says hello");

In this code above, "\@sayhi" is the trigger.

The lambda function is what gets run upon a triggering. The lambda function __must__ take a single parameter, a `const IRC::Packet&` type. Using this `Packet&`, a user can easily `reply()` to the message (a PRIVMSG is sent to the same channel/person from which the original message came).

The third parameter to `on_privmsg` is a description of the command. There is a fourth, optional parameter that specifies whether or not the command needs administrative privileges to run. This defaults to `false`.

---

I have added a few commands as an example, but they needn't stay if you don't want them to.

---
# Acknowledgements

Thanks to @nlohmann for his [C++ JSON Library](https://github.com/nlohmann/json).
