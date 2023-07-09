// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu.h"
#include "Components/Button.h"

#include "UI/MasterView.h"

void UPauseMenu::NativeConstruct()
{
	if (ResumeButton && MainMenuButton)
	{
		ResumeButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::OnResumeGamePressed);
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::OnMainMenuPressed);
	}
}

void UPauseMenu::OnResumeGamePressed()
{
	ParentViewRef->OnResumeGame.Broadcast();
}

void UPauseMenu::OnMainMenuPressed()
{
	ParentViewRef->OnMainMenu.Broadcast();
}
