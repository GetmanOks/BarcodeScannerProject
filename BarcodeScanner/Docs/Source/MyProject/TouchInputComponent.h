// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "TouchInputComponent.generated.h"

// Структура для данных о тач-событии
USTRUCT(BlueprintType)
struct FTouchData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVector2D Location;

	UPROPERTY(BlueprintReadOnly)
	int32 FingerIndex;

	UPROPERTY(BlueprintReadOnly)
	float Time;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SLIMCAPE_API UTouchInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTouchInputComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Делегаты для тач-событий
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchBegan, const FTouchData&, TouchData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchMoved, const FTouchData&, TouchData);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTouchEnded, const FTouchData&, TouchData);

	UPROPERTY(BlueprintAssignable)
	FOnTouchBegan OnTouchBegan;

	UPROPERTY(BlueprintAssignable)
	FOnTouchMoved OnTouchMoved;

	UPROPERTY(BlueprintAssignable)
	FOnTouchEnded OnTouchEnded;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// Параметры для мышиной эмуляции
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MouseSmoothing = 0.1f;

	// Функции для мышиной эмуляции
	void HandleMousePressed();
	void HandleMouseReleased();
	void HandleMouseMoved(float X, float Y);

private:
	TArray<FTouchData> ActiveTouches;
	float LastPinchDistance;
	FVector2D LastSwipeDirection;
	float SwipeThreshold;
	float PinchThreshold;

	// Методы для обработки тачей
	void HandleTouchBegan(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchMoved(ETouchIndex::Type FingerIndex, FVector Location);
	void HandleTouchEnded(ETouchIndex::Type FingerIndex, FVector Location);

};