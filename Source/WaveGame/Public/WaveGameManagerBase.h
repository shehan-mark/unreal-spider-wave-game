// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveGameManagerBase.generated.h"

/**
 * Base class for Manager objects
 * This will be an abstract class which other classes will use as a template.
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class WAVEGAME_API UWaveGameManagerBase : public UObject
{
	GENERATED_BODY()
public:
	
	friend class UWaveGameManagerHub;

	/*UWaveGameManagerBase();
	~UWaveGameManagerBase();*/

protected:
	/**
	* If this manager was constructed by a UWaveGameManagerHub, this pointer will automatically be populated.
	*/
	UPROPERTY(BlueprintReadOnly)
	class UWaveGameManagerHub* OwningManagerHub = nullptr;
};
