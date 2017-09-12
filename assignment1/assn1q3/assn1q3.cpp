#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "FormulaBuilder.h"
#include "Sequent.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cout << "Please input the sequent!" << endl;
        return -1;
    }
    else
    {
        stringstream ss;
        for (int i = 1; i < argc; i++)
        {
            ss << argv[i];
            ss << " ";
        }

        string input = ss.str();
        //cout << input << endl;

        vector<SyntaxItem> sv;
        Formula fm;
        FormulaBuilder::generateSyntaxVector(sv, input);

        //cout << input << endl;

        FormulaBuilder::buildFormula(sv, fm);

        //cout << fm << endl;

        Sequent seq{ fm };
        //cout << seq << endl;

        //cout << "\n";
        //cout << "\n";

        Sequent::init();
        Sequent::executeProof(seq);

        //cout << "\n";
        //cout << "\n";
    }

    return 1;
}