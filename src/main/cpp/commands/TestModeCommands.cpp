// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "commands/TestModeCommands.h"

#include <cmath>

ZeroCommand::ZeroCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Zero"); }

XsAndOsCommand::XsAndOsCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Xs and Os"); }

SquareCommand::SquareCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Square"); }

SpirographCommand::SpirographCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Spirograph"); }

OrbitCommand::OrbitCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Orbit"); }

PirouetteCommand::PirouetteCommand(DriveSubsystem *subsystem) noexcept
    : m_subsystem{subsystem} { SetName("Pirouette"); }

void ZeroCommand::Execute() noexcept { (void)m_subsystem->ZeroModules(); }

void XsAndOsCommand::Initialize() noexcept
{
    m_iteration = 0;

    m_subsystem->SetDriveBrakeMode(true);
}

void XsAndOsCommand::Execute() noexcept
{
    // This is expected to run at ~20Hz.  So 100 iterations is ~5s.
    if (m_iteration < 100)
    {
        (void)m_subsystem->SetLockWheelsX();
    }
    else
    {
        (void)m_subsystem->SetTurnInPlace();
    }

    if (++m_iteration >= 200)
    {
        m_iteration = 0;
    }
}

void XsAndOsCommand::End(bool interrupted) noexcept
{
    m_subsystem->SetDriveBrakeMode(false);
}

void SquareCommand::Initialize() noexcept
{
    m_side = 0;

    m_subsystem->SetDriveBrakeMode(true);

    m_subsystem->ResetDrive();
}

void SquareCommand::Execute() noexcept
{
    // First, command turning to the appropriate angle and go no further, until
    // the wheels are oriented appropriately.
    if (!m_subsystem->SetTurningPosition(90_deg * m_side))
    {
        return;
    }

    // Now, command driving a fixed distance and go no further, until it is
    // reached.
    if (!m_subsystem->SetDriveDistance(0.5_m))
    {
        return;
    }

    // Finished driving a side; now, set things up to start the next side, on
    // the next iteration.  After the fourth side, also reset the side counter.
    m_subsystem->ResetDrive();

    if (++m_side >= 4)
    {
        m_side = 0;
    }
}

void SquareCommand::End(bool interrupted) noexcept
{
    m_subsystem->SetDriveBrakeMode(false);

    m_subsystem->ResetDrive();
}

void SpirographCommand::Initialize() noexcept
{
    m_angle = 0;

    m_subsystem->SetDriveBrakeMode(true);

    m_subsystem->ResetDrive();
}

void SpirographCommand::Execute() noexcept
{
    // First, command turning to the appropriate angle and go no further, until
    // the wheels are oriented appropriately.
    if (!m_subsystem->SetTurningPosition(1_deg * m_angle))
    {
        return;
    }

    // Now, command driving a fixed distance and go no further, until it is
    // reached.
    if (!m_subsystem->SetDriveDistance(0.5_m))
    {
        return;
    }

    // Finished driving a side; now, set things up to start the next side, on
    // the next iteration.  After the fourth side, also reset the side counter.
    m_subsystem->ResetDrive();

    m_angle += 75;
    if (m_angle >= 360)
    {
        m_angle -= 360;
    }
}

void SpirographCommand::End(bool interrupted) noexcept
{
    m_subsystem->SetDriveBrakeMode(false);

    m_subsystem->ResetDrive();
}

void OrbitCommand::Execute() noexcept
{
    m_subsystem->Drive(0_mps, 0_mps, 10_deg_per_s, false, 1_m, 0_m);
}

void PirouetteCommand::Initialize() noexcept
{
    // Arrange the origin and orientation of the coordinate system, so that the
    // robot is on the unit circle, at (1,0) and facing positive X.  (The robot
    // does not move here, the coordinate system itself moves.)
    frc::Pose2d pose(1_m, 0_m, frc::Rotation2d(0_deg));

    m_subsystem->ResetOdometry(pose);
}

void PirouetteCommand::Execute() noexcept
{
    // Determine the direction the robot is facing, and where it is positioned.
    // As it happens, the direction the robot is facing is not used here.
    const frc::Pose2d pose = m_subsystem->GetPose();

    // Now, work out the angle of where the robot is positioned on the unit
    // circle, which is independent of where it is facing.  It may drift either
    // inside or outside of the unit circle.
    const double X = pose.X().to<double>();
    const double Y = pose.Y().to<double>();
    frc::Rotation2d major_angle(X, Y);

    // The robot should translate tangentially to the unit circle, in a
    // counter-clockwise fashion.  So, add 90 degrees.
    major_angle += 90_deg;

    // Work out the radius of the circle where robot is actually sitting, so it
    // may be used to make an adjustment to try to get back onto the unit
    // circle.  Positive error means robot is outside target circle, negative
    // means inside.  Outside correction is to increase the computed angle;
    // inside is to decrease.
    double major_error = std::hypot(X, Y) - 1.0;

    // Limit the maximum correction to apply and then apply it.
    if (major_error > 0.5)
    {
        major_error = 0.5;
    }
    else if (major_error < -0.5)
    {
        major_error = -0.5;
    }
    major_angle += major_error * 20_deg;

    // Update X and Y drive command inputs based on above computations, while
    // always commanding rotation.  The commanded drive needs to be adjusted to
    // account for the actual orientation of the robot, thus field oriented
    // applies.
    m_subsystem->Drive(major_angle.Sin() * 1_mps, major_angle.Cos() * 1_mps, 10_deg_per_s, true);
}