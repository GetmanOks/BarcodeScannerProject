// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Containers/Map.h"
#include "Math/Box.h"
#include "CameraPawn.generated.h"

// Макросы для логирования
#define LOG_CAMERA(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("[CameraPawn] %s"), *FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LOG_CAMERA_INFO(Format, ...) LOG_CAMERA(Log, Format, ##__VA_ARGS__)
#define LOG_CAMERA_WARNING(Format, ...) LOG_CAMERA(Warning, Format, ##__VA_ARGS__)
#define LOG_CAMERA_ERROR(Format, ...) LOG_CAMERA(Error, Format, ##__VA_ARGS__)

UCLASS()
class SLIMCAPE_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
    ACameraPawn();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Camera|Positions")
    void SetCameraPosition(const FString& PositionName);

    UFUNCTION(BlueprintCallable, Category = "Camera|Positions")
    FVector GetCameraPosition(const FString& PositionName) const;

    UFUNCTION(BlueprintCallable, Category = "Camera|Positions")
    bool HasCameraPosition(const FString& PositionName) const;

protected:
    // Компоненты камеры
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComponent;

    // Параметры камеры
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraSpeed = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinZoomDistance = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxZoomDistance = 1000.0f;

    // Параметры автоматического вращения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    bool bEnableAutoRotation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    float AutoRotationSpeed = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    float AutoRotationRadius = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    float AutoRotationHeight = 200.0f;

    // Центр вращения
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    FVector RotationCenter = FVector::ZeroVector;

    // Параметры плавности
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    float RotationTransitionSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AutoRotation")
    float CurrentRotationAlpha = 0.0f;

    // Режим управления
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    bool bUseTouchInput = true;  // true - тач, false - мышь

    // Функция для переключения режима
    UFUNCTION(BlueprintCallable, Category = "Input")
    void ToggleInputMode();

    // Input Actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchPressAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchReleaseAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* TouchMoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    // Input Action для переключения режимов
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    class UInputAction* ToggleInputModeAction;

    // Функции обработки Enhanced Input
    void OnTouchPress(const FInputActionValue& Value);
    void OnTouchRelease(const FInputActionValue& Value);
    void OnTouchMove(const FInputActionValue& Value);

    // Функция обработки переключения режима
    void OnToggleInputMode(const FInputActionValue& Value);

    // Blueprint-функции для управления вращением
    UFUNCTION(BlueprintCallable, Category = "Camera|AutoRotation")
    void StartAutoRotation();

    UFUNCTION(BlueprintCallable, Category = "Camera|AutoRotation")
    void StopAutoRotation();

    UFUNCTION(BlueprintCallable, Category = "Camera|AutoRotation")
    void SetRotationCenter(const FVector& NewCenter);

    // Позиции камеры для разных уровней
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera|Positions")
    TMap<FString, FVector> CameraPositions;

    // Параметры для поиска объектов
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects")
    TArray<TSubclassOf<AActor>> TargetActorClasses;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects")
    float PaddingPercent = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects")
    float MinRotationRadius = 500.0f;

    // Дополнительные параметры настройки камеры
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects", Meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float RadiusMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects", Meta = (ClampMin = "0.1", ClampMax = "2.0"))
    float HeightMultiplier = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects")
    float MaxRotationRadius = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|SceneObjects")
    float MaxHeight = 1000.0f;

    // Функции для работы с объектами сцены
    UFUNCTION(BlueprintCallable, Category = "Camera|SceneObjects")
    void CalculateOptimalCameraPosition();

    UFUNCTION(BlueprintCallable, Category = "Camera|SceneObjects")
    FVector CalculateSceneCenter() const;

    UFUNCTION(BlueprintCallable, Category = "Camera|SceneObjects")
    float CalculateOptimalRadius() const;

    UFUNCTION(BlueprintCallable, Category = "Camera|SceneObjects")
    void UpdateCameraForCurrentLevel();

    // Функции для работы с позициями камеры
    UFUNCTION(BlueprintCallable, Category = "Camera|Positions")
    void MoveCameraToPosition(const FString& LevelName, bool bStartAutoRotation = true);

    // Обработчик события загрузки уровня
    virtual void NotifyOnLevelChange();

    // Параметры перемещения камеры
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    float CameraTransitionSpeed = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    bool bSmoothTransition = true;

private:
    // Состояние касаний
    bool bIsTouching;
    FVector2D TouchStart;
    FVector2D LastTouchLocation;
    float CurrentZoomDistance;

    // Переменные для плавного перемещения
    FVector TargetPosition;
    bool bIsMovingToPosition;
    float CurrentTransitionAlpha;

    // Вспомогательные функции
    TArray<AActor*> GetTargetActors() const;
    FBox GetTargetActorsBounds() const;
    float CalculateOptimalHeight(const FBox& Bounds) const;
};
