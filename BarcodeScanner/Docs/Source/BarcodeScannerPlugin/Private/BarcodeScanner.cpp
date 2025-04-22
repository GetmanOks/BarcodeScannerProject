#include "BarcodeScanner.h"
#include "HID.h"

ABarcodeScanner::ABarcodeScanner()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsScannerActive = false;
}

void ABarcodeScanner::BeginPlay()
{
    Super::BeginPlay();
    InitializeScanner();
}

void ABarcodeScanner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsScannerActive)
    {
        // Здесь будет код для чтения данных со сканера
        // Пока что это заглушка
    }
}

void ABarcodeScanner::StartScanner()
{
    bIsScannerActive = true;
}

void ABarcodeScanner::StopScanner()
{
    bIsScannerActive = false;
}

FString ABarcodeScanner::GetLastScannedCode()
{
    return LastScannedCode;
}

void ABarcodeScanner::InitializeScanner()
{
    // Здесь будет код инициализации сканера
    // Пока что это заглушка
}

void ABarcodeScanner::ProcessScannedData(const FString& Data)
{
    LastScannedCode = Data;
    OnBarcodeScanned(Data);
} 