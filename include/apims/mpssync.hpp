#ifndef MPSSYNC_HPP
#define MPSSYNC_HPP
#include <apims/mpsterm.hpp>
namespace apims {
// DOCUMENTATION: MpsSync {{{
/*!
 * MpsSync performs a symmetric synchronization with the other participants in the session.
 */
// }}}
class MpsSync : public MpsTerm // {{{
{
  public:
    MpsSync(int maxpid, const std::string &session,  const std::map< std::string, MpsTerm*> &branches, const std::map<std::string,MpsExp*> &assertions);
    virtual ~MpsSync();

    bool TypeCheck(const MpsExp &Theta,
                   const MpsMsgEnv &Gamma,
                   const MpsProcEnv &Omega);
    bool SubSteps(std::vector<MpsStep> &dest);
    MpsTerm *ApplySync(const std::vector<std::string> &paths, const std::string &label) const;
    MpsTerm *ReIndex(const std::string &session,
                     int pid, int maxpid) const;
    MpsTerm *PRename(const std::string &src, const std::string &dst) const;
    MpsTerm *ERename(const std::string &src, const std::string &dst) const;
    MpsTerm *PSubst(const std::string &var,
                    const MpsTerm &exp,
                    const std::vector<std::string> &args,
                    const std::vector<std::pair<int,int> > &argpids,
                    const std::vector<std::string> &stateargs) const;
    MpsTerm *ESubst(const std::string &source, const MpsExp &dest) const;
    MpsTerm *GSubst(const std::string &source, const MpsGlobalType &dest, const std::vector<std::string> &args) const;
    MpsTerm *LSubst(const std::string &source, const MpsLocalType &dest, const std::vector<std::string> &args) const;
    std::set<std::string> FPV() const;
    std::set<std::string> FEV() const;
    MpsTerm *Copy() const;
    bool Terminated() const;
    MpsTerm *Simplify() const;
    std::string ToString(std::string indent="") const;
    std::string ToTex(int indent=0, int sw=2) const;
    MpsTerm *RenameAll() const;
    MpsTerm *CloseDefinitions() const;
    MpsTerm *ExtractDefinitions(MpsFunctionEnv &env) const;
    std::string ToC() const;
    std::string ToCHeader() const;

  private:
    std::string mySession;
    std::map<std::string, MpsTerm*> myBranches;
    int myMaxpid;
    std::map<std::string, MpsExp*> myAssertions;
}; // }}}
}
#endif