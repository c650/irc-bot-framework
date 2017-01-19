#! /usr/bin/ruby

# g++ --std=c++17 -c -fPIC -Os -o ./bin/server.o ./src/IRCBot/server.cpp
# g++ -rdynamic -shared -o file.so files.o ...

if ARGV.empty?
	puts "No args supplied"
end

object_files = ["./bin/bot.o ./bin/server.o ./bin/packet.o"]

ARGV.each do |arg|

	name = arg.gsub(".cpp","").split("/").last

	`g++ --std=c++17 -c -fPIC -Os -o ./bin/#{name}.o #{arg}`
	puts "g++ --std=c++17 -c -fPIC -Os -o ./bin/#{name}.o #{arg}"

	object_files << "./bin/#{name}.o"
end

`g++ -rdynamic -shared -o ./bin/#{ARGV.first.gsub(".cpp","").split("/").last}.so #{object_files.join(" ")}`
puts "g++ -rdynamic -shared -o ./bin/#{ARGV.first.gsub(".cpp","").split("/").last}.so #{object_files.join(" ")}"
