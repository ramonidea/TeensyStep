#pragma once

#include <cstdint>
#include <algorithm>

class Stepper
{
    static const int32_t vMaxMax;      // largest speed possible (steps/s)
    static const uint32_t aMax;        // speed up to 500kHz within 1 s (steps/s^2)
    static const uint32_t vMaxDefault; // should work with every motor (1 rev/sec in 1/4-step mode)
    static const uint32_t aDefault;    // reasonably low (~0.5s for reaching the default speed)

  public:
    Stepper();
    Stepper(const int StepPin, const int DirPin);

    void setup(const int StepPin, const int DirPin);

    Stepper &setMaxSpeed(int32_t speed);   // steps/s
    Stepper &setAcceleration(uint32_t _a); // steps/s^2

    Stepper &setStepPinPolarity(int p);  // HIGH -> positive pulses, LOW -> negative pulses
    Stepper &setInverseRotation(bool b); // Change polarity of the dir pulse

    void setTargetAbs(int32_t pos);   // Set target position absolute
    void setTargetRel(int32_t delta); // Set target position relative to current position

    inline int32_t getPosition() const { return current; }
    inline void setPosition(int32_t pos) { current = pos; }
    int32_t dir;

  protected:
    inline void doStep();
    inline void clearStepPin() const;

    inline void setDir(int d);
    inline void toggleDir();

    volatile int32_t current;
    volatile int32_t currentSpeed;
    volatile int32_t target;

    int32_t A, B; // Bresenham paramters
    int32_t vMax;
    uint32_t a;

    // compare functions
    static bool cmpDelta(const Stepper *a, const Stepper *b) { return a->A > b->A; }
    static bool cmpAcc(const Stepper *a, const Stepper *b) { return a->a < b->a; }
    static bool cmpVmin(const Stepper *a, const Stepper *b) { return abs(a->vMax) < abs(b->vMax); }
    static bool cmpVmax(const Stepper *a, const Stepper *b) { return abs(a->vMax) > abs(b->vMax); }

    // Pin & Dir registers
    volatile uint32_t *stepPinActiveReg;
    volatile uint32_t *stepPinInactiveReg;
    volatile uint32_t *dirPinCwReg;
    volatile uint32_t *dirPinCcwReg;
    int stepPin, dirPin;

    // Friends
    template <typename a, typename t>
    friend class StepControlBase;

    template <typename a, typename t>
    friend class RotateControlBase;

    template <typename t>
    friend class MotorControlBase;
};

// Inline implementation -----------------------------------------

void Stepper::doStep()
{
    *stepPinActiveReg = 1;
    current += dir;
}
void Stepper::clearStepPin() const
{
    *stepPinInactiveReg = 1;
}

void Stepper::setDir(int d)
{
    dir = d;
    dir == 1 ? *dirPinCwReg = 1 : *dirPinCcwReg = 1;
}

void Stepper::toggleDir()
{
    setDir(-dir);
}
