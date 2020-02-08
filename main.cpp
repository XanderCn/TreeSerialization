#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <limits>
#include <map>

#include "nodes.h"

using std::cout;
using std::endl;
using std::cerr;

//functions defined after int main()
void printUsage ();
int parseOpts ( int argc, char** argv, std::string& sInput, std::string& sOutput );

int main(int argc, char ** argv)
{
    std::string sInputFIle;
    std::string sOutputFile;

    if ( parseOpts(argc, argv, sInputFIle, sOutputFile) ) {
        cerr << "Failed to parse options" << endl;
        printUsage();
        return 1;
    }

    TiXmlDocument inputDoc(sInputFIle.c_str());
    inputDoc.LoadFile();

    if ( inputDoc.Error() ){
        cerr << "Failed to load document  \'"<< sInputFIle << "\' " << endl;
        inputDoc.ClearError();
        return 1;
    }

    baseNode* treeRoot = nodes::xmlToTree(inputDoc);

    if (!treeRoot) {
        cerr << "Failed to serialize tree" << endl;
        return 1;
    }

    cout << "Tree loaded from file \'" << sInputFIle << "\':" << endl;

    treeRoot->printTree();

    if ( !sOutputFile.empty()) { // if "-o" option specified
        try {
            TiXmlDocument outputDoc = nodes::treeToXML(treeRoot);
            outputDoc.SaveFile(sOutputFile.c_str());
            cout << "Tree saved to file \'" << sOutputFile<< "\'" << endl;
        } catch (...) {
            cerr << "Failed to serialize tree" << endl;
        }
    }

    delete treeRoot;

    return 0;
}

// simplest option parser for input/output file specification, return 0 on success, accepts only "-i"/"-o" options and no others.
// Need to <space> between "-i"/"-o" and <fileName>

void printUsage () {
    cout << "Usage:" << endl;
    cout << "\t 2GISTreeSerialization -i <inputFile> -o <outputFile>" << endl;
    cout << "\t 2GISTreeSerialization -i <inputFile>" << endl;
}

int parseOpts ( int argc, char** argv, std::string& sInput, std::string& sOutput ) {
    int retErr = 0;
    bool inputIsSet = false;
    bool outputIsdSet = false;

    for ( int i = 1; i < argc && !retErr; i++ ) {
        if (		argv[i][0] == '-'
                &&	std::strlen(argv[i]) == 2
                &&	i < argc - 1) {
                switch(argv[i][1]) {
                case 'i':
                    if ( inputIsSet )
                        return 1;

                    sInput.assign(argv[i++ + 1]);
                    inputIsSet = true;
                    break;
                case 'o':
                    if ( outputIsdSet )
                        return 1;
                    sOutput.assign(argv[i++ + 1]);
                    outputIsdSet = true;
                    break;
                default:
                    cout << "Unknown option specified" << endl;
                    return 1;
                }
        } else {
            cout << "Options syntax broken" << endl;
            retErr = 1;
            break;
        }
    }

    return retErr;
}

