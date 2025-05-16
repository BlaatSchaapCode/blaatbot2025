MAKE ?= make

all:
	$(MAKE) -j -C pluginloader
	$(MAKE) -j -C core
	$(MAKE) -j -C client/bot
	$(MAKE) -j -C botmodules/test
	$(MAKE) -j -C botmodules/cbotmod
	$(MAKE) -j -C connection/tcp
	$(MAKE) -j -C protocol/irc
	$(MAKE) -j -C connection/libretls

format:
	find ../src/ -iname '*.hpp' -o -iname '*.cpp' -o -iname '*.h' -o -iname '*.c' | xargs clang-format -i
