// Fill out your copyright notice in the Description page of Project Settings.


#include "TouchInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"

UTouchInputComponent::UTouchInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SwipeThreshold = 50.0f;
	PinchThreshold = 0.1f;
	LastPinchDistance = 0.0f;
}

void UTouchInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Подключаем обработку тачей через InputComponent владельца
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (PC->InputComponent)
		{
			PC->InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &UTouchInputComponent::HandleTouchBegan);
			PC->InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &UTouchInputComponent::HandleTouchMoved);
			PC->InputComponent->BindTouch(EInputEvent::IE_Released, this, &UTouchInputComponent::HandleTouchEnded);
		}
	}
}

void UTouchInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UTouchInputComponent::HandleMousePressed()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		FVector2D MousePosition;
		PC->GetMousePosition(MousePosition.X, MousePosition.Y);
		HandleTouchBegan(ETouchIndex::Touch1, FVector(MousePosition, 0));
	}
}

void UTouchInputComponent::HandleMouseReleased()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		FVector2D MousePosition;
		PC->GetMousePosition(MousePosition.X, MousePosition.Y);
		HandleTouchEnded(ETouchIndex::Touch1, FVector(MousePosition, 0));
	}
}

void UTouchInputComponent::HandleMouseMoved(float X, float Y)
{
	if (ActiveTouches.Num() > 0)
	{
		FVector2D MousePosition(X, Y);
		HandleTouchMoved(ETouchIndex::Touch1, FVector(MousePosition, 0));
	}
}

void UTouchInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTouchInputComponent::HandleTouchBegan(ETouchIndex::Type FingerIndex, FVector Location)
{
	FTouchData TouchData;
	TouchData.Location = FVector2D(Location.X, Location.Y);
	TouchData.FingerIndex = (int32)FingerIndex;
	TouchData.Time = GetWorld()->GetTimeSeconds();

	ActiveTouches.Add(TouchData);
	OnTouchBegan.Broadcast(TouchData);
}

void UTouchInputComponent::HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	for (FTouchData& Touch : ActiveTouches)
	{
		if (Touch.FingerIndex == (int32)FingerIndex)
		{
			Touch.Location = FVector2D(Location.X, Location.Y);
			OnTouchMoved.Broadcast(Touch);
			break;
		}
	}
}

void UTouchInputComponent::HandleTouchEnded(ETouchIndex::Type FingerIndex, FVector Location)
{
	for (int32 i = 0; i < ActiveTouches.Num(); ++i)
	{
		if (ActiveTouches[i].FingerIndex == (int32)FingerIndex)
		{
			FTouchData EndedTouch = ActiveTouches[i];
			ActiveTouches.RemoveAt(i);
			OnTouchEnded.Broadcast(EndedTouch);
			break;
		}
	}
}