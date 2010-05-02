#pragma once

#include <Constants.hpp>
#include <LogFrame.hpp>
#include <MotionCmd.hpp>

#include <stdint.h>
#include <string>
#include <list>
#include <vector>

namespace Gameplay
{
	class Opponent;
	class GameplayModule;
	class Behavior;

	// This is largely a wrapper around Packet::LogFrame::Robot.
	// It provides convenience functions for setting motion commands and reading state.
	// It also tracks per-robot information that is internal to gameplay which does not need to be logged.
	class Robot
	{
		public:
			Robot(GameplayModule *gameplay, int id, bool self);

			Packet::LogFrame::Robot *packet() const;

			// Status indicators
			bool charged() const; /// true if the kicker is ready
			bool self() const;    /// true if this is one of our robots
			bool visible() const; /// true if robot is valid - FIXME: needs better check
			int id() const;       /// shell number of robot
			const Geometry2d::Point &pos() const;  /// Position
			const Geometry2d::Point &vel() const;  /// Velocity (vector)
			const float &angle() const;	  /// global orientation of the robot (radians)
			bool haveBall() const; /// true if we have the ball

			// Commands
			void setVScale(float scale = 1.0); /// scales the velocity
			void resetMotionCommand();  /// resets all motion commands for the robot

			// Move to a particular point using the RRT planner
			void move(Geometry2d::Point pt, bool stopAtEnd=true);

			/**
			 * Move along a path for waypoint-based control
			 * If not set to stop at end, the planner will send the robot
			 * traveling in whatever direction it was moving in at the end of the path.
			 * This should only be used when there will be another command when
			 * the robot reaches the end of the path.
			 */
			void move(const std::vector<Geometry2d::Point>& path, bool stopAtEnd=true);

			/**
			 * Move via a bezier curve, designed to allow for faster movement
			 * The points specified are bezier control points, which define the
			 * path taken.  Note: longer paths are more computationally expensive.
			 *
			 * To enable control point modification to allow for avoidance of obstacles,
			 * set the enableAvoid flag to true, false otherwise.  The stop at end
			 * flag works like in other move commands
			 */
			void bezierMove(const std::vector<Geometry2d::Point>& controls,
					Packet::MotionCmd::OrientationType facing,
					Packet::MotionCmd::PathEndType endpoint=Packet::MotionCmd::StopAtEnd);

			/** Move using direct velocity control by specifying
			 *  translational and angular velocity
			 */
			void move(const Geometry2d::Point& trans, double ang);

			/**
			 * Move using timed-positions, so that each node has a target time
			 * Also, the command needs a start time, so that it can calculate deltas
			 * in seconds
			 */
			void move(const std::vector<Packet::MotionCmd::PathNode>& timedPath, uint64_t start);

			/**
			 * Makes the robot spin in a specified direction
			 */
			void spin(Packet::MotionCmd::SpinType dir);

			/*
			 * Enable dribbler (note: can go both ways)
			 */
			void dribble(int8_t speed);

			/**
			 * Pivots around a given point in a particular direction
			 * Specify direction manually, or with bool
			 */
			void pivot(Geometry2d::Point ctr, Packet::MotionCmd::PivotType dir);
			void pivot(Geometry2d::Point center, bool cw);

			/**
			 * Face a point while remaining in place
			 */
			void face(Geometry2d::Point pt, bool continuous = false);

			/**
			 * Remove the facing command
			 */
			void faceNone();

			/**
			 * enable kick when ready at a given strength
			 */
			void kick(uint8_t strength);


			ObstacleGroup &obstacles() const;

			// True if this robot intends to kick the ball.
			// This is reset when this robot's role changes.
			// This allows the robot to get close to the ball during a restart.
			bool willKick;

			// True if this robot should avoid the ball by 500mm.
			// Used during our restart for robots that aren't going to kick
			// (not strictly necessary).
			bool avoidBall;

			// True if this robot intends to get close to an opponent
			// (e.g. for stealing).
			// This reduces the size of the opponent's obstacle.
			// These are reset when this robot's role changes.
			bool approachOpponent[Constants::Robots_Per_Team];

			// External access functions for utility reasons

			/** adds the pose to the history in the state variable */
			void updatePoseHistory();

		protected:
			GameplayModule *_gameplay;

			int _id;
			bool _self;
			Packet::LogFrame::Robot *_packet;
			std::vector<Packet::LogFrame::Robot::Pose> _poseHistory;
	};
}
