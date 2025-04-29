// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ACameraPawn::ACameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // Создаем SpringArm компонент
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    RootComponent = SpringArmComponent;
    SpringArmComponent->TargetArmLength = 500.0f;
    SpringArmComponent->bInheritPitch = false;
    SpringArmComponent->bInheritYaw = false;
    SpringArmComponent->bInheritRoll = false;
    SpringArmComponent->bDoCollisionTest = true;
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->CameraLagSpeed = 3.0f;

    // Создаем Camera компонент
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // Инициализация переменных
    bIsTouching = false;
    CurrentZoomDistance = SpringArmComponent->TargetArmLength;

    LOG_CAMERA_INFO("CameraPawn created with SpringArm length: %f", CurrentZoomDistance);
}

void ACameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Устанавливаем начальную позицию камеры
    UpdateCameraForCurrentLevel();

    // Enhanced Input
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    LOG_CAMERA_INFO("CameraPawn started with controller: %s",
        GetController() ? *GetController()->GetName() : TEXT("None"));
}

void ACameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Обработка плавного перемещения к позиции
    if (bIsMovingToPosition)
    {
        CurrentTransitionAlpha += DeltaTime * CameraTransitionSpeed;
        if (CurrentTransitionAlpha >= 1.0f)
        {
            CurrentTransitionAlpha = 1.0f;
            bIsMovingToPosition = false;
        }

        FVector NewLocation = FMath::Lerp(
            GetActorLocation(),
            TargetPosition,
            CurrentTransitionAlpha
        );
        SetActorLocation(NewLocation);
    }

    // Обработка плавного включения/выключения вращения
    if (bEnableAutoRotation && CurrentRotationAlpha < 1.0f)
    {
        CurrentRotationAlpha = FMath::Min(CurrentRotationAlpha + DeltaTime * RotationTransitionSpeed, 1.0f);
    }
    else if (!bEnableAutoRotation && CurrentRotationAlpha > 0.0f)
    {
        CurrentRotationAlpha = FMath::Max(CurrentRotationAlpha - DeltaTime * RotationTransitionSpeed, 0.0f);
    }

    // Обработка автоматического вращения
    if (CurrentRotationAlpha > 0.0f)
    {
        // Вычисляем новую позицию камеры по кругу
        float CurrentTime = GetWorld()->GetTimeSeconds();
        float Angle = CurrentTime * AutoRotationSpeed;
        
        FVector NewLocation = RotationCenter + FVector(
            FMath::Cos(Angle) * AutoRotationRadius,
            FMath::Sin(Angle) * AutoRotationRadius,
            AutoRotationHeight
        );
        
        // Интерполируем между текущей позицией и целевой
        FVector CurrentLocation = GetActorLocation();
        FVector InterpolatedLocation = FMath::Lerp(
            CurrentLocation,
            NewLocation,
            CurrentRotationAlpha
        );
        
        // Устанавливаем новую позицию
        SetActorLocation(InterpolatedLocation);
        
        // Направляем камеру в центр
        FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(InterpolatedLocation, RotationCenter);
        SetActorRotation(NewRotation);
        
        // Обновляем Spring Arm для сохранения направления камеры
        SpringArmComponent->SetWorldRotation(NewRotation);
    }

    // Обработка изменения зума
    if (SpringArmComponent->TargetArmLength != CurrentZoomDistance)
    {
        float NewLength = FMath::FInterpTo(
            SpringArmComponent->TargetArmLength,
            CurrentZoomDistance,
            DeltaTime,
            5.0f
        );
        SpringArmComponent->TargetArmLength = NewLength;
    }
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced Input
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(TouchPressAction, ETriggerEvent::Started, this, &ACameraPawn::OnTouchPress);
        EnhancedInputComponent->BindAction(TouchReleaseAction, ETriggerEvent::Completed, this, &ACameraPawn::OnTouchRelease);
        EnhancedInputComponent->BindAction(TouchMoveAction, ETriggerEvent::Triggered, this, &ACameraPawn::OnTouchMove);

        //  Enhanced Input for Enhanced Input
        EnhancedInputComponent->BindAction(ToggleInputModeAction, ETriggerEvent::Started, this, &ACameraPawn::OnToggleInputMode);
    }

    LOG_CAMERA_INFO("Enhanced Input bindings set up");
}

void ACameraPawn::ToggleInputMode()
{
    bUseTouchInput = !bUseTouchInput;
    LOG_CAMERA_INFO("Input mode toggled to: %s", bUseTouchInput ? TEXT("Touch") : TEXT("Mouse"));
}

void ACameraPawn::OnTouchPress(const FInputActionValue& Value)
{
    bIsTouching = true;
    const FVector2D TouchPosition = Value.Get<FVector2D>();
    TouchStart = TouchPosition;
    LastTouchLocation = TouchPosition;

    LOG_CAMERA_INFO("Touch pressed at location: X=%.2f, Y=%.2f",
        TouchStart.X, TouchStart.Y);
}

void ACameraPawn::OnTouchRelease(const FInputActionValue& Value)
{
    bIsTouching = false;
    const FVector2D TouchPosition = Value.Get<FVector2D>();

    LOG_CAMERA_INFO("Touch released at location: X=%.2f, Y=%.2f",
        TouchPosition.X, TouchPosition.Y);
}

void ACameraPawn::OnTouchMove(const FInputActionValue& Value)
{
    if (!bIsTouching) return;

    const FVector2D TouchPosition = Value.Get<FVector2D>();
    FVector2D Delta = TouchPosition - LastTouchLocation;
    LastTouchLocation = TouchPosition;

    //  Enhanced Input for Enhanced Input
    float ZoomDelta = -Delta.Y * ZoomSpeed;
    CurrentZoomDistance = FMath::Clamp(
        CurrentZoomDistance + ZoomDelta,
        MinZoomDistance,
        MaxZoomDistance
    );

    LOG_CAMERA_INFO("Touch moved - Delta: X=%.2f, Y=%.2f, New zoom: %.2f",
        Delta.X, Delta.Y, CurrentZoomDistance);
}

void ACameraPawn::OnToggleInputMode(const FInputActionValue& Value)
{
    ToggleInputMode();
}

void ACameraPawn::StartAutoRotation()
{
    bEnableAutoRotation = true;
    LOG_CAMERA_INFO("Auto rotation started");
}

void ACameraPawn::StopAutoRotation()
{
    bEnableAutoRotation = false;
    LOG_CAMERA_INFO("Auto rotation stopped");
}

void ACameraPawn::SetRotationCenter(const FVector& NewCenter)
{
    RotationCenter = NewCenter;
    LOG_CAMERA_INFO("Rotation center set to: X=%.2f, Y=%.2f, Z=%.2f", 
        NewCenter.X, NewCenter.Y, NewCenter.Z);
}

void ACameraPawn::AddCameraPosition(const FString& LevelName, const FVector& Position)
{
    CameraPositions.Add(LevelName, Position);
    LOG_CAMERA_INFO("Added camera position for level %s: X=%.2f, Y=%.2f, Z=%.2f",
        *LevelName, Position.X, Position.Y, Position.Z);
}

void ACameraPawn::RemoveCameraPosition(const FString& LevelName)
{
    CameraPositions.Remove(LevelName);
    LOG_CAMERA_INFO("Removed camera position for level %s", *LevelName);
}

bool ACameraPawn::TryGetCameraPosition(const FString& LevelName, FVector& OutPosition) const
{
    const FVector* Position = CameraPositions.Find(LevelName);
    if (Position)
    {
        OutPosition = *Position;
        return true;
    }
    return false;
}

void ACameraPawn::MoveCameraToPosition(const FString& LevelName, bool bStartAutoRotation)
{
    const FVector* Position = CameraPositions.Find(LevelName);
    if (Position)
    {
        // Устанавливаем позицию камеры
        SetActorLocation(*Position);

        // Устанавливаем центр вращения
        SetRotationCenter(*Position);

        // Включаем автоматическое вращение, если требуется
        if (bStartAutoRotation)
        {
            StartAutoRotation();
        }

        LOG_CAMERA_INFO("Camera moved to position for level %s: X=%.2f, Y=%.2f, Z=%.2f",
            *LevelName, Position->X, Position->Y, Position->Z);
    }
    else
    {
        LOG_CAMERA_WARNING("No camera position found for level %s", *LevelName);
    }
}

void ACameraPawn::NotifyOnLevelChange()
{
    // При изменении уровня обновляем позицию камеры
    UpdateCameraForCurrentLevel();
    LOG_CAMERA_INFO("Level changed, updating camera position");
}

TArray<AActor*> ACameraPawn::GetTargetActors() const
{
    TArray<AActor*> TargetActors;
    
    if (UWorld* World = GetWorld())
    {
        // Если классы не заданы, ищем все акторы
        if (TargetActorClasses.Num() == 0)
        {
            UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), TargetActors);
        }
        else
        {
            // Ищем акторы заданных классов
            for (auto ActorClass : TargetActorClasses)
            {
                TArray<AActor*> ActorsOfClass;
                UGameplayStatics::GetAllActorsOfClass(World, ActorClass, ActorsOfClass);
                TargetActors.Append(ActorsOfClass);
            }
        }
    }

    return TargetActors;
}

FBox ACameraPawn::GetTargetActorsBounds() const
{
    FBox Bounds(ForceInit);
    TArray<AActor*> Actors = GetTargetActors();

    // Собираем границы всех объектов
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            FVector Origin;
            FVector BoxExtent;
            Actor->GetActorBounds(false, Origin, BoxExtent);
            Bounds += FBox::BuildAABB(Origin, BoxExtent);
        }
    }

    return Bounds;
}

float ACameraPawn::CalculateOptimalHeight(const FBox& Bounds) const
{
    // Высота будет зависеть от размера объектов
    float BoundsHeight = Bounds.GetSize().Z;
    float BaseHeight = Bounds.GetCenter().Z + BoundsHeight * (1.0f + PaddingPercent / 100.0f);
    
    // Применяем множитель и ограничиваем максимальную высоту
    float DesiredHeight = BaseHeight * HeightMultiplier;
    return FMath::Min(DesiredHeight, MaxHeight);
}

FVector ACameraPawn::CalculateSceneCenter() const
{
    FBox Bounds = GetTargetActorsBounds();
    return Bounds.GetCenter();
}

float ACameraPawn::CalculateOptimalRadius() const
{
    FBox Bounds = GetTargetActorsBounds();
    FVector Size = Bounds.GetSize();
    
    // Базовый радиус зависит от максимального размера объектов
    float MaxSize = FMath::Max(Size.X, Size.Y);
    float BaseRadius = MaxSize * (1.0f + PaddingPercent / 100.0f);
    
    // Применяем множитель и ограничиваем минимальный и максимальный радиус
    float DesiredRadius = BaseRadius * RadiusMultiplier;
    return FMath::Clamp(DesiredRadius, MinRotationRadius, MaxRotationRadius);
}

void ACameraPawn::CalculateOptimalCameraPosition()
{
    FVector Center = CalculateSceneCenter();
    float Radius = CalculateOptimalRadius();
    float Height = CalculateOptimalHeight(GetTargetActorsBounds());

    // Устанавливаем центр вращения
    SetRotationCenter(Center);

    // Обновляем параметры автоматического вращения
    AutoRotationRadius = Radius;
    AutoRotationHeight = Height - Center.Z; // Корректируем высоту относительно центра

    // Устанавливаем начальную позицию камеры
    FVector NewLocation = Center + FVector(Radius, 0.0f, Height - Center.Z);
    SetActorLocation(NewLocation);

    LOG_CAMERA_INFO("Optimal camera position calculated: Center=(%.1f, %.1f, %.1f), Radius=%.1f, Height=%.1f",
        Center.X, Center.Y, Center.Z, Radius, Height);
}

void ACameraPawn::UpdateCameraForCurrentLevel()
{
    // Сначала пробуем найти предустановленную позицию
    if (UWorld* World = GetWorld())
    {
        FString CurrentLevelName = World->GetMapName();
        const FVector* Position = CameraPositions.Find(CurrentLevelName);
        
        if (Position)
        {
            MoveCameraToPosition(CurrentLevelName);
        }
        else
        {
            // Если позиция не найдена, вычисляем оптимальную
            CalculateOptimalCameraPosition();
            StartAutoRotation();
        }
    }
}