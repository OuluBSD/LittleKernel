#if 0
#include "EcsWin.h"


NAMESPACE_UPP



using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Graphics::Holographic;
using namespace winrt::Windows::UI::Input::Spatial;


std::string ControllerModelKeyToString(const std::tuple<uint16_t, uint16_t, uint16_t, SpatialInteractionSourceHandedness>& tuple)
{
    std::stringstream ss;

    ss << "MotionController_"
        << std::get<0>(tuple) << "_"
        << std::get<1>(tuple) << "_"
        << std::get<2>(tuple) << "_"
        << static_cast<uint16_t>(std::get<3>(tuple));

    return ss.str();
}

std::future<void> LoadAndCacheModel(
    const SpatialInteractionSource& source,
    Engine& engine)
{
    const auto controllerModelName = ControllerModelKeyToString(ControllerRendering::GetControllerModelKey(source));

    auto pbrModelCache = engine.Get<PbrModelCache>();
    if (!pbrModelCache->ModelExists(controllerModelName.c_str()))
    {
        const auto pbrResources = engine.Get<HolographicRenderer>()->GetPbrResources();

        const auto model = co_await ControllerRendering::TryLoadRenderModelAsync(pbrResources, source);

        if (model) {
            pbrModelCache->RegisterModel(controllerModelName, model->Clone(*pbrResources));
        }
        else {
            debug_log("Failed to load model for source %d", source.Id());
        }
    }
}


END_UPP_NAMESPACE


NAMESPACE_UPP
using namespace ::Upp::Ecs;


void MotionControllerSystem::Start()
{
	Engine& m_mach = GetEngine();
	m_mach.Get<HolographicScene>()->AddPredictionUpdateListener(*this);
	
	Engine& m_engine = m_mach.Get<EntitySystem>()->GetEngine();
	m_engine.Get<SpatialInteractionSystem>()->AddListener(*this);
}

void MotionControllerSystem::OnPredictionUpdated(
	IPredictionUpdateListener::PredictionUpdateReason /*reason*/,
	const SpatialCoordinateSystem& coordinateSystem,
	const HolographicFramePrediction& prediction)
{
	Engine& mach = GetEngine();
	Engine& m_engine = mach;
	// Update the positions of the controllers based on the current timestamp.
	auto states = m_engine.Get<SpatialInteractionSystem>()->GetInteractionManager().GetDetectedSourcesAtTimestamp(prediction.Timestamp());
	PoolRef root =  m_engine.Get<EntityStore>()->GetRoot();
	for (const auto& sourceState : states)
	{
	    for (const auto& componentSet : root->GetComponents<Transform, MotionControllerComponent>())
	    {
	        auto[transform, controller] = componentSet;
	
	        RefreshComponentsForSource(sourceState.Source());
	
	        if (controller->IsSource(sourceState.Source()))
	        {
	            const SpatialInteractionSourceLocation location = sourceState.Properties().TryGetLocation(coordinateSystem);
	
	            controller->location = location;
	
	            if (location) {
	                transform->data.position = ToTs(location_util::position(location));
	                transform->data.orientation = ToTs(location_util::orientation(location));
	            }
	        }
	    }
	}
}

void MotionControllerSystem::Stop()
{
	Engine& mach = GetEngine();
	mach.Get<HolographicScene>()->RemovePredictionUpdateListener(*this);
	
	Engine& m_engine = mach.Get<EntitySystem>()->GetEngine();
	m_engine.Get<SpatialInteractionSystem>()->RemoveListener(*this);
}

void MotionControllerSystem::RefreshComponentsForSource(const SpatialInteractionSource& source)
{
	Engine& mach = GetEngine();
	Engine& eng = mach.GetEngine();
	PoolRef root = eng.Get<EntityStore>()->GetRoot();
	
	auto comps = root->GetComponentsWithEntity<MotionControllerComponent>();
	for (auto& componentSet : comps)
	{
	    auto[entity, controller] = componentSet;
	
	    fail_fast_if(controller->requestedHandedness == SpatialInteractionSourceHandedness::Unspecified, "Unspecified is not supported yet");
	
	    if (controller->source == nullptr && source.Handedness() == controller->requestedHandedness)
	    {
	        controller->source = source;
	        debug_log("Attached source id %d to entity %lld with requested handedness %d", source.Id(), entity->Id(), static_cast<uint32_t>(controller->requestedHandedness));
	    }
	}
}

void MotionControllerSystem::OnSourceUpdated(const SpatialInteractionSourceEventArgs& args)
{
	Engine& mach = GetEngine();
	Engine& eng = mach.GetEngine();
	PoolRef root = eng.Get<EntityStore>()->GetRoot();
	
	if (args.State().Source().Kind() == SpatialInteractionSourceKind::Controller)
	{
		auto comps = root->GetComponents<PbrRenderable, MotionControllerComponent>();
	    for (auto& componentSet : comps) {
	        auto[pbr, controller] = componentSet;
	
	        if (controller->IsSource(args.State().Source()) && controller->attachControllerModel) {
	            // If we don't have a model yet, set the ModelName so PbrModelCache will update the model
	            if (!pbr->Model) {
	                pbr->ResetModel(ControllerModelKeyToString(ControllerRendering::GetControllerModelKey(controller->source)));
	            }
	            else {
	                ControllerRendering::ArticulateControllerModel(ControllerRendering::GetArticulateValues(args.State()), *pbr->Model);
	            }
	        }
	    }
	}
}

void MotionControllerSystem::OnSourceDetected(const SpatialInteractionSourceEventArgs& args)
{
	Engine& mach = GetEngine();
	TODO
	/*Engine& ecs_engine = ;
	if (args.State().Source().Kind() == SpatialInteractionSourceKind::Controller) {
	    // Attempt to load any controller models into the PbrModelCache
	    (void)LoadAndCacheModel(args.State().Source(), ecs_engine);
	
	    // Update any components with their new Source
	    RefreshComponentsForSource(args.State().Source());
	}*/
}

void MotionControllerSystem::OnSourceLost(const SpatialInteractionSourceEventArgs& args)
{
	if (args.State().Source().Kind() == SpatialInteractionSourceKind::Controller) {
		PoolRef root = GetEngine().GetEngine().Get<EntityStore>()->GetRoot();
		auto comps = root->GetComponents<MotionControllerComponent>();
	    for (auto& componentSet : comps) {
	        auto[controller] = componentSet;
	
	        if (controller->IsSource(args.State().Source()))
	        {
	            controller->source = nullptr;
	            controller->location = nullptr;
	        }
	    }
	}
}


END_UPP_NAMESPACE




NAMESPACE_UPP


bool MotionControllerComponent::IsSource(const SpatialInteractionSource& rhs) const {
	return (this->source && rhs) ? this->source.Id() == rhs.Id() : false;
}


END_UPP_NAMESPACE

#endif
