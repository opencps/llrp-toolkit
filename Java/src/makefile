SHELL=/bin/bash

all: source llrp

.PHONY: clean source llrp

clean:
	rm -rf classes
	rm -rf codegen/*.class
	rm -rf edu/uark/csce/llrp/*.java

source:
	cd codegen; javac -source 5 -target 5 codegen.java; cd ..;
	cd edu/uark/csce/llrp; java -cp ../../../../codegen codegen ../../../../codegen/llrp.xml; cd ../../../..
	cp byhand/*.java edu/uark/csce/llrp

llrp:
	rm -rf classes
	mkdir classes
	javac -source 5 -target 5 -d classes edu/uark/csce/llrp/*.java
	cd classes; jar cvf ../../lib/llrp.jar edu/uark/csce/llrp; cd ..

docinfo:
	for file in `ls edu/uark/csce/llrp`; do echo XYXY$${file%.java}; grep "//\|public" edu/uark/csce/llrp/$$file | grep -v "member variables" | grep -v "serializeBody" | grep -v "deserializeBody" | grep -v "toXML"; done
