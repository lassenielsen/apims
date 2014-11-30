#include<apims/mpslink.hpp>
#include <apims/mpsend.hpp>
#include <apims/mpsgui.hpp>
#include <apims/common.hpp>

using namespace std;
using namespace apims;

MpsLink::MpsLink(const string &channel, const std::string &session, int pid, int maxpid, const MpsTerm &succ, bool pure) // {{{
: myChannel(channel),
  mySession(session),
  myMaxpid(maxpid),
  myPid(pid),
  myPure(pure)
{
  // Assert succ!=NULL
  mySucc = succ.Copy();
} // }}}
MpsLink::~MpsLink() // {{{
{
  // assert mySucc != NULL
  delete mySucc;
} // }}}
bool MpsLink::TypeCheck(const MpsExp &Theta, const MpsMsgEnv &Gamma, const MpsProcEnv &Omega, const set<pair<string,int> > &pureStack, const string &curPure) // * Use rules Mcast and Macc {{{
{
  // Check purity constraints
  if (pureStack.size()>0)
    return PrintTypeError("Implementation of pure participant " + int2string(pureStack.begin()->second) + "@" + pureStack.begin()->first + " must be immediately after its decleration",*this,Theta,Gamma,Omega);

  // Verify link
  MpsMsgEnv newGamma = Gamma;
  // Check linking on available channel
  MpsMsgEnv::iterator var=newGamma.find(myChannel);
  if (var==newGamma.end())
    return PrintTypeError((string)"Linking on unknown channel:" + myChannel,*this,Theta,Gamma,Omega);
  const MpsChannelMsgType *channel=dynamic_cast<const MpsChannelMsgType*>(var->second);
  if (channel==NULL)
    return PrintTypeError((string)"Linking on non-channel:" + myChannel,*this,Theta,Gamma,Omega);
  // Store purity of channel
  myPure=true;
  for (int i=0; myPure && i<channel->GetMaxPid(); ++i)
    if (i+1!=myPid && !channel->GetParticipants()[i].IsPure())
      myPure=false;
  // Check purity constraint
  if (curPure.size()>0 && !myPure)
    return PrintTypeError((string)"Unpure link in pure context is not allowed",*this,Theta,Gamma,Omega);

  // Check correct maxpid
  if (myMaxpid != channel->GetGlobalType()->GetMaxPid())
    return PrintTypeError((string)"MaxPID is different from:" + int2string(channel->GetGlobalType()->GetMaxPid()),*this,Theta,Gamma,Omega);

  // Check that only completed sessions are hidden
  var=newGamma.find(mySession);
  if (var!=newGamma.end())
  { const MpsDelegateMsgType *session=dynamic_cast<const MpsDelegateMsgType*>(var->second);
    if (session!=NULL &&
        !session->GetLocalType()->Equal(Theta,MpsLocalEndType()))
      return PrintTypeError((string)"Linking on open session:" + mySession,*this,Theta,Gamma,Omega);

    newGamma.erase(var);
  }
  // Create local type
  MpsLocalType *newType=channel->GetGlobalType()->Project(myPid);
  set<string> fv = newType->FEV();
  // Rename all free variables
  for (set<string>::const_iterator it=fv.begin(); it!=fv.end(); ++it)
  { MpsLocalType *tmpType=newType->ERename(*it,MpsExp::NewVar(*it));
    delete newType;
    newType=tmpType;
  }
  // Create Gamma with new session
  newGamma[mySession] = new MpsDelegateLocalMsgType(*newType,myPid,channel->GetParticipants());
  delete newType;

  bool result=mySucc->TypeCheck(Theta,newGamma,Omega,pureStack,curPure);

  // Clean up
  delete newGamma[mySession];

  return result;
} // }}}
MpsTerm *MpsLink::ApplyLink(const std::vector<std::string> &paths, const std::string &session) const // {{{
{ if (paths.size()==0)
    return Copy();
  else if (paths.size()>1)
    return Error((string)"Applying Link on "+ToString());

  if (myPid==1) // Only first participant registers session
    mpsgui::CreateSession(session,myMaxpid); // Inform GUI
  // Renamimg session channel to common name
  MpsTerm *tmpResult = mySucc->ERename(mySession,session);
  // Reindexing session
  // session[i] << exp becomes session[myMaxpid*i+myPid] << exp, and
  // session[i] >> var becomes session[myMaxpid*myPid+i] >> var.
  MpsTerm *result = tmpResult->ReIndex(session,myPid,myMaxpid);
  delete tmpResult;
  return result;
} // }}}
bool MpsLink::SubSteps(vector<MpsStep> &dest) // {{{
{
  MpsEvent event;
  event.myType    = link;
  event.myChannel = myChannel;
  event.myMaxpid  = myMaxpid;
  event.myPid     = myPid;
  vector<string> paths;
  paths.push_back("");
  dest.push_back(MpsStep(event,paths));
  return false;
} // }}}
MpsTerm *MpsLink::PRename(const string &src, const string &dst) const // {{{
{
  // assert mySucc != NULL
  MpsTerm *newSucc = mySucc->PRename(src,dst);
  MpsTerm *result = new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::ERename(const string &src, const string &dst) const // {{{
{
  // assert mySucc != NULL
  string newChannel = myChannel==src?dst:myChannel;
  if (mySession == src) // No further substitution needed
    return new MpsLink(newChannel, mySession, myPid, myMaxpid, *mySucc, myPure);

  string newSession = mySession;
  MpsTerm *newSucc = NULL;
  if (mySession==dst)
  {
    newSession = MpsExp::NewVar(mySession);
    MpsTerm *tmpSucc = mySucc->ERename(mySession, newSession);
    newSucc = tmpSucc->ERename(src,dst);
    delete tmpSucc;
  }
  else
    newSucc = mySucc->ERename(src,dst);

  MpsTerm *result = new MpsLink(newChannel, newSession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::ReIndex(const string &session, int pid, int maxpid) const // {{{
{
  // assert mySucc != NULL
  if (mySession == session) // No further substitution needed
    return new MpsLink(myChannel, mySession, myPid, myMaxpid, *mySucc, myPure);

  MpsTerm *newSucc =  mySucc->ReIndex(session,pid,maxpid);

  MpsTerm *result = new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::PSubst(const string &var, const MpsTerm &exp, const vector<string> &args, const vector<pair<int,int> > &argpids, const vector<string> &stateargs) const // {{{
{
  // assert mySucc != NULL
  MpsTerm *newSucc = mySucc->PSubst(var,exp,args,argpids,stateargs);
  MpsTerm *result = new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::ESubst(const string &source, const MpsExp &dest) const // {{{
{
  // assert mySucc != NULL
  string newChannel = stringESubst(myChannel,source,dest);
  if (mySession == source) // No further substitution needed
    return new MpsLink(newChannel, mySession, myPid, myMaxpid, *mySucc, myPure);

  set<string> fv=dest.FV();
  string newSession = mySession;
  MpsTerm *newSucc = NULL;
  if (fv.find(mySession) != fv.end())
  {
    newSession = MpsExp::NewVar();
    MpsExp *newVar = new MpsVarExp(newSession,MpsMsgNoType());
    MpsTerm *tmpSucc = mySucc->ESubst(mySession, *newVar);
    newSucc = tmpSucc->ESubst(source,dest);
    delete newVar;
    delete tmpSucc;
  }
  else
    newSucc = mySucc->ESubst(source,dest);

  MpsTerm *result = new MpsLink(newChannel, newSession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::GSubst(const string &source, const MpsGlobalType &dest, const vector<string> &args) const // {{{
{
  MpsTerm *newSucc = mySucc->GSubst(source,dest,args);
  MpsTerm *result = new MpsLink(myChannel,mySession,myPid,myMaxpid,*newSucc, myPure);

  // Clean Up
  delete newSucc;

  return result;
} // }}}
MpsTerm *MpsLink::LSubst(const string &source, const MpsLocalType &dest, const vector<string> &args) const // {{{
{
  MpsTerm *newSucc = mySucc->LSubst(source,dest,args);
  MpsTerm *result = new MpsLink(myChannel,mySession,myPid,myMaxpid,*newSucc, myPure);

  // Clean Up
  delete newSucc;

  return result;
} // }}}
set<string> MpsLink::FPV() const // {{{
{
  set<string> result = mySucc->FPV();
  return result;
} // }}}
set<string> MpsLink::FEV() const // {{{
{
  set<string> result = mySucc->FEV();
  result.erase(mySession);
  result.insert(myChannel);
  return result;
} // }}}
MpsTerm *MpsLink::Copy() const // {{{
{
return new MpsLink(myChannel, mySession, myPid, myMaxpid, *mySucc, myPure);
} // }}}
bool MpsLink::Terminated() const // {{{
{
  return false;
} // }}}
MpsTerm *MpsLink::Simplify() const // {{{
{
  // assert mySucc != NULL
  MpsTerm *newSucc = mySucc->Simplify();
  MpsTerm *result = new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
string MpsLink::ToString(string indent) const // {{{
{
  return mySession + "= new " + myChannel + "(" + int2string(myPid) + " of " + int2string(myMaxpid) + ");\n"
       + indent + mySucc->ToString(indent);
} // }}}
string MpsLink::ToTex(int indent, int sw) const // {{{
{
  return ToTex_KW("link") + "(" + ToTex_PP(myMaxpid) + "," + ToTex_ChName(myChannel) + "," + ToTex_Session(mySession) + "," + ToTex_PP(myPid) + ");\\newline\n"
       + ToTex_Hspace(indent,sw) + mySucc->ToTex(indent,sw);
} // }}}
string MpsLink::ToC() const // {{{
{
  stringstream result;
  result << "  DecAprocs();" << endl
         << "  Session*" << ToC_Name(mySession) << "=new Session_FIFO(" << ToC_Name(myChannel) << ".GetValues(), " << int2string(myPid-1) << ", " << int2string(myMaxpid) << ");" << endl
         << "  IncAprocs();" << endl;
  result << mySucc->ToC();
  return result.str();
} // }}}
string MpsLink::ToCHeader() const // {{{
{
  return mySucc->ToCHeader();
} // }}}
MpsTerm *MpsLink::RenameAll() const // {{{
{ string newSession=MpsExp::NewVar(mySession);
  MpsTerm *tmpSucc=mySucc->ERename(mySession,newSession);
  MpsTerm *newSucc=tmpSucc->RenameAll();
  delete tmpSucc;
  MpsTerm *result=new MpsLink(myChannel,newSession,myPid,myMaxpid,*newSucc, myPure);
  
  delete newSucc;

  return result;
} // }}}
bool MpsLink::Parallelize(const MpsTerm &receives, MpsTerm* &seqTerm, MpsTerm* &parTerm) const // {{{
{ MpsTerm *pre;
  MpsTerm *post;
  if (myPure) // Only optimize if pure
  { set<string> fv;
    fv.insert(myChannel);
    fv.insert(mySession);
    receives.Split(fv,pre,post);
  }
  else
  { pre=receives.Copy();
    post=new MpsEnd();
  }
  bool opt1 = dynamic_cast<const MpsEnd*>(post)==NULL;
  // Parallelize succ
  MpsTerm *seqSucc;
  MpsTerm *parSucc;
  bool opt2 = mySucc->Parallelize(*post,seqSucc,parSucc);
  delete post;
  // Create seqTerm as link;seqSucc
  seqTerm=new MpsLink(myChannel, mySession, myPid, myMaxpid, *seqSucc, myPure);
  delete seqSucc;
  // Create parTerm as pre;link;parSucc
  MpsTerm *parTmp=new MpsLink(myChannel, mySession, myPid, myMaxpid, *parSucc, myPure);
  delete parSucc;
  parTerm=pre->Append(*parTmp);
  delete parTmp;
  delete pre;
  return opt1 || opt2;
} // }}}
MpsTerm *MpsLink::Append(const MpsTerm &term) const // {{{
{ MpsTerm *newSucc=mySucc->Append(term);
  MpsTerm *result=new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::CloseDefinitions() const // {{{
{
  MpsTerm *newSucc = mySucc->CloseDefinitions();
  MpsTerm *result= new MpsLink(myChannel, mySession, myPid, myMaxpid, *newSucc, myPure);
  delete newSucc;
  return result;
} // }}}
MpsTerm *MpsLink::ExtractDefinitions(MpsFunctionEnv &env) const // {{{
{ MpsTerm *newSucc=mySucc->ExtractDefinitions(env);
  MpsTerm *result=new MpsLink(myChannel,mySession,myPid,myMaxpid,*newSucc, myPure);
  
  delete newSucc;

  return result;
} // }}}
