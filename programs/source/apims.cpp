#include <apims/mpsterm.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;
using namespace apims;
int main(int argc, char **argv)
{
  // Config Parameters
  bool cfgSteps=false;
  bool cfgBuffers=false;
  bool cfgChoices=false;
  bool cfgTypecheck=true;
  string term="";

  bool argFault=true;
  for (int i=1; i<argc; ++i) // {{{
  { if ((string)argv[i]=="-steps")
      cfgSteps=true;
    else if ((string)argv[i]=="-buffers")
      cfgBuffers=true;
    else if ((string)argv[i]=="-choices")
      cfgChoices=true;
    else if ((string)argv[i]=="-nocheck")
      cfgTypecheck=false;
    else if ((string)argv[i]=="-f" && i+1<argc)
    { ++i;
      // Read program from file
      ifstream fin(argv[i]);
      if (fin)
      { argFault=false;
        fin.seekg (0, ios::end);
        int size = fin.tellg();
        char *memblock = new char [size+1];
        fin.seekg (0, ios::beg);
        fin.read (memblock, size);
        fin.close();
        memblock[size]=0;
        term=memblock;
        delete[] memblock;
      }
    }
    else if (argv[i][0]!='-')
    { argFault=false;
      term=argv[i];
    }
    else
    { argFault=true;
      i=argc;
    }
  } // }}}
  if (argFault) // {{{
  {
    cerr << "Syntax: apims '<term>'" << endl;
    cerr << "    or: apims [-steps] [-buffers] [-choices] [-nocheck] (-f <file.mps>|<program>)" << endl;
    return -1;
  } // }}}
  // Parse program
  MpsTerm *next = MpsTerm::Create(term);
  cout << "Program: " << endl << next->ToString() << endl;
  MpsTerm *current = NULL;
  if (cfgTypecheck)
  { // Typecheck program
    cout << "************ Type Checking Program ************" << endl;
    if (not next->TypeCheck())
      return 1;
    cout << "************ Type Check Succeeded! ************" << endl;
  }
  else
    cout << "************** NO TYPE CHECKEING **************" << endl;
  // Apply semantics repeatedly (evaluate)
  MpsEnv env;
  vector<MpsFunction> defs;
  srand((unsigned)time(0));
  while (next != NULL) // While it can step, choose random step
  {
    if (current != NULL) // Clean up old state
      delete current;
    // Move to next state
    current = next->Simplify(); // Goto next state (and remove dead processes)
    delete next;
    next = NULL;
    // Print state
    if (cfgSteps)
      cout << "******************* Step: *******************\n"
           << Env2string(env) << "\n"
           << DefEnv2string(defs) << " in\n"
           << current->ToString() << endl;
    else if (cfgBuffers)
      cout << "***************** Buffers: ******************\n"
           << Env2string(env) << endl;
    // Find next state
    int choices = -1;
    int choice = -1;
    next = current->Step(env,defs,choice,choices);
    if (cfgChoices && choices>0)
      cout << "*********** Selected Step: " << choice+1 << " of " << choices << " ***********" << endl;
  }
  cout << "******************* Done! *******************" << endl;
  delete current;
}
