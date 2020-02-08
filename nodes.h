#ifndef NODES_H
#define NODES_H

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>
#include <limits>
#include <memory>

#include "TinyXML/tinyxml.h"
#include "TinyXML/tinystr.h"

/*  File nodes.h defines node classes and factories for nodes, and functions to serialize/deserialize tree.
    Methods/functions definitions are placed in header file for simplicity
    (as long as all tree-related code fits well into one file).
*/

using std::cout;
using std::cin;
using std::endl;

//MACRO defined just for tree printing

#define PRINT_TABS(x) for ( int i = 0; i < x; i++ ) cout << "\t";

enum nodeType {
    STRING_VAL,
    INT_NUM,
    REAL_NUM};

class baseNode {

public:

    baseNode(nodeType t) : type(t){};

    void pushChild ( baseNode * child ) {
        children.push_back ( child );
    }

    virtual std::string getStrData () = 0; //serializable interface
    virtual std::string getStrType () = 0; //for pretty printing

    void printTree (int tabOffset = 0) {
        PRINT_TABS(tabOffset);
        cout << "--{ " << this->getStrType() << "("<< this->getStrData()<< ")" << endl;

        for ( auto it = this->children.begin(); it != this->children.end(); it++ ) {
            (*it)->printTree(tabOffset+1);
        }
    }

    nodeType getType(){
        return type;
    }

    const std::list<baseNode*>& getChildren(){
        return children;
    }

    virtual ~baseNode(){
        for ( auto it = this->children.begin(); it != this->children.end(); it++ ) {
            delete (*it);
        }
    }

private:
    nodeType type;
    std::list<baseNode *> children;
};

class stringNode: public baseNode {
public:
    stringNode(std::string str): baseNode(STRING_VAL), val(str) {};

    std::string getStrData() {
        return this->val;
    }
    std::string getStrType (){
        return "STRING";
    }
    std::string getValue () {
        return this->val;
    }
private:
    std::string val;
};

class intNode: public baseNode {
public:
    intNode(int num): baseNode(INT_NUM), val(num) {};

    std::string getStrData() {
        std::string retStr = std::to_string(this->val);
        return retStr;
    }
    std::string getStrType (){
        return "INT";
    }
    int getValue () {
        return this->val;
    }
private:
    int val;
};

class realNode: public baseNode {
public:
    realNode(double num): baseNode(REAL_NUM), val(num) {};

    std::string getStrData() {
        std::stringstream stream;
        stream << this->val;
        return stream.str();
    }

    std::string getStrType (){
        return "REAL";
    }

    double getValue () {
        return this->val;
    }
private:
    double val;
};

// abstract factory class to spawn tree nodes according to its type stored in file
class nodeFactory {
    public:
        nodeFactory(){};
        virtual baseNode* createNode( const char* strValue ) = 0;
};

class nodeStrFactory : public nodeFactory { //to spawn string node
    public:
        nodeStrFactory() {};
        baseNode* createNode (const char* strValue) {
            return new stringNode(strValue);
        }
};

class nodeIntFactory : public nodeFactory { //to spawn int node
    public:
        nodeIntFactory() {};
        baseNode* createNode (const char* strValue) {
            return new intNode(std::stoi( strValue ));
        }
};

class nodeRealFactory : public nodeFactory { //to spawn double node
    public:
        nodeRealFactory() {};
        baseNode* createNode (const char* strValue) {
            return new realNode(std::stod(strValue));
        }
};

namespace nodes { //free functions grouped in "nodes" namespace for convenience

    nodeType getType(const TiXmlElement* elem){
        return static_cast<nodeType>(std::stoi(elem->Attribute("type")));
    }

    void deSerializeSubTree (const TiXmlElement* current, baseNode* parent, std::map<nodeType, std::unique_ptr<nodeFactory>>& factories) {
        for( const TiXmlElement * child = current->FirstChildElement(); child; child = child->NextSiblingElement() ) {
            baseNode *childNode = factories [ getType(child) ] -> createNode(child->Attribute("val"));
            parent->pushChild(childNode);

            deSerializeSubTree (child, childNode, factories);
        }
    }

    baseNode* xmlToTree(const TiXmlDocument& doc) {

        baseNode* rootElement = NULL;

        try {
            std::map<nodeType, std::unique_ptr<nodeFactory>> factories;
            factories[STRING_VAL] = std::unique_ptr<nodeFactory>(new nodeStrFactory());
            factories[INT_NUM] = std::unique_ptr<nodeFactory>(new nodeIntFactory());
            factories[REAL_NUM] = std::unique_ptr<nodeFactory>(new nodeRealFactory());

            const TiXmlElement * cursor = doc.FirstChildElement("node");
            if ( !cursor ) return NULL;

            rootElement = factories [ getType(cursor) ] -> createNode(cursor->Attribute("val"));
            deSerializeSubTree (cursor, rootElement, factories);
        } catch (...) {
            return NULL;
        }

        return rootElement;
    }

    void serializeSubTree(TiXmlElement* current, baseNode* parent ) {
        for( auto child = parent->getChildren().begin(); child != parent->getChildren().end() ; child++ ) {

            TiXmlElement * currentElem = new TiXmlElement( "node" );
            currentElem->SetAttribute("type", (*child)->getType());
            currentElem->SetAttribute("val", (*child)->getStrData().c_str());
            current->LinkEndChild(currentElem);

            serializeSubTree(currentElem,*child);
        }
    }

    TiXmlDocument treeToXML( baseNode* treeRoot) {
        TiXmlDocument doc;

        if (!treeRoot) return doc;

        TiXmlElement * rootElement = new TiXmlElement( "node" );
        rootElement->SetAttribute("type", treeRoot->getType());
        rootElement->SetAttribute("val", treeRoot->getStrData().c_str());
        doc.LinkEndChild( rootElement );

        serializeSubTree(rootElement, treeRoot);

        return doc;
    }

} // END nodes namespace

#endif // NODES_H
