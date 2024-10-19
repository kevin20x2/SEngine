//
// Created by vinkzheng on 2024/10/19.
//

#include "PlayerController.h"

#include <spdlog/spdlog.h>

#include "CameraManager.h"
#include "Engine.h"

SPlayerController::SPlayerController()
{
    CameraManager = SNew <SCameraManager>();
}

void SPlayerController::InitCameraInput()
{
	auto Camera = CameraManager->GetCamera()->AsShared();
	GEngine->GetInput()->BindKey(GLFW_KEY_W, [WeakCamera = TWeakPtr<SCameraComponent>(Camera)]()
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos.z += 0.1f;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});

	GEngine->GetInput()->BindKey(GLFW_KEY_S, [WeakCamera = TWeakPtr<SCameraComponent>(Camera)]()
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos.z -= 0.1f;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});

	GEngine->GetInput()->BindScrollOperation([WeakCamera = TWeakPtr<SCameraComponent>(Camera)](double Value)
	{
		auto SharedCamera=WeakCamera.lock();
		if(SharedCamera != nullptr)
		{
			auto Forward = SharedCamera->GetForward();
			spdlog::info("dir: {}",ToString(Forward));
			float Scale = 10.0f;
			auto OldPos = SharedCamera->GetWorldLocation();
			OldPos += Scale *(float)(Value)* Forward;
			SharedCamera->SetWorldLocation(OldPos);
		}
	});

	Camera->SetWorldLocation({-300,0,250});
}
