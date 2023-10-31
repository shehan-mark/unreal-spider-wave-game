// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveGameManagerHub.h"
#include "WaveGameManagerBase.h"

void UWaveGameManagerHub::PostInitProperties()
{
	Super::PostInitProperties();
	ManagerInstances.Reserve(ManagerClasses.Num());
	ManagerLookupMap.Reserve(ManagerClasses.Num());

	// Construct instances of all the manager classes, and populate the quick access map
	for (const TSubclassOf<UWaveGameManagerBase>& ManagerClass : ManagerClasses)
	{
		UWaveGameManagerBase* Manager = NewObject<UWaveGameManagerBase>(this, ManagerClass);
		Manager->OwningManagerHub = this;
		check(Manager != nullptr);
		ManagerInstances.Add(Manager);
		
		// Add parent classes to quick access map
		UClass* CurrentClass = ManagerClass.Get();
		while (CurrentClass != nullptr && !CurrentClass->HasAnyClassFlags(EClassFlags::CLASS_Abstract))
		{
			check(!ManagerLookupMap.Contains(CurrentClass));
			ManagerLookupMap.Add(CurrentClass, Manager);
			CurrentClass = CurrentClass->GetSuperClass();
		}
	}
}

UWaveGameManagerBase* UWaveGameManagerHub::GetManagerByClass(TSubclassOf<class UWaveGameManagerBase> ManagerClass)
{
	UWaveGameManagerBase** ManagerPtr = ManagerLookupMap.Find(ManagerClass);
	return ManagerPtr ? *ManagerPtr : nullptr;
}
