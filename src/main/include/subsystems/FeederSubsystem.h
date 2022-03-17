#pragma once

#include "subsystems/SparkMax.h"

#include <frc/DoubleSolenoid.h>
#include <frc2/command/SubsystemBase.h>

#include <memory>

class FeederSubsystem : public frc2::SubsystemBase
{
public:
    // The only ctor of the FeederSubsystem class.
    FeederSubsystem() noexcept;

    FeederSubsystem(const FeederSubsystem &) = delete;
    FeederSubsystem &operator=(const FeederSubsystem &) = delete;

    void Periodic() noexcept override;

    void Set(double percent) noexcept;

    void Pneumatics() noexcept;

    void LockIntake() noexcept;

    void DropIntake() noexcept;

    void LowerIntake() noexcept;

    void RaiseIntake() noexcept;

private:
    std::unique_ptr<SmartMotorBase> m_intakeMotor;
    std::unique_ptr<SmartMotorBase> m_elevatorMotor;
    std::unique_ptr<SmartMotorBase> m_feederMotor;
    std::unique_ptr<SmartMotorBase> m_shooterMotor;
    std::unique_ptr<SmartMotorBase> m_backspinMotor;
    std::unique_ptr<SmartMotorBase> m_climberMotor;

    std::unique_ptr<frc::DoubleSolenoid> m_intakeRelease;
    std::unique_ptr<frc::DoubleSolenoid> m_intakeRaise;
};
