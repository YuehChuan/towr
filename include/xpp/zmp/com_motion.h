/**
 @file    com_motion.h
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Aug 23, 2016
 @brief   Declares the ComMotion class.
 */

#ifndef USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ZMP_COM_MOTION_H_
#define USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ZMP_COM_MOTION_H_

#include <xpp/utils/geometric_structs.h>
#include <memory>

namespace xpp {
namespace zmp {


enum PhaseType {kStancePhase=0, kStepPhase, kFlightPhase};
struct PhaseInfo {
  /** Information to represent different types of motion.
    *
    * @param type whether this is a stance, step of flight phase.
    * @param id for a step phase this specifies the current step taken ( first
    *           step starting at 0. For a stance phase this specifies the
    *           last step that was completed before the stance phase. The first
    *           stance has an id of -1 (no step taken before).
    */
  PhaseInfo(PhaseType type, int id) : type_(type), id_(id) {};
  PhaseType type_;
  int id_;
};

inline bool operator==(const PhaseInfo& lhs, const PhaseInfo& rhs)
{
  return (lhs.type_ == rhs.type_ && lhs.id_ == rhs.id_);
}

inline bool operator!=(const PhaseInfo& lhs, const PhaseInfo& rhs)
{
  return !(lhs == rhs);
}


/** Abstracts the Center of Mass (CoM) motion of any system.
  *
  * This class is responsible for providing a common interface to represent
  * the motion of a system. Specific implementation can for example use
  * splines or solutions of the Equation of Motion as representation.
  */
class ComMotion {
public:
  typedef Eigen::VectorXd VectorXd;
  typedef xpp::utils::Point2d Point2d;
  typedef std::shared_ptr<ComMotion> Ptr;
  typedef std::vector<PhaseInfo> PhaseInfoVec;

  ComMotion ();
  virtual ~ComMotion ();

  /** Get the Center of Mass position, velocity and acceleration.
    *
    * @param t_global current time
    * @return pos/vel/acc in 2D.
    */
  virtual Point2d GetCom(double t_global) const = 0;

  /** Set all coefficients to fully describe the CoM motion.
    *
    * These can be spline coefficients or coefficients from any type of equation
    * that produce x(t) = ...
    */
  virtual void SetCoefficients(const VectorXd& optimized_coeff) = 0;
  virtual int GetTotalFreeCoeff() const = 0;
  virtual VectorXd GetCoeffients() const = 0;

  /** If the trajectory has to be discretized, use this for consistent time steps.
   *  t(0)------t(1)------t(2)------...------t(N-1)---|------t(N)
   *
   *  so first and last time are t0 and and tN, but there might be a
   *  timestep > delta t before the last node.
   */
  std::vector<double> GetDiscretizedGlobalTimes() const;
  virtual double GetTotalTime() const = 0;
  int GetTotalNodes() const;

  /** Gets the continuously increasing phase (stance, swing) count
    *
    * This allows to pair the current instance with the correct footholds
    * and support polygon. A phase is a motion during which the dynamics are
    * continuous (stance, swing, flight).
    */
  virtual PhaseInfo GetCurrentPhase(double t_global) const = 0;

  /** Returns a vector of phases, where no phase is duplicated.
    */
  virtual PhaseInfoVec GetPhases() const = 0;

  /** Sets coefficients, so motion ends up at initial position.
    *
    * So at the end of the total time T, the state of the system will
    * be the start position with zero velocity.
    */
  virtual void SetEndAtStart() = 0;
};

} /* namespace zmp */
} /* namespace xpp */

#endif /* USER_TASK_DEPENDS_XPP_OPT_INCLUDE_XPP_ZMP_COM_MOTION_H_ */
