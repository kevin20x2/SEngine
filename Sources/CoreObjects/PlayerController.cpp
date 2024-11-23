//
// Created by vinkzheng on 2024/10/19.
//

#include "PlayerController.h"

#include <spdlog/spdlog.h>

#include "CameraComponent.h"
#include "CameraManager.h"
#include "Engine.h"
#include "Core/Log.h"
#include "RHI/RHI.h"

SPlayerController::SPlayerController()
{
    CameraManager = SNew <SCameraManager>();
}

void SPlayerController::InitCameraInput()
{
	auto Camera = CameraManager->GetCamera()->AsShared();
	GEngine->GetInput()->GetOnKeyPressing().AddLambda([this ,WeakCamera = TWeakPtr<SCameraComponent>(Camera)]
		(int32 Key)
	{
		auto SharedCamera = WeakCamera.lock();
		if (SharedCamera == nullptr) return ;
		auto OldPos = SharedCamera->GetWorldLocation();
		if(Key  == GLFW_KEY_W)
		{
			OldPos += SharedCamera->GetUp()* MoveSensitivity;
		}
		else if(Key == GLFW_KEY_S)
		{
			OldPos += -1.0f* SharedCamera->GetUp()* MoveSensitivity;
		}
		else if(Key == GLFW_KEY_A)
		{
			OldPos += -1.0f* SharedCamera->GetRight() * MoveSensitivity;
		}
		else if(Key == GLFW_KEY_D)
		{
			OldPos += SharedCamera->GetRight()* MoveSensitivity;
		}
		SharedCamera->SetWorldLocation(OldPos);
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

	GEngine->GetInput()->RegisterDragBegin([this,
		WeakCamera = TWeakPtr<SCameraComponent>(Camera)
		](EMouseType Type)
	{
		auto SharedCamera = WeakCamera.lock();
		if(SharedCamera)
		{
			BeginRotation = SharedCamera->GetWorldRotation();
		}
	});

	GEngine->GetInput()->RegisterDragging([this,
		WeakCamera = TWeakPtr<SCameraComponent>(Camera)
		](EMouseType Type ,float DeltaX ,float DeltaY)
	{
		auto SharedCamera = WeakCamera.lock();
		if(SharedCamera)
		{
			auto Size =  GRHI->GetDisplaySize();
			float XRatio =  DeltaX / Size.width;
			float YRatio =  DeltaY / Size.height;
			SLogD(TEXT("XRatio {}, YRatio {}"),XRatio,YRatio);

			// pitch, yaw, roll
			FVector EulerAngles = FVector(0,
				  YRotationSensitivity *  YRatio * FMath::PI ,
				 XRotationSensitivity * XRatio *FMath::PI  ); //0.5f * XRatio *FMath::PI );
			FQuat NewRotation (EulerAngles);
			SharedCamera->SetRotation(BeginRotation* NewRotation);
		}
	} );

	Camera->SetWorldLocation({-300,0,250});
}
