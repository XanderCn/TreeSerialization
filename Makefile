all:
	g++ -Wall -g main.cpp ./TinyXML/tinystr.cpp ./TinyXML/tinyxml.cpp ./TinyXML/tinyxmlerror.cpp ./TinyXML/tinyxmlparser.cpp -o 2GISTreeSerialization -DTIXML_USE_STL
clean:
	rm -f ./2GISTreeSerialization