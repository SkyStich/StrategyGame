#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightedInterface.generated.h"

class APlayerController;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHighlightedInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STRATEGYGAME_API IHighlightedInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces", meta=(DisplayName="BP_HilighlightInterface"))
	void HighlightedActor(APlayerController* PlayerController);
};
