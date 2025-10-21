#if 0
#pragma once


NAMESPACE_UPP


////////////////////////////////////////////////////////////////////////////////
// PhysicsSystem
// Simple physics system to move objects around with basic integration (acceleration, velocity)
class PhysicsSystem : public System
{
public:
    using System::System;

    static const winrt::Windows::Foundation::Numerics::float3 EarthGravity;

protected:
    void Update(double dt) override;
    
};


END_UPP_NAMESPACE
#endif
