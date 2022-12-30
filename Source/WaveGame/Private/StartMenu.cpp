// Fill out your copyright notice in the Description page of Project Settings.


#include "StartMenu.h"
#include "MasterView.h"
#include "Components/Button.h"

void UStartMenu::NativeConstruct()
{
	if (StartButton && QuitButton)
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &UStartMenu::OnStartGamePressed);
		QuitButton->OnClicked.AddUniqueDynamic(this, &UStartMenu::OnQuitGamePressed);
	}
}

void UStartMenu::OnStartGamePressed()
{
	if (ParentViewRef != nullptr)
	{
		ParentViewRef->OnStartGame.Broadcast();
	}
}

void UStartMenu::OnQuitGamePressed()
{
	if (ParentViewRef != nullptr)
	{
		ParentViewRef->OnQuitGame.Broadcast();
	}
}
