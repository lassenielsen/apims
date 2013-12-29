#ifndef MPSPAR_HPP
#define MPSPAR_HPP
#include <apims/mpsterm.hpp>
namespace apims {
// DOCUMENTATION: MpsPar {{{
/*!
 * MpsPar is the parallel compisition of two processes.
 */
// }}}
class MpsPar : public MpsTerm // {{{
{
  public:
    MpsPar(const MpsTerm &left, const MpsTerm &right, const std::vector<std::string> &leftFinal, const std::vector<std::string> &rightFinal);
    virtual ~MpsPar();

    bool TypeCheck(const MpsExp &Theta,
                   const MpsMsgEnv &Gamma,
                   const MpsProcEnv &Omega);
    bool SubSteps(std::vector<MpsStep> &dest);
    MpsTerm *ApplyRcv(const std::string &path, const MpsExp *val) const;
    MpsTerm *ApplySnd(const std::string &path, MpsExp **val, MpsChannel &ch) const;
    MpsTerm *ApplyBRcv(const std::string &path, const std::string &label) const;
    MpsTerm *ApplyBSnd(const std::string &path, std::string &label, MpsChannel &ch) const;
    MpsTerm *ApplyLink(const std::vector<std::string> &paths, const std::string &session) const;
    MpsTerm *ApplySync(const std::vector<std::string> &paths, const std::string &label) const;
    MpsTerm *ApplyDef(const std::string &path, std::vector<MpsFunction> &dest) const;
    MpsTerm *ApplyCall(const std::string &path, const std::vector<MpsFunction> &funs) const;
    MpsTerm *ApplyOther(const std::string &path) const;
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

    const std::vector<std::string> &GetLeftFinal() const;
    const std::vector<std::string> &GetRightFinal() const;

  private:
    MpsTerm *myLeft;
    MpsTerm *myRight;
    std::vector<std::string> myLeftFinal;
    std::vector<std::string> myRightFinal;
}; // }}}
}
#endif