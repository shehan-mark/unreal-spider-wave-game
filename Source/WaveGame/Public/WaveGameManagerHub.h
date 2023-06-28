// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveGameManagerHub.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class WAVEGAME_API UWaveGameManagerHub : public UObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class UWaveGameManagerBase>> ManagerClasses;

	UPROPERTY(Transient)
	TArray<class UWaveGameManagerBase*> ManagerInstances;

	/**
	* Map used by manager getter functions, so we don't have to check all manager instances every time.
	*/
	UPROPERTY(Transient)
	TMap<UClass*, class UWaveGameManagerBase*> ManagerLookupMap;

protected:

	virtual void PostInitProperties();

public:

	/**
	* Getter function for managers for use in blueprints. Return type is automatically converted in blueprints.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DeterminesOutputType = "ManagerClass"))
	UWaveGameManagerBase* GetManagerByClass(TSubclassOf<class UWaveGameManagerBase> ManagerClass);


	/**
	* Getter method to get managers from the hub
	*/
	template<class ManagerClass>
	FORCEINLINE ManagerClass* GetManager()
	{
		UWaveGameManagerBase** ManagerPtr = ManagerLookupMap.Find(ManagerClass::StaticClass());
		return ManagerPtr ? Cast<ManagerClass>(*ManagerPtr) : nullptr;
	}
};
