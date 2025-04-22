#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarcodeScanner.generated.h"

UCLASS()
class BARCODESCANNERPLUGIN_API ABarcodeScanner : public AActor
{
    GENERATED_BODY()

public:
    ABarcodeScanner();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Barcode Scanner")
    void StartScanner();

    UFUNCTION(BlueprintCallable, Category = "Barcode Scanner")
    void StopScanner();

    UFUNCTION(BlueprintCallable, Category = "Barcode Scanner")
    FString GetLastScannedCode();

    UFUNCTION(BlueprintImplementableEvent, Category = "Barcode Scanner")
    void OnBarcodeScanned(const FString& Barcode);

private:
    void InitializeScanner();
    void ProcessScannedData(const FString& Data);

    bool bIsScannerActive;
    FString LastScannedCode;
}; 