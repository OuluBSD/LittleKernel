#if 0
#pragma once


NAMESPACE_UPP
using namespace Upp;

	
struct PaintStrokeComponent : Component
{
    struct Square
    {
        winrt::Windows::Foundation::Numerics::float3 TopLeft, TopRight, BottomLeft, BottomRight;
    };

    std::vector<Square> squares;
    bool strokeChanged{ true };

    void AddPoint(const mat4& transformationMatrix, float width);
    Pbr::PrimitiveBuilder GetPrimitiveData();
    
    COPY_PANIC(PaintStrokeComponent)
    
};


// PaintStrokeSystem
// This System manages the PaintStrokeComponents and automatically generates the 3D mesh for each stroke
class PaintStrokeSystem : public System
{
public:
    PaintStrokeSystem(Engine& core, std::shared_ptr<Pbr::Resources> pbrResources);
    ~PaintStrokeSystem() = default;

protected:
    void Update(double) override;

private:
    std::shared_ptr<Pbr::Resources> m_pbrResources{ nullptr };
};


END_UPP_NAMESPACE
#endif
