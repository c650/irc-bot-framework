#!/usr/bin/ruby

if ARGV.empty?
	puts "Usage: ./compile-plugin.rb [plugin_name] [--with-curl]"
	exit
end

plugin_name = ARGV[0]

with_curl = ARGV.length >= 2 && ARGV[1] == "--with-curl"

command = "g++ -Os --std=c++17 -fPIC -shared -rdynamic -o ./bin/#{plugin_name}.so ./src/Plugins/#{plugin_name}.cpp #{with_curl ? "./src/Plugins/http.cpp -lcurl" : ""}"

puts command
output = `#{command}`
puts output unless output.empty?
