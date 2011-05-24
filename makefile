all:
	node-waf configure build

install:
	@mkdir -p ~/.node_libraries && cp ./build/default/liblzg.node ~/.node_libraries/

tests:
	@node ./liblzg_test.js

clean:
	@rm -rf ./build
